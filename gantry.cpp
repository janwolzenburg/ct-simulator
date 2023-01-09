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




/*********************************************************************
  Implementations
*********************************************************************/


gantry::gantry( cartCSys* const cSys_, const double radius_, const double beamAngle_, const size_t numRaysInBeam_, const tubeParameter tubeParameters_, const detectorParameter detectorParameters_ ) :

	cSys( cSys_ ),
	radius( Fpos( radius_ ) ),
	raySource{ cSys->addCSys( primitiveVec3{ 0, radius, 0 }, primitiveVec3{ 1, 0, 0 }, primitiveVec3{ 0, -1, 0 }, primitiveVec3{ 0, 0, 1 }, "xRay tube" ), tubeParameters_ },
	numRaysInBeam( Fpos( numRaysInBeam_ ) ),
	beamAngle( Fpos( beamAngle_ ) ),
	rayDetector{ cSys->addCSys( primitiveVec3{ 0, radius, 0 }, primitiveVec3{ 1, 0, 0 }, primitiveVec3{ 0, -1, 0 }, primitiveVec3{ 0, 0, 1 }, "xRay detector" ), radius, detectorParameters_ }
{

	//cSys->addCSys( primitiveVec3{ 0, radius, 0 }, primitiveVec3{ 1, 0, 0 }, primitiveVec3{ 0, -1, 0 }, primitiveVec3{ 0, 0, 1 }, "xRay tube" );

}


vector<ray> gantry::getBeam( void ) const{

	return raySource.getBeam( beamAngle, numRaysInBeam ); 

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

void gantry::radiate( const model& radModel ) {

	vector<ray> rays = this->getBeam();		// Current rays. Start with rays from source
	vector<ray> raysToDetect;				// Rays to detect

	// Loop until maximum loop depth is reached or no more rays are left to transmit
	for( size_t currentLoop = 0; currentLoop < maxRadiationLoops && rays.size() > 0; currentLoop++ ){

		vector<ray> raysForNextIteration;								// Rays to process in the next iteration

		// Iterate over all rays in current ray collection
		for( const ray currentRay : rays ){
		
			vector<ray> returnedRays;												// Rays that have been scattered or left the model
			bool enableScattering = currentLoop < maxRadiationLoops;				// No scattering in last iteration
			returnedRays = radModel.rayTransmission( currentRay, enableScattering );// Transmit ray through model
			
			// When current ray does not intersect model add it for detection
			if( returnedRays.empty() ) raysToDetect.push_back( currentRay );

			// Iterate all rays scattered or transmitted through model
			for( const ray returnedRay : returnedRays ){
			
				// Is the ray outside the model
				if( !radModel.pntInside( returnedRay.O() ) ) raysToDetect.push_back( returnedRay );	// Add ray for detection
				else raysForNextIteration.push_back( returnedRay );									// Add ray for next iteration

			}
		}

		// Copy rays to vector
		rays = raysForNextIteration;

	}

	vector<pixel> detectorPixel = this->getPixel();		// All pixel in detector
	rayDetector.reset();								// Reset all pixel

	// Iterate all rays
	for( const ray currentRay : raysToDetect ){

		// Detect ray
		rayDetector.detectRay( currentRay );

	}

}