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

#include <chrono>
using namespace std::chrono;

#include "gantry.h"
#include "coordinateSystemTree.h"
#include "tomography.h"
#include "simulation.h"


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
	detector_.UpdateProperties( radon_properties, physical_detector_properties );

	ResetGantry();

}

void Gantry::RotateCounterClockwise( const double arc_angle ){
	this->coordinate_system_->Rotate( coordinate_system_->GetZAxis(), arc_angle );
}

void Gantry::TranslateInZDirection( const double distance ){
	this->coordinate_system_->Translate( coordinate_system_->GetEz() * distance );
}

void Gantry::TransmitRaysThreaded(	const Model& radModel, const TomographyProperties tomoParameter, const RayScattering rayScatterAngles,
									const pair<vector<Ray>::const_iterator, vector<Ray>::const_iterator> first_and_end_ray,
									vector<Ray>& raysForNextIteration, mutex& iterationMutex,
									XRayDetector& rayDetector, mutex& detectorMutex ){

	for( auto current_ray = first_and_end_ray.first; current_ray < first_and_end_ray.second; current_ray++ ){

		// Transmit Ray through model
		Ray returnedRay = radModel.TransmitRay( *current_ray, tomoParameter, rayScatterAngles );
		returnedRay.properties().energy_spectrum().Scale( 1. / (double) returnedRay.voxel_hits() );

		// Is the Ray outside the model
		if( !radModel.IsPointInside( returnedRay.origin() ) ){
			rayDetector.DetectRay( returnedRay, detectorMutex );
		}
		else{
			iterationMutex.lock();
			raysForNextIteration.push_back( returnedRay );									// Add Ray for next iteration
			iterationMutex.unlock();
		}
	}

	return;
}

void Gantry::RadiateModel( const Model& model, TomographyProperties tomography_properties ) {

	vector<Ray> rays = tube_.GetEmittedBeam( detector_.pixel_array(), detector_.properties().detector_focus_distance, tomography_properties.exposure_time );		// Current rays. Start with rays from source
	
	// Convert rays to model coordinate system
	for( Ray& currentRay : rays ){
		currentRay = currentRay.ConvertTo( model.coordinate_system() );
	}
	
	// Convert pixel
	detector_.ConvertPixelArray( model.coordinate_system() );

	// Scattered rays should lie in the same plane as the detector 
	const UnitVector3D scatteringRotationNormal = this->coordinate_system_->GetEz().ConvertTo( model.coordinate_system() );

	RayScattering rayScatterAngles{ number_of_scatter_angles, tube_.GetEmittedEnergyRange(), number_of_energies_for_scattering, coordinate_system_->GetEz() };

	detector_.ResetDetectedRayPorperties();								// Reset all pixel

	size_t sharedCurrentRayIndex = 0;		// Index of next Ray to iterate
	mutex rayIndexMutex;				// Mutex for Ray index
	mutex raysForNextIterationMutex;	// Mutex for vector of rays for next iteration
	mutex detectorMutex;				// Mutex for detector

	// Loop until maximum loop depth is reached or no more rays are left to transmit
	for( size_t currentLoop = 0; currentLoop < tomography_properties.max_scattering_occurrences && rays.size() > 0; currentLoop++ ){

		tomography_properties.scattering_enabled = currentLoop < tomography_properties.max_scattering_occurrences && tomography_properties.scattering_enabled;	// No scattering in last iteration
		
		vector<Ray> raysForNextIteration;								// Rays to process in the next iteration

		// Split rays in chunks to use for threads
		const size_t number_of_threads = std::thread::hardware_concurrency();
		const size_t rays_per_thread = ForceToMin1( static_cast<size_t>( ceil( static_cast<double>( rays.size() ) / static_cast<double>( number_of_threads  ) ) ) );
		vector<Ray>::const_iterator range_start = rays.cbegin();

		// Start threads
		vector<std::thread> threads;
		for( size_t threadIdx = 0; threadIdx < number_of_threads && range_start < rays.cend(); threadIdx++ ){

			// End of ray range
			vector<Ray>::const_iterator range_end;
			
			// Check if range end is valid
			if( static_cast<size_t>( rays.cend() - range_start ) > rays_per_thread )
				range_end = range_start + rays_per_thread;
			else
				range_end = rays.cend();

			// Create pair with start and end of ray range for this thread
			pair<vector<Ray>::const_iterator, vector<Ray>::const_iterator> rays_range{
				range_start, range_end
			};

			// New start is current end
			range_start = range_end;

			// Add to threads vector
			threads.emplace_back( TransmitRaysThreaded,	cref( model ), tomography_properties , rayScatterAngles,
														rays_range, ref( raysForNextIteration ), ref( raysForNextIterationMutex ),
														ref( detector_ ), ref( detectorMutex ) );
		}

		// Wait for threads to finish
		for( std::thread& currentThread : threads ) currentThread.join();

		// Copy rays to vector
		rays = raysForNextIteration;

	}

}

void Gantry::ResetGantry( void ){
	
	// Set to initial position
	coordinate_system_->SetPrimitive( initial_position_ );

	// Reset detector
	detector_.ResetDetectedRayPorperties();
}

