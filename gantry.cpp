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


gantry::gantry( cartCSys* const cSys_, const size_t raysPerPixel_, const tubeParameter tubeParameter_, detectorRadonParameter& radonParameter,
				detectorIndipendentParameter& indipendentParameter ) :
	cSys( cSys_ ),
	resetPostition( cSys->getPrimitive() ),
	raySource{ cSys->addCSys( primitiveVec3{ 0, 0, 0}, primitiveVec3{1, 0, 0}, primitiveVec3{0, -1, 0}, primitiveVec3{0, 0, 1}, "xRay tube"), tubeParameter_ },
	rayDetector{	cSys->addCSys( primitiveVec3{ 0, 0, 0 }, primitiveVec3{ 1, 0, 0 }, primitiveVec3{ 0, -1, 0 }, primitiveVec3{ 0, 0, 1 }, "xRay detector" ),
					radonParameter, indipendentParameter },
	raysPerPixel( Fpos( raysPerPixel_ )),
	numberRaysInBeam( raysPerPixel * rayDetector.getPhysicalParameters().number.col ),
	beamAngle( rayDetector.getPhysicalParameters().angle + 2. * (double) ( raysPerPixel - 1 ) / ( 2. * (double) raysPerPixel ) * rayDetector.getSignalParameter().resolution.col ),
	radius( rayDetector.getPhysicalParameters().detectorFocusDistance / 2 )

{
	// Align detector - tube axis with x axis
	primitiveCartCSys xAxisALigned{ primitiveVec3{ 0, 0, 0 }, primitiveVec3{ 0, 1, 0 }, primitiveVec3{ 1, 0, 0 }, primitiveVec3{ 0, 0, 1 } };
	cSys->setPrimitive( xAxisALigned );

	raySource.CSys()->translateM( vec3{ v3{ 0, rayDetector.getPhysicalParameters().detectorFocusDistance / 2, 0 }, cSys } );
	rayDetector.CSys()->translateM( vec3{ v3{ 0, rayDetector.getPhysicalParameters().detectorFocusDistance / 2, 0 }, cSys } );
}


vector<ray> gantry::getBeam( void ) const{
	return raySource.getBeam( beamAngle, numberRaysInBeam ); 
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
	
	// Convert rays to model coordinate system
	for( ray& currentRay : rays ){
		currentRay = currentRay.convertTo( radModel.CSys());
	}
	
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