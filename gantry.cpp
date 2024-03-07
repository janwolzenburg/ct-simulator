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
	detector_{ coordinate_system_->AddCoordinateSystem( PrimitiveVector3{ 0, 0, 0 }, PrimitiveVector3{ 1, 0, 0 }, PrimitiveVector3{ 0, -1, 0 }, PrimitiveVector3{ 0, 0, 1 }, "xRay detector" ),
					radonParameter, indipendentParameter },
	tube_{ coordinate_system_->AddCoordinateSystem( PrimitiveVector3{ 0, 0, 0}, PrimitiveVector3{1, 0, 0}, PrimitiveVector3{0, -1, 0}, PrimitiveVector3{0, 0, 1}, "xRay tube"), tubeParameter_ }

{
	// Align detector - tube axis with x axis
	PrimitiveCoordinateSystem xAxisAligned{ PrimitiveVector3{ 0, 0, 0 }, PrimitiveVector3{ 0, 1, 0 }, PrimitiveVector3{ 1, 0, 0 }, PrimitiveVector3{ 0, 0, 1 } };
	coordinate_system_->SetPrimitive( xAxisAligned );

	tube_.coordinate_system()->Translate( Vector3D{ Tuple3D{ 0, detector_.properties().detector_focus_distance / 2, 0 }, coordinate_system_ } );
	
}


void Gantry::UpdateTubeAndDetectorProperties( const XRayTubeProperties tube_properties, const ProjectionsProperties radon_properties,
									const PhysicalDetectorProperties physical_detector_properties ){
	
	tube_.UpdateProperties( tube_properties );
	tube_.coordinate_system()->SetPrimitive( PrimitiveCoordinateSystem{ PrimitiveVector3{ 0, physical_detector_properties.detector_focus_distance / 2, 0 }, PrimitiveVector3{ 0, 1, 0 }, PrimitiveVector3{ 1, 0, 0 }, PrimitiveVector3{ 0, 0, 1 } } );
	
	detector_.UpdateProperties( radon_properties, physical_detector_properties );

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
									RayScattering& ray_scattering, mutex& scattering_properties_mutex,
									const vector<Ray>& rays, const bool repeat_transmission_after_scattering,
									size_t& shared_current_ray_index, mutex& current_ray_index_mutex,
									vector<Ray>& rays_for_next_iteration, mutex& rays_for_next_iteration_mutex,
									XRayDetector& detector, mutex& detector_mutex ){

	size_t local_ray_index;
	Ray current_ray;
	pair<Ray, vector<Ray>> rays_to_return;

	// Loop while rays are left
	while( shared_current_ray_index < rays.size() ){

		// Get the Ray which should be transmitted next and increment index
		current_ray_index_mutex.lock();
		local_ray_index = shared_current_ray_index++;
		current_ray_index_mutex.unlock();

		// No more rays left
		if( local_ray_index >= rays.size() ) break;

		// Get current ray
		current_ray =  rays.at( local_ray_index );

		// Transmit Ray through model
		rays_to_return = std::move( model.TransmitRay( 
																				cref( current_ray ), cref( tomography_properties ), 
																				ref( ray_scattering ), 
																				ref( scattering_properties_mutex ) ) );

		detector.DetectRay( ref( rays_to_return.first ), ref( detector_mutex ) );
		
		// Add Ray for next iteration
		rays_for_next_iteration_mutex.lock();
		rays_for_next_iteration.insert( rays_for_next_iteration.end(), 
																		make_move_iterator( rays_to_return.second.begin() ), 
																		make_move_iterator( rays_to_return.second.end() ) );									
		rays_for_next_iteration_mutex.unlock();

	}

	return;
}

void Gantry::RadiateModel( const Model& model, TomographyProperties tomography_properties ) {

// Current rays. Start with rays from source
	vector<Ray> rays = std::move( tube_.GetEmittedBeam( 
																				detector_.pixel_array(), 
																				detector_.properties().detector_focus_distance ) );		
	
	// Convert rays to model coordinate system
	for( Ray& current_ray : rays ){
		current_ray = std::move( current_ray.ConvertTo( model.coordinate_system() ) );
	}
	
	// Convert pixel
	detector_.ConvertPixelArray( model.coordinate_system() );

	// Scattered rays should lie in the same plane as the detector 
	const UnitVector3D scattering_rotation_axis = 
		this->coordinate_system_->GetEz().ConvertTo( model.coordinate_system() );

	RayScattering scattering_information{ 
		simulation_properties.number_of_scatter_angles, 
		tube_.GetEmittedEnergyRange(), 
		simulation_properties.number_of_energies_for_scattering, 
		coordinate_system_->GetEz(), 
		atan( this->detector_.properties().row_width / 
					this->detector_.properties().detector_focus_distance ) };

	detector_.ResetDetectedRayPorperties();								// Reset all pixel

	size_t shared_current_ray_index = 0;	// Index of next Ray to iterate
	mutex current_ray_index_mutex;				// Mutex for Ray index
	mutex rays_for_next_iteration_mutex;	// Mutex for vector of rays for next iteration
	mutex detector_mutex;									// Mutex for detector
	mutex scattering_mutex;								// Mutex for scattering

	// Loop until maximum loop depth is reached or no more rays are left to transmit
	for( size_t current_loop = 0; 
							current_loop <= tomography_properties.max_scattering_occurrences && 
														 rays.size() > 0; 
							current_loop++ ){

		// No scattering in last iteration
		tomography_properties.scattering_enabled = 
			current_loop < tomography_properties.max_scattering_occurrences && 
			tomography_properties.scattering_enabled;	
		
		// Store for information output
		tomography_properties.mean_energy_of_tube = this->tube_.GetMeanEnergy();

		vector<Ray> rays_for_next_iteration;						// Rays to process in the next iteration
		shared_current_ray_index = 0;										// Reset current ray index

		// Start threads
		vector<std::thread> threads;
		for( size_t thread_index = 0; 
								thread_index < std::thread::hardware_concurrency(); thread_index++ ){
			
			threads.emplace_back( TransmitRaysThreaded,	
														cref( model ), cref( tomography_properties ), 
														ref( scattering_information ), ref( scattering_mutex ),
														cref( rays ), current_loop == 0,
														ref( shared_current_ray_index ), 
														ref( current_ray_index_mutex ), ref( rays_for_next_iteration ), 
														ref( rays_for_next_iteration_mutex ),
														ref( detector_ ), ref( detector_mutex ) );

			// For debugging
			if( thread_index == 0 ) first_thread_id = threads.back().get_id();
		}

		// Wait for threads to finish
		for( std::thread& currentThread : threads ) currentThread.join();

		// Copy rays to vector
		rays = std::move( rays_for_next_iteration );

	}



}

void Gantry::ResetGantry( void ){
	
	// Set to initial position
	coordinate_system_->SetPrimitive( initial_position_ );

	// Reset detector
	detector_.ResetDetectedRayPorperties();
}

