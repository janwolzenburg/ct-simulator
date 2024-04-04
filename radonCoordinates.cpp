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

RadonCoordinates::RadonCoordinates( const CoordinateSystem* const radon_coordinate_system, 
																		const Line line )
{
	// project ray on xy-plane
	const Line projected_line = line.ProjectOnXYPlane( radon_coordinate_system );

	// get perpendicualar to projected ray through coordinate system's origin
	vector3D lot = projected_line.GetLot( radon_coordinate_system->GetOriginPoint() );

	// ray intersects origin
	if( IsNearlyEqualDistance( lot.length(), 0. ) ){
		// lot vector only for angle calculation
		lot = projected_line.direction() ^ radon_coordinate_system->GetEz();
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
	else if( lot.Y() > 0. ) theta = radon_coordinate_system->GetEx().GetAngle( lot );
	// y component is less than zero
	else theta = PI - radon_coordinate_system->GetEx().GetAngle( lot );

}