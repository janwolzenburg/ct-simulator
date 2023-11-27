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

optional<double> DetectorPixel::GetDetectedLineIntegral( const bool use_simple_absorption, const size_t expected_ray_hits ) const{

	double start_intensity = 0.;
	double sum_of_end_intensity = 0.;
	double sum_of_simple_end_intensity = 0.;
	//double sum_of_simple_start_intensity = 0.;


	// Iterate all detected Ray properties
	for( const RayProperties& currentRay : detected_ray_properties_ ){

		sum_of_end_intensity += currentRay.energy_spectrum_.GetTotalPower();
		start_intensity = currentRay.start_intensity();
		//sum_of_start_intensity += currentRay.start_intensity();

		sum_of_simple_end_intensity += currentRay.simple_intensity();
		//sum_of_simple_start_intensity += 1.;

	}

	// Check no rays were detected return empty
	if( detected_ray_properties_.size() == 0){
		return {};
	}

	// Calculate line inegral
	const double line_integral_spectrum = ForceToMin( log( start_intensity * static_cast<double>( expected_ray_hits ) / sum_of_end_intensity ), 0. );
	const double line_integral_simple= ForceToMin( log( 1.* static_cast<double>( expected_ray_hits ) / sum_of_simple_end_intensity ), 0. );

	return !use_simple_absorption ? line_integral_spectrum : line_integral_simple;
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