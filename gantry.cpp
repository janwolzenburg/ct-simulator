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


gantry::gantry( cartCSys* const cSys_, const size_t numRaysInBeam_, const tubeParameter tubeParameters_, const detectorRadonParameter detectorParameters_,
				size_t numRows_, const double angle_, const double columnSize, const bool structered ) :

	cSys( cSys_ ),
	resetPostition( cSys->getPrimitive() ),
	//radius( Fpos( radius_ ) ),
	raySource{ cSys->addCSys( primitiveVec3{ 0, detectorParameters_.getRadius( angle_ ) , 0}, primitiveVec3{1, 0, 0}, primitiveVec3{0, -1, 0}, primitiveVec3{0, 0, 1}, "xRay tube"), tubeParameters_},
	rayDetector{	cSys->addCSys( primitiveVec3{ 0,  detectorParameters_.getRadius( angle_ ), 0 }, primitiveVec3{ 1, 0, 0 }, primitiveVec3{ 0, -1, 0 }, primitiveVec3{ 0, 0, 1 }, "xRay detector" ),
					detectorParameters_, numRows_, angle_, columnSize, structered },
	numRaysInBeam( Fpos( numRaysInBeam_ ) ),
	beamAngle( 1.1 * angle_ ),
	radius( rayDetector.getPhysicalParameters().radius )
	{
	// Align detector - tube axis with x axis

	primitiveCartCSys xAxisALigned{ primitiveVec3{ 0, 0, 0 }, primitiveVec3{ 0, 1, 0 }, primitiveVec3{ 1, 0, 0 }, primitiveVec3{ 0, 0, 1 } };
	cSys->setPrimitive( xAxisALigned );

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