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

double DetectorPixel::GetDetectedLineIntegral( void ) const{

	double sum_of_start_intensity = 0.;
	double sum_of_end_intensity = 0.;



	// Iterate all detected Ray properties
	for( const RayProperties& currentRay : detected_ray_properties_ ){
		//sum_of_end_intensity += currentRay.energy_spectrum_.GetTotal();
		//sum_of_start_intensity += currentRay.start_intensity();
		double q_espec = currentRay.start_intensity() / currentRay.energy_spectrum_.GetTotal();

		double q_J = 1. / currentRay.J;

		sum_of_end_intensity += currentRay.J;
		sum_of_start_intensity += 1.;

	}

	// Check no rays were detected return -1
	if( detected_ray_properties_.size() == 0){
		return -1.;
	}

	// Calculate line inegral
	const double line_integral = log( sum_of_start_intensity / sum_of_end_intensity );

	return line_integral;
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