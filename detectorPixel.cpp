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

	// Iterate all detected ray properties
	for( const rayProperties& currentRay : detectedRayProperties ){
		intensitySum += currentRay.energySpectrum.getSum();
	}
	return intensitySum;
}

Line pixel::NormalLine( void ) const{
	return Line{ Surface::GetNormal(), this->origin_ };		// Line origin_ is surface origin_ for pixel and not the middle center!
}

pixel pixel::convertTo( const CoordinateSystem* const target_CSys ) const{
	return pixel{ this->BoundedSurface::ConvertTo( target_CSys ), this->detectedRayProperties };
}


/*
	RayPixelIntersection implementation
*/

RayPixelIntersection::RayPixelIntersection( const ray r, const pixel px ) :
	LineSurfaceIntersection<ray, pixel>{ r, px },
	rayProps(r.Properties() )
{}