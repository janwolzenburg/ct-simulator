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
	detectedRays.clear();
}



/*
	rayPix_Intersection implementation
*/

rayPix_Intersection::rayPix_Intersection( const ray r, const pixel px ) :
	linSurfIntersection<ray, pixel>( r, px )
{
	linSurf_Intersection_Result linSurfResult = linSurfIntersection<ray, pixel>::Result();
	result.rayProps = r.Properties();
}