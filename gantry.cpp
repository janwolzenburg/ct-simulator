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
#include <thread>
#include <mutex>

/*********************************************************************
  Implementations
*********************************************************************/


gantry::gantry( cartCSys* const cSys_, const size_t raysPerPixel_, const tubeParameter tubeParameter_, 
				const detectorRadonParameter radonParameter, const detectorIndipendentParameter indipendentParameter ) :
	cSys( cSys_ ),
	resetPostition( cSys->getPrimitive() ),
	rayDetector{ cSys->addCSys( primitiveVec3{ 0, 0, 0 }, primitiveVec3{ 1, 0, 0 }, primitiveVec3{ 0, -1, 0 }, primitiveVec3{ 0, 0, 1 }, "xRay detector" ),
					radonParameter, indipendentParameter },
	raySource{ cSys->addCSys( primitiveVec3{ 0, 0, 0}, primitiveVec3{1, 0, 0}, primitiveVec3{0, -1, 0}, primitiveVec3{0, 0, 1}, "xRay tube"), tubeParameter_ },
	raysPerPixel( Fpos( raysPerPixel_ )),
	radius( rayDetector.getPhysicalParameters().detectorFocusDistance / 2 )

{
	// Align detector - tube axis with x axis
	primitiveCartCSys xAxisAligned{ primitiveVec3{ 0, 0, 0 }, primitiveVec3{ 0, 1, 0 }, primitiveVec3{ 1, 0, 0 }, primitiveVec3{ 0, 0, 1 } };
	cSys->setPrimitive( xAxisAligned );

	raySource.CSys()->translateM( vec3{ v3{ 0, rayDetector.getPhysicalParameters().detectorFocusDistance / 2, 0 }, cSys } );
	
}


vector<ray> gantry::getBeam( void ) const{
	return raySource.getBeam( rayDetector.getPixel(), rayDetector.getPhysicalParameters().detectorFocusDistance, raysPerPixel);
}


vector<pixel> gantry::getPixel( void ) const{
	return rayDetector.getPixel();
}


double gantry::Radius( void ) const{
	return radius;
}


pnt3 gantry::Center( void ) const{
	return cSys->OPnt();
}


void gantry::rotateCounterClockwise( const double angle ){
	this->cSys->rotateM( cSys->zAxis(), angle );
}

void threadFunction( const ray currentRay, const model& radModel, const bool enableScattering, vector<ray>& raysToDetect, vector<ray>& raysForNextIteration ){

	std::mutex mu;

	vector<ray> returnedRays;												// Rays that have been scattered or left the model
	returnedRays = radModel.rayTransmission( currentRay, enableScattering );// Transmit ray through model

	// When current ray does not intersect model add it for detection

	if( returnedRays.empty() ){
		mu.lock();
		raysToDetect.push_back( currentRay );
		mu.unlock();
	}

	// Iterate all rays scattered or transmitted through model
	for( const ray returnedRay : returnedRays ){

		// Is the ray outside the model
		if( !radModel.pntInside( returnedRay.O() ) ){
			mu.lock();
			raysToDetect.push_back( returnedRay );	// Add ray for detection
			mu.unlock();
		}
		else{
			mu.lock();
			raysForNextIteration.push_back( returnedRay );									// Add ray for next iteration
			mu.unlock();
		}

	}

	return;
}


void gantry::radiate( const model& radModel ) {

	vector<ray> rays = this->getBeam();		// Current rays. Start with rays from source
	
	// Convert rays to model coordinate system
	for( ray& currentRay : rays ){
		currentRay = currentRay.convertTo( radModel.CSys());
	}
	
	// Convert pixel
	rayDetector.convertPixel( radModel.CSys() );

	// Thread var
	vector<ray> raysToDetect;				// Rays to detect



	// Loop until maximum loop depth is reached or no more rays are left to transmit
	for( size_t currentLoop = 0; currentLoop < maxRadiationLoops && rays.size() > 0; currentLoop++ ){

		cout << "Loop: " << currentLoop + 1 << endl;


		const bool enableScattering = currentLoop < maxRadiationLoops;	// No scattering in last iteration

		vector<ray> raysForNextIteration;								// Rays to process in the next iteration

		size_t rayNum = 0;

		// Start threads
		
		// Number of threads
		constexpr size_t numThreads = 1;

		// Start new threads while there are rays to transmit
		while( rays.size() > 0 ){

			vector<std::thread> threads;

			// Assign rays to threads
			for( size_t threadIdx = 0; threadIdx < numThreads && rays.size() > 0; threadIdx++ ){
				
				// Add thread to vector radiating last ray in vector
				threads.emplace_back( threadFunction, rays.back(), std::cref( radModel ), enableScattering, std::ref( raysToDetect ), std::ref( raysForNextIteration ) );

				// Remove ray from vector
				rays.pop_back();
			}



			// Wait for threads to finish
			for( std::thread& currentThread : threads ) currentThread.join();

			cout << '\r' << rays.size() << " rays left " << "           ";
		}

		cout << endl;

		// Copy rays to vector
		rays = raysForNextIteration;

	}

	rayDetector.reset();								// Reset all pixel

	// Iterate all rays
	for( const ray currentRay : raysToDetect ){
		// Detect ray
		rayDetector.detectRay( currentRay );
	}

}


void gantry::reset( void ){
	
	// Set to initial position
	cSys->setPrimitive( resetPostition );

	// Reset detector
	rayDetector.reset();
}


detectorRadonParameter gantry::getDetectorParameter(void) const{
	return rayDetector.getSignalParameter( );
}


const cartCSys* gantry::CSys( void ) const{
	return cSys;
}