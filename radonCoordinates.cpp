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
 * RadonCoordinates implementation
*/

RadonCoordinates::RadonCoordinates( const CoordinateSystem* const cSys, const Line l )

{
	// project Ray on XY plane
	const Line projectedLine = l.ProjectOnXYPlane( cSys );

	// get perpendicualar to projected Ray through coordinate system's origin_
	Vector3D lot = projectedLine.GetLot( cSys->GetOriginPoint() );


	// ray intersects origin_
	if( IsNearlyEqualDistance( lot.length(), 0. ) ){
		// lot vector only for angle calculation
		lot = projectedLine.direction() ^ cSys->GetEz();
		distance = 0.;
	}
	// no intersection -> distance from perpendicular
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

	// calculate radon angle

	// y component is zero
	if( IsNearlyEqualDistance( lot.Y(), 0 ) ) theta = 0;
	// y component is greater than zero
	else if( lot.Y() > 0. ) theta = cSys->GetEx().GetAngle( lot );
	// y component is less than zero
	else theta = PI - cSys->GetEx().GetAngle( lot );

}