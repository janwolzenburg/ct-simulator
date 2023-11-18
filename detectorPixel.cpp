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
	DetectorPixel implementation
*/

double DetectorPixel::GetDetectedLineIntegral( const bool use_simple_attenuation ) const{

	double sum_of_start_intensity = 0.;
	double sum_of_end_intensity = 0.;
	double sum_of_simple_end_intensity = 0.;
	double sum_of_simple_start_intensity = 0.;


	// Iterate all detected Ray properties
	for( const RayProperties& currentRay : detected_ray_properties_ ){
		sum_of_end_intensity += currentRay.energy_spectrum_.GetTotalPower();
		sum_of_start_intensity += currentRay.start_intensity();

		sum_of_simple_end_intensity += currentRay.J;
		sum_of_simple_start_intensity += 1.;

	}

	// Check no rays were detected return -1
	if( detected_ray_properties_.size() == 0){
		return -1.;
	}

	// Calculate line inegral
	const double line_integral_spectrum = log( sum_of_start_intensity / sum_of_end_intensity );
	const double line_integral_simple= log( sum_of_simple_start_intensity / sum_of_simple_end_intensity );

	return !use_simple_attenuation ? line_integral_spectrum : line_integral_simple;
}

DetectorPixel DetectorPixel::ConvertTo( const CoordinateSystem* const target_CSys ) const{
	return DetectorPixel{ this->BoundedSurface::ConvertTo( target_CSys ), this->detected_ray_properties_ };
}


/*
	RayPixelIntersection implementation
*/

RayPixelIntersection::RayPixelIntersection( const Ray r, const DetectorPixel px ) :
	LineSurfaceIntersection<Ray, DetectorPixel>{ r, px },
	ray_properties( r.properties() )
{}