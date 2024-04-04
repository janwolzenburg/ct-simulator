/*********************************************************************
 * @file   gantry.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/



 /*********************************************************************
   Includes
*********************************************************************/
#include <thread>
using std::ref;
using std::cref;

#include "gantry.h"
#include "coordinateSystemTree.h"
#include "tomography.h"
#include "simulation.h"
#include "projectionsProperties.h"


/*********************************************************************
  Implementations
*********************************************************************/

Gantry::Gantry( CoordinateSystem* const coordinate_system, const XRayTubeProperties tubeParameter_, 
				const ProjectionsProperties radonParameter, const PhysicalDetectorProperties indipendentParameter ) :
	coordinate_system_( coordinate_system ),
	initial_position_( coordinate_system_->GetPrimitive() ),
	detector_{ coordinate_system_->AddCoordinateSystem( Primitivevector3{ 0, 0, 0 }, Primitivevector3{ 1, 0, 0 }, Primitivevector3{ 0, -1, 0 }, Primitivevector3{ 0, 0, 1 }, "xRay detector" ),
					radonParameter, indipendentParameter },
	tube_{ coordinate_system_->AddCoordinateSystem( Primitivevector3{ 0, 0, 0}, Primitivevector3{1, 0, 0}, Primitivevector3{0, -1, 0}, Primitivevector3{0, 0, 1}, "xRay tube"), tubeParameter_ }

{
	// align detector - tube axis with x axis
	PrimitiveCoordinateSystem xAxisAligned{ Primitivevector3{ 0, 0, 0 }, Primitivevector3{ 0, 1, 0 }, Primitivevector3{ 1, 0, 0 }, Primitivevector3{ 0, 0, 1 } };
	coordinate_system_->SetPrimitive( xAxisAligned );

	tube_.coordinate_system()->Translate( vector3D{ Tuple3D{ 0, detector_.properties().detector_focus_distance / 2, 0 }, coordinate_system_ } );
	
}


void Gantry::UpdateTubeAndDetectorProperties( const XRayTubeProperties tube_properties, const ProjectionsProperties projections_properties,
									const PhysicalDetectorProperties physical_detector_properties ){
	
	tube_.UpdateProperties( tube_properties );
	tube_.coordinate_system()->SetPrimitive( PrimitiveCoordinateSystem{ Primitivevector3{ 0, physical_detector_properties.detector_focus_distance / 2, 0 }, Primitivevector3{ 0, 1, 0 }, Primitivevector3{ 1, 0, 0 }, Primitivevector3{ 0, 0, 1 } } );
	
	detector_.UpdateProperties( projections_properties, physical_detector_properties );

	ResetGantry();

}

void Gantry::RotateCounterClockwise( const double arc_angle ){
	this->coordinate_system_->Rotate( coordinate_system_->GetZAxis(), arc_angle );
}

void Gantry::TranslateInZDirection( const double distance ){
	this->coordinate_system_->Translate( coordinate_system_->GetEz() * distance );
}

void Gantry::TransmitRaysThreaded(	
						   const Model& model, const TomographyProperties& tomography_properties,
							 const RayScattering& ray_scattering,
							 const vector<Ray>& rays, const bool second_to_last_iteration,
							 size_t& shared_current_ray_index, mutex& current_ray_index_mutex,
							 vector<Ray>& rays_for_next_iteration, mutex& rays_for_next_iteration_mutex,
							 XRayDetector& detector, mutex& detector_mutex,
							 RandomNumberGenerator& dedicated_rng ){

	size_t local_ray_index;
	Ray current_ray;
	pair<Ray, vector<Ray>> rays_to_return;

	// loop while rays are left
	while( shared_current_ray_index < rays.size() ){

		// get the ray which should be transmitted next and increment index
		current_ray_index_mutex.lock();
		local_ray_index = shared_current_ray_index++;
		current_ray_index_mutex.unlock();

		// no more rays left
		if( local_ray_index >= rays.size() ) break;

		// get current ray
		current_ray =  rays.at( local_ray_index );

		// transmit ray through model
		rays_to_return = std::move( 
			model.TransmitRay( cref( current_ray ), cref( tomography_properties ), 
												 cref( ray_scattering ), 
												 ref( dedicated_rng ) ) );

		// detect the ray
		detector.DetectRay( ref( rays_to_return.first ), ref( detector_mutex ) );
		
		if( rays_to_return.second.empty() ){
			continue;
		}

		// check if this is last iteration and delete rays which can not be detected
		if( second_to_last_iteration ){
			
			// check each ray
			for( auto& ray : rays_to_return.second ){
				// detection possible. ray's expected pixel index is updated in TryDetection()
				if( detector.TryDetection( ray ) ){
					// only rays which can be detected are in the next iteration
					rays_for_next_iteration_mutex.lock();
					rays_for_next_iteration.emplace_back( std::move( ray ) );									
					rays_for_next_iteration_mutex.unlock();
				}
			}
		}
		else{
			// add all rays for next iteration
			rays_for_next_iteration_mutex.lock();
			rays_for_next_iteration.insert( 
				rays_for_next_iteration.end(), 
				make_move_iterator( rays_to_return.second.begin() ), 
				make_move_iterator( rays_to_return.second.end() ) );									
			rays_for_next_iteration_mutex.unlock();
		}
	
	}

	return;
}

void Gantry::RadiateModel( const Model& model, 
													 TomographyProperties tomography_properties,
													 const RayScattering& scattering_information ) {

	// current rays. start with rays from source
	vector<Ray> rays = std::move( 
		tube_.GetEmittedBeam( 
			detector_.pixel_array(), 
			detector_.properties().detector_focus_distance ) );		
	
	// convert rays to model's coordinate system
	for( Ray& current_ray : rays ){
		current_ray = std::move( current_ray.ConvertTo( model.coordinate_system() ) );
	}
	
	// convert pixel
	detector_.ConvertPixelArray( model.coordinate_system() );

	// scattered rays should lie in the same plane as the detector 
	const Unitvector3D scattering_rotation_axis = 
		this->coordinate_system_->GetEz().ConvertTo( model.coordinate_system() );


	detector_.ResetDetectedRayPorperties(); // reset all pixel

	size_t shared_current_ray_index = 0;	// index of next ray to iterate
	mutex current_ray_index_mutex;				// mutual exclusion for ray index
	mutex rays_for_next_iteration_mutex;	// mutual exclusion for ray storage
	mutex detector_mutex;									// mutual exclusion for detector

	// loop until maximum loop depth is reached or no more rays are left to transmit
	for( size_t current_loop = 0; 
							current_loop <= tomography_properties.max_scattering_occurrences && 
														 rays.size() > 0; 
							current_loop++ ){

		// no scattering in last iteration
		tomography_properties.scattering_enabled = 
			current_loop < tomography_properties.max_scattering_occurrences && 
			tomography_properties.scattering_enabled;	
		
		const bool second_to_last_iteration = 
			( current_loop == tomography_properties.max_scattering_occurrences - 1 );

		// store for information output
		tomography_properties.mean_energy_of_tube = this->tube_.GetMeanEnergy();

		vector<Ray> rays_for_next_iteration;				// rays to process in the next iteration
		shared_current_ray_index = 0;								// reset current ray index

		const size_t number_of_threads = std::thread::hardware_concurrency();

		vector<RandomNumberGenerator> dedicated_rngs( number_of_threads );

		// start threads
		vector<std::thread> threads;
		for( size_t thread_index = 0; 
								thread_index < number_of_threads; thread_index++ ){

			// transmit rays
			threads.emplace_back( TransmitRaysThreaded,	
														cref( model ), cref( tomography_properties ), 
														cref( scattering_information ),
														cref( rays ),  second_to_last_iteration,
														ref( shared_current_ray_index ), 
														ref( current_ray_index_mutex ), ref( rays_for_next_iteration ), 
														ref( rays_for_next_iteration_mutex ),
														ref( detector_ ), ref( detector_mutex ),
														ref( dedicated_rngs.at( thread_index ) ) );

			// for debugging
			if( thread_index == 0 ) first_thread_id = threads.back().get_id();

		}

		// wait for threads to finish
		for( std::thread& currentThread : threads ) currentThread.join();
		
		rays = std::move( rays_for_next_iteration );

	}
}

void Gantry::ResetGantry( void ){
	
	// set to initial position
	coordinate_system_->SetPrimitive( initial_position_ );

	// reset detector
	detector_.ResetDetectedRayPorperties();
}

