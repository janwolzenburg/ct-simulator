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

void pixel::reset( void ){
	detectedRayProperties.clear();
}


double pixel::getRadonValue( void ) const{

	double intensitySum = 0;

	// Iterate all detected ray properties
	for( rayProperties currentRay : detectedRayProperties ){
		intensitySum += currentRay.powerSpectrum.getSum();
	}
	return intensitySum;
}



/*
	rayPix_Intersection_Result implementation
*/

rayPix_Intersection_Result::rayPix_Intersection_Result( linSurf_Intersection_Result linSurfResult ) : 
	linSurf_Intersection_Result( linSurfResult )
{}



/*
	rayPix_Intersection implementation
*/

rayPix_Intersection::rayPix_Intersection( const ray r, const pixel px ) :
	linSurfIntersection<ray, pixel>( r, px )
{
	result = linSurfIntersection<ray, pixel>::Result();
	result.rayProps = r.Properties();
}