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

void Gantry::TransmitRaysThreaded(	const Model& radModel, const TomographyProperties& tomoParameter, 
									RayScattering& rayScatterAngles, 
									mutex& scattering_properties_mutex,
									const vector<Ray>& rays, 
									const bool repeat_transmission_after_scattering,
									size_t& sharedCurrentRayIndex, mutex& currentRayIndexMutex,
									vector<Ray>& raysForNextIteration, mutex& iterationMutex,
									XRayDetector& rayDetector, mutex& detectorMutex ){

	size_t currentRayIndex;
	Ray currentRay;
	pair<Ray, vector<Ray>> returned_rays;

	// Loop while rays are left
	while( sharedCurrentRayIndex < rays.size() ){

		// Get the Ray which should be transmitted next and increment index
		currentRayIndexMutex.lock();
		currentRayIndex = sharedCurrentRayIndex++;
		currentRayIndexMutex.unlock();

		// No more rays left
		if( currentRayIndex >= rays.size() ) break;

		// Get current ray
		currentRay =  rays.at( currentRayIndex );

		// Transmit Ray through model
		returned_rays = std::move( radModel.TransmitRay( cref( currentRay ), cref( tomoParameter ), ref( rayScatterAngles ), ref( scattering_properties_mutex ) ) );

		rayDetector.DetectRay( ref( returned_rays.first ), ref( detectorMutex ) );

		iterationMutex.lock();
		raysForNextIteration.insert( raysForNextIteration.end(), make_move_iterator( returned_rays.second.begin() ), make_move_iterator( returned_rays.second.end() ) );									// Add Ray for next iteration
		iterationMutex.unlock();

	}

	return;
}

void Gantry::RadiateModel( const Model& model, TomographyProperties tomography_properties ) {

	vector<Ray> rays = std::move( tube_.GetEmittedBeam( detector_.pixel_array(), detector_.properties().detector_focus_distance ) );		// Current rays. Start with rays from source
	
	// Convert rays to model coordinate system
	for( Ray& currentRay : rays ){
		currentRay = std::move( currentRay.ConvertTo( model.coordinate_system() ) );
	}
	
	// Convert pixel
	detector_.ConvertPixelArray( model.coordinate_system() );

	// Scattered rays should lie in the same plane as the detector 
	const UnitVector3D scatteringRotationNormal = this->coordinate_system_->GetEz().ConvertTo( model.coordinate_system() );

	RayScattering scattering_information{ number_of_scatter_angles, tube_.GetEmittedEnergyRange(), number_of_energies_for_scattering, coordinate_system_->GetEz(), atan( this->detector_.properties().row_width / this->detector_.properties().detector_focus_distance )};

	detector_.ResetDetectedRayPorperties();								// Reset all pixel

	size_t sharedCurrentRayIndex = 0;		// Index of next Ray to iterate
	mutex rayIndexMutex;				// Mutex for Ray index
	mutex raysForNextIterationMutex;	// Mutex for vector of rays for next iteration
	mutex detectorMutex;				// Mutex for detector
	mutex scatterMutex;

	// Loop until maximum loop depth is reached or no more rays are left to transmit
	for( size_t currentLoop = 0; currentLoop <= tomography_properties.max_scattering_occurrences && rays.size() > 0; currentLoop++ ){

		// No scattering in last iteration
		tomography_properties.scattering_enabled = currentLoop < tomography_properties.max_scattering_occurrences && tomography_properties.scattering_enabled;	
		
		// Adjust scattering propability because only some scattered rays would reach detector
		tomography_properties.scatter_propability_correction *= 1.;// atan( this->detector_.properties().row_width / this->detector_.properties().detector_focus_distance ) / PI;

		tomography_properties.mean_energy_of_tube = this->tube_.GetMeanEnergy();

		vector<Ray> raysForNextIteration;								// Rays to process in the next iteration
		sharedCurrentRayIndex = 0;										// Reset current ray index

		// Start threads
		vector<std::thread> threads;
		for( size_t threadIdx = 0; threadIdx < std::thread::hardware_concurrency(); threadIdx++ ){
			
			threads.emplace_back( TransmitRaysThreaded,	cref( model ), cref( tomography_properties ), 
														ref( scattering_information ), 
														ref( scatterMutex ),
														cref( rays ),
														currentLoop == 0,
														ref( sharedCurrentRayIndex ), ref( rayIndexMutex ), 
														ref( raysForNextIteration ), ref( raysForNextIterationMutex ),
														ref( detector_ ), ref( detectorMutex ) );

			if( threadIdx == 0 ) first_thread_id = threads.back().get_id();
		}

		// Wait for threads to finish
		for( std::thread& currentThread : threads ) currentThread.join();

		// Copy rays to vector
		rays = std::move( raysForNextIteration );

	}



}

void Gantry::ResetGantry( void ){
	
	// Set to initial position
	coordinate_system_->SetPrimitive( initial_position_ );

	// Reset detector
	detector_.ResetDetectedRayPorperties();
}

