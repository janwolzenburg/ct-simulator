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

line pixel::NormalLine( void ) const{
	return line{ this->Normal(), this->o };		// Line origin is surface origin for pixel and not the middle center!
}

pixel pixel::convertTo( const cartCSys* const target_CSys ) const{
	return pixel{ this->surfLim::convertTo( target_CSys ), this->detectedRayProperties };
}


/*
	rayPix_Intersection implementation
*/

rayPix_Intersection::rayPix_Intersection( const ray r, const pixel px ) :
	linSurfIntersection<ray, pixel>( r, px )
{
	result = linSurfIntersection<ray, pixel>::result;
	result.rayProps = r.Properties();
}