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


/*********************************************************************
  Implementations
*********************************************************************/


gantry::gantry( CoordinateSystem* const coordinate_system, const XRayTubeProperties tubeParameter_, 
				const radonProperties radonParameter, const PhysicalDetectorProperties indipendentParameter ) :
	cSys( coordinate_system ),
	resetPostition( cSys->GetPrimitive() ),
	rayDetector{ cSys->AddCoordinateSystem( PrimitiveVector3{ 0, 0, 0 }, PrimitiveVector3{ 1, 0, 0 }, PrimitiveVector3{ 0, -1, 0 }, PrimitiveVector3{ 0, 0, 1 }, "xRay detector" ),
					radonParameter, indipendentParameter },
	raySource{ cSys->AddCoordinateSystem( PrimitiveVector3{ 0, 0, 0}, PrimitiveVector3{1, 0, 0}, PrimitiveVector3{0, -1, 0}, PrimitiveVector3{0, 0, 1}, "xRay tube"), tubeParameter_ },
	radius( rayDetector.properties().detector_focus_distance / 2 ),
	rayScatterAngles{ 127, raySource.GetEmittedEnergyRange(), 64, cSys->GetEz() }

{
	// Align detector - tube axis with x axis
	PrimitiveCoordinateSystem xAxisAligned{ PrimitiveVector3{ 0, 0, 0 }, PrimitiveVector3{ 0, 1, 0 }, PrimitiveVector3{ 1, 0, 0 }, PrimitiveVector3{ 0, 0, 1 } };
	cSys->SetPrimitive( xAxisAligned );

	raySource.coordinate_system()->Translate( Vector3D{ Tuple3D{ 0, rayDetector.properties().detector_focus_distance / 2, 0 }, cSys } );
	
}


vector<Ray> gantry::getBeam( const double exposureTime ) const{
	return raySource.GetEmittedBeam( rayDetector.getPixel(), rayDetector.properties().detector_focus_distance, exposureTime );
}


void gantry::rotateCounterClockwise( const double arc_angle ){
	this->cSys->Rotate( cSys->GetZAxis(), arc_angle );
}

void gantry::transmitRays(	const Model& radModel, const tomographyParameter& tomoParameter, const rayScattering& rayScatterAngles,
								const vector<Ray>& rays, size_t& sharedCurrentRayIndex, mutex& currentRayIndexMutex,
								vector<Ray>& raysForNextIteration, mutex& iterationMutex,
								detector& rayDetector, mutex& detectorMutex ){

	size_t currentRayIndex;
	Ray currentRay;
	Ray returnedRay;

	// Loop while rays are left
	while( sharedCurrentRayIndex < rays.size() ){

		// Get the Ray which should be transmitted next and increment index
		currentRayIndexMutex.lock();
		currentRayIndex = sharedCurrentRayIndex++;
		currentRayIndexMutex.unlock();



		// No more rays left
		if( currentRayIndex >= rays.size() ) break;

		// Write current Ray to local variable
		currentRay = rays.at( currentRayIndex  );

		// Transmit Ray through model
		returnedRay = radModel.TransmitRay( currentRay, tomoParameter, rayScatterAngles );
		returnedRay.properties().energy_spectrum().Scale( 1. / (double) returnedRay.voxel_hits() );

		// Is the Ray outside the model
		if( !radModel.IsPointInside( returnedRay.origin() ) ){
			rayDetector.detectRay( returnedRay, detectorMutex );
		}
		else{
			iterationMutex.lock();
			raysForNextIteration.push_back( returnedRay );									// Add Ray for next iteration
			iterationMutex.unlock();
		}

	}

	return;
}


void gantry::radiate( const Model& radModel, tomographyParameter voxel_data_ ) {

	vector<Ray> rays = this->getBeam( voxel_data_.exposureTime );		// Current rays. Start with rays from source
	
	// Convert rays to model coordinate system
	for( Ray& currentRay : rays ){
		currentRay = currentRay.ConvertTo( radModel.coordinate_system() );
	}
	
	// Convert pixel
	rayDetector.convertPixel( radModel.coordinate_system() );

	// Scattered rays should lie in the same plane as the detector 
	const UnitVector3D scatteringRotationNormal = this->cSys->GetEz().ConvertTo( radModel.coordinate_system() );

	rayDetector.reset();								// Reset all pixel


	size_t sharedCurrentRayIndex = 0;		// Index of next Ray to iterate
	mutex rayIndexMutex;				// Mutex for Ray index
	mutex raysForNextIterationMutex;	// Mutex for vector of rays for next iteration
	mutex detectorMutex;				// Mutex for detector

	// Loop until maximum loop depth is reached or no more rays are left to transmit
	for( size_t currentLoop = 0; currentLoop < voxel_data_.maxRadiationLoops && rays.size() > 0; currentLoop++ ){

		//cout << "Loop: " << currentLoop + 1 << endl;

		voxel_data_.scattering = currentLoop < voxel_data_.maxRadiationLoops && voxel_data_.scattering;	// No scattering in last iteration
		
		vector<Ray> raysForNextIteration;								// Rays to process in the next iteration


		// Start threads
		vector<std::thread> threads;



		for( size_t threadIdx = 0; threadIdx < std::thread::hardware_concurrency(); threadIdx++ ){
			threads.emplace_back( transmitRays,	cref( radModel ), cref( voxel_data_ ), cref( rayScatterAngles ),
													cref( rays ), ref( sharedCurrentRayIndex ), ref( rayIndexMutex ), 
													ref( raysForNextIteration ), ref( raysForNextIterationMutex ),
													ref( rayDetector ), ref( detectorMutex ) );
		}

		// Wait for threads to finish
		for( std::thread& currentThread : threads ) currentThread.join();

		// Copy rays to vector
		rays = raysForNextIteration;

	}
}


void gantry::reset( void ){
	
	// Set to initial position
	cSys->SetPrimitive( resetPostition );

	// Reset detector
	rayDetector.reset();
}

