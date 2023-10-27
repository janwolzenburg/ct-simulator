/*********************************************************************
 * @file   detectorPixel.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


/*********************************************************************
	Includes
 *********************************************************************/

#include "detectorPixel.h"



/*********************************************************************
   Implementations
*********************************************************************/


/*
	pixel implementation
*/

double pixel::getRadonValue( void ) const{

	double intensitySum = 0;

	// Iterate all detected Ray properties_
	for( const RayProperties& currentRay : detectedRayProperties ){
		intensitySum += currentRay.energy_spectrum_.getSum();
	}
	return intensitySum;
}

Line pixel::NormalLine( void ) const{
	return Line{ Surface::GetNormal(), this->origin_ };		// Line origin_ is surface origin_ for pixel and not the middle center!
}

pixel pixel::ConvertTo( const CoordinateSystem* const target_CSys ) const{
	return pixel{ this->BoundedSurface::ConvertTo( target_CSys ), this->detectedRayProperties };
}


/*
	RayPixelIntersection implementation
*/

RayPixelIntersection::RayPixelIntersection( const Ray r, const pixel px ) :
	LineSurfaceIntersection<Ray, pixel>{ r, px },
	rayProps(r.properties() )
{}