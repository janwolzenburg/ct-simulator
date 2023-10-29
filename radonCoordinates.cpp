/*********************************************************************
 * @file   radonCoordinates.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


  /*********************************************************************
	Includes
 *********************************************************************/

#include "radonCoordinates.h"



  /*********************************************************************
	Implementations
 *********************************************************************/

 
/*!
 * radonCoords implementation
*/

radonCoords::radonCoords( const CoordinateSystem* const cSys, const Line l ) : 
	theta( 0 ), distance( 0 )
{
	// Project Ray on XY plane
	const Line projectedLine = l.ProjectOnXYPlane( cSys );

	// Get perpendicualar to projected Ray through coordinate system's origin_
	Vector3D lot = projectedLine.GetLot( cSys->GetOriginPoint() );


	// Ray intersects origin_
	if( IsNearlyEqualDistance( lot.length(), 0 ) ){
		// Lot vector only for angle calculation
		lot = projectedLine.direction() ^ cSys->GetEz();
	}
	// No intersection -> distance from perpendicular
	else{
		// y component is zero
		if( IsNearlyEqualDistance( lot.Y(), 0 ) ){
			// x component is less than zero
			if( lot.X() < 0 ) distance = -lot.length();
			else distance = lot.length();
		}
		// y component is not zero
		else{
			// y component is less than zero
			if( lot.Y() < 0 ) distance = -lot.length();
			else  distance = lot.length();
		}
	}

	// Calculate radon angle

	// y component is zero
	if( IsNearlyEqualDistance( lot.Y(), 0 ) ) theta = 0;
	// y component is greater than zero
	else if( lot.Y() > 0 ) theta = cSys->GetEx().GetAngle( lot );
	// y component is less than zero
	else theta = PI - cSys->GetEx().GetAngle( lot );

}