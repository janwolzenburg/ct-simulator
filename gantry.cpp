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

#include "gantry.h"
#include "cSysTree.h"
#include <thread>
#include <mutex>
using std::ref;
using std::cref;


/*********************************************************************
  Implementations
*********************************************************************/


gantry::gantry( cartCSys* const cSys_, const tubeParameter tubeParameter_, 
				const detectorRadonParameter radonParameter, const detectorIndipendentParameter indipendentParameter ) :
	cSys( cSys_ ),
	resetPostition( cSys->getPrimitive() ),
	rayDetector{ cSys->addCSys( primitiveVec3{ 0, 0, 0 }, primitiveVec3{ 1, 0, 0 }, primitiveVec3{ 0, -1, 0 }, primitiveVec3{ 0, 0, 1 }, "xRay detector" ),
					radonParameter, indipendentParameter },
	raySource{ cSys->addCSys( primitiveVec3{ 0, 0, 0}, primitiveVec3{1, 0, 0}, primitiveVec3{0, -1, 0}, primitiveVec3{0, 0, 1}, "xRay tube"), tubeParameter_ },
	raysPerPixel( Fpos( indipendentParameter.raysPerPixel )),
	radius( rayDetector.getPhysicalParameters().detectorFocusDistance / 2 ),
	rayScatterAngles{ 127, raySource.getFrequencyRange(), 64, cSys->EzVec() }

{
	// Align detector - tube axis with x axis
	primitiveCartCSys xAxisAligned{ primitiveVec3{ 0, 0, 0 }, primitiveVec3{ 0, 1, 0 }, primitiveVec3{ 1, 0, 0 }, primitiveVec3{ 0, 0, 1 } };
	cSys->setPrimitive( xAxisAligned );

	raySource.CSys()->translateM( vec3{ v3{ 0, rayDetector.getPhysicalParameters().detectorFocusDistance / 2, 0 }, cSys } );
	
}

//gantry::gantry( void ) : 
//	gantry{ CSYS_TREE().addCSys( "Model system" ), tubeParameter{}, detectorRadonParameter{}, detectorIndipendentParameter{} }
//{}

vector<ray> gantry::getBeam( const double exposureTime ) const{
	return raySource.getBeam( rayDetector.getPixel(), rayDetector.getPhysicalParameters().detectorFocusDistance, raysPerPixel, exposureTime );
}


void gantry::rotateCounterClockwise( const double angle ){
	this->cSys->rotateM( cSys->zAxis(), angle );
}

void threadFunction(	const model& radModel, const bool enableScattering, const rayScattering& rayScatterAngles,
						const vector<ray>& rays, size_t& sharedCurrentRayIndex, std::mutex& currentRayIndexMutex,
						vector<ray>& raysForNextIteration, std::mutex& iterationMutex,
						detector& rayDetector, std::mutex& detectorMutex ){

	// Loop while rays are left
	while( sharedCurrentRayIndex < rays.size() ){

		// Get the ray which should be transmitted next and increment index
		currentRayIndexMutex.lock();
		size_t currentRayIndex = sharedCurrentRayIndex++;
		currentRayIndexMutex.unlock();

		// No more rays left
		if( currentRayIndex >= rays.size() ) break;

		// Write current ray to local variable
		const ray currentRay = rays.at( currentRayIndex  );

		// Transmit ray through model
		const ray returnedRay = radModel.rayTransmission( currentRay, enableScattering, rayScatterAngles );

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


void gantry::radiate( const model& radModel, const double exposureTime ) {

	vector<ray> rays = this->getBeam( exposureTime );		// Current rays. Start with rays from source
	
	// Convert rays to model coordinate system
	for( ray& currentRay : rays ){
		currentRay = currentRay.convertTo( radModel.CSys() );
	}
	
	// Convert pixel
	rayDetector.convertPixel( radModel.CSys() );

	// Scattered rays should lie in the same plane as the detector 
	const uvec3 scatteringRotationNormal = this->cSys->EzVec().convertTo( radModel.CSys() );

	rayDetector.reset();								// Reset all pixel


	size_t sharedCurrentRayIndex = 0;		// Index of next ray to iterate
	std::mutex rayIndexMutex;				// Mutex for ray index
	std::mutex raysForNextIterationMutex;	// Mutex for vector of rays for next iteration
	std::mutex detectorMutex;				// Mutex for detector

	// Loop until maximum loop depth is reached or no more rays are left to transmit
	for( size_t currentLoop = 0; currentLoop < maxRadiationLoops && rays.size() > 0; currentLoop++ ){

		//cout << "Loop: " << currentLoop + 1 << endl;

		const bool enableScattering = currentLoop < maxRadiationLoops;	// No scattering in last iteration
		vector<ray> raysForNextIteration;								// Rays to process in the next iteration


		// Start threads
		vector<std::thread> threads;

		for( size_t threadIdx = 0; threadIdx < numThreads; threadIdx++ ){
			threads.emplace_back( threadFunction,	cref( radModel ), enableScattering, cref( rayScatterAngles ),
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
	cSys->setPrimitive( resetPostition );

	// Reset detector
	rayDetector.reset();
}

