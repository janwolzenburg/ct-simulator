/*********************************************************************
 * @file   detectorPixel.cpp
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

optional<double> DetectorPixel::GetProjectionValue( 
																	const bool use_simple_absorption, 
																	const size_t expected_ray_hits, 
																	const double start_intensity ) const{

	double sum_of_end_intensity = 0.;
	double sum_of_simple_end_intensity = 0.;

	// iterate all detected ray properties
	for( const RayProperties& currentRay : detected_ray_properties_ ){
		sum_of_end_intensity += currentRay.energy_spectrum_.GetTotalPower();
		sum_of_simple_end_intensity += currentRay.simple_intensity();
	}

	// check no rays were detected return empty
	if( detected_ray_properties_.size() == 0)
		return {};

	// calculate line inegral
	const double line_integral_spectrum = log( start_intensity * 
																							 static_cast<double>( expected_ray_hits ) / 
																							 sum_of_end_intensity ) ;

	const double line_integral_simple = log( 1.* 
																						 static_cast<double>( expected_ray_hits ) / 
																						 sum_of_simple_end_intensity );

	return !use_simple_absorption ? line_integral_spectrum : line_integral_simple;
}

DetectorPixel DetectorPixel::ConvertTo( const CoordinateSystem* const target_coordinate_system ) const{
	return DetectorPixel{ this->BoundedSurface::ConvertTo( target_coordinate_system ), this->detected_ray_properties_ };
}


/*
	RayPixelIntersection implementation
*/

RayPixelIntersection::RayPixelIntersection( const Ray& ray, const DetectorPixel& pixel ) :
	LineSurfaceIntersection<Ray, DetectorPixel>{ ray, pixel },
	ray_properties( ray.properties() )
{}