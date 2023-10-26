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


gantry::gantry( CoordinateSystem* const coordinate_system, const tubeParameter tubeParameter_, 
				const detectorRadonParameter radonParameter, const detectorIndipendentParameter indipendentParameter ) :
	cSys( coordinate_system ),
	resetPostition( cSys->Primitive() ),
	rayDetector{ cSys->AddCoordinateSystem( PrimitiveVector3{ 0, 0, 0 }, PrimitiveVector3{ 1, 0, 0 }, PrimitiveVector3{ 0, -1, 0 }, PrimitiveVector3{ 0, 0, 1 }, "xRay detector" ),
					radonParameter, indipendentParameter },
	raySource{ cSys->AddCoordinateSystem( PrimitiveVector3{ 0, 0, 0}, PrimitiveVector3{1, 0, 0}, PrimitiveVector3{0, -1, 0}, PrimitiveVector3{0, 0, 1}, "xRay tube"), tubeParameter_ },
	raysPerPixel( ForcePositive( indipendentParameter.raysPerPixel )),
	radius( rayDetector.getPhysicalParameters().detectorFocusDistance / 2 ),
	rayScatterAngles{ 127, raySource.getEnergyRange(), 64, cSys->UnitZ() }

{
	// Align detector - tube axis with x axis
	PrimitiveCoordinateSystem xAxisAligned{ PrimitiveVector3{ 0, 0, 0 }, PrimitiveVector3{ 0, 1, 0 }, PrimitiveVector3{ 1, 0, 0 }, PrimitiveVector3{ 0, 0, 1 } };
	cSys->Primitive( xAxisAligned );

	raySource.CSys()->Translate( Vector3D{ Tuple3D{ 0, rayDetector.getPhysicalParameters().detectorFocusDistance / 2, 0 }, cSys } );
	
}


vector<ray> gantry::getBeam( const double exposureTime ) const{
	return raySource.getBeam( rayDetector.getPixel(), rayDetector.getPhysicalParameters().detectorFocusDistance, raysPerPixel, exposureTime );
}


void gantry::rotateCounterClockwise( const double angle ){
	this->cSys->Rotate( cSys->ZAxis(), angle );
}

void gantry::transmitRays(	const model& radModel, const tomographyParameter& tomoParameter, const rayScattering& rayScatterAngles,
								const vector<ray>& rays, size_t& sharedCurrentRayIndex, mutex& currentRayIndexMutex,
								vector<ray>& raysForNextIteration, mutex& iterationMutex,
								detector& rayDetector, mutex& detectorMutex ){

	size_t currentRayIndex;
	ray currentRay;
	ray returnedRay;

	// Loop while rays are left
	while( sharedCurrentRayIndex < rays.size() ){

		// Get the ray which should be transmitted next and increment index
		currentRayIndexMutex.lock();
		currentRayIndex = sharedCurrentRayIndex++;
		currentRayIndexMutex.unlock();



		// No more rays left
		if( currentRayIndex >= rays.size() ) break;

		// Write current ray to local variable
		currentRay = rays.at( currentRayIndex  );

		// Transmit ray through model
		returnedRay = radModel.rayTransmission( currentRay, tomoParameter, rayScatterAngles );
		returnedRay.Properties().EnergySpectrum().Scale( 1. / (double) returnedRay.VoxelHits() );

		// Is the ray outside the model
		if( !radModel.pntInside( returnedRay.O() ) ){
			rayDetector.detectRay( returnedRay, detectorMutex );
		}
		else{
			iterationMutex.lock();
			raysForNextIteration.push_back( returnedRay );									// Add ray for next iteration
			iterationMutex.unlock();
		}

	}

	return;
}


void gantry::radiate( const model& radModel, tomographyParameter parameter ) {

	vector<ray> rays = this->getBeam( parameter.exposureTime );		// Current rays. Start with rays from source
	
	// Convert rays to model coordinate system
	for( ray& currentRay : rays ){
		currentRay = currentRay.convertTo( radModel.CSys() );
	}
	
	// Convert pixel
	rayDetector.convertPixel( radModel.CSys() );

	// Scattered rays should lie in the same plane as the detector 
	const UnitVector3D scatteringRotationNormal = this->cSys->UnitZ().ConvertTo( radModel.CSys() );

	rayDetector.reset();								// Reset all pixel


	size_t sharedCurrentRayIndex = 0;		// Index of next ray to iterate
	mutex rayIndexMutex;				// Mutex for ray index
	mutex raysForNextIterationMutex;	// Mutex for vector of rays for next iteration
	mutex detectorMutex;				// Mutex for detector

	// Loop until maximum loop depth is reached or no more rays are left to transmit
	for( size_t currentLoop = 0; currentLoop < parameter.maxRadiationLoops && rays.size() > 0; currentLoop++ ){

		//cout << "Loop: " << currentLoop + 1 << endl;

		parameter.scattering = currentLoop < parameter.maxRadiationLoops && parameter.scattering;	// No scattering in last iteration
		
		vector<ray> raysForNextIteration;								// Rays to process in the next iteration


		// Start threads
		vector<std::thread> threads;



		for( size_t threadIdx = 0; threadIdx < std::thread::hardware_concurrency(); threadIdx++ ){
			threads.emplace_back( transmitRays,	cref( radModel ), cref( parameter ), cref( rayScatterAngles ),
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
	cSys->Primitive( resetPostition );

	// Reset detector
	rayDetector.reset();
}

