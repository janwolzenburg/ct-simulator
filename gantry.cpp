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



vector<vector<pixel>> gantry::getPixel( void ) const{

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