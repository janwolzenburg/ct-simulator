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

double DetectorPixel::GetRadonValue( void ) const{

	double sum_of_line_integrals = 0.;

	// Iterate all detected Ray properties_
	for( const RayProperties& currentRay : detected_ray_properties_ ){
		const double current_intensity = currentRay.energy_spectrum_.GetTotal();
		

		if( IsNearlyEqual( current_intensity, 0., 0.00001, ComparisonMode::Relative ) ){
			
			sum_of_line_integrals += 100;

			continue;
		}
		sum_of_line_integrals += log( currentRay.start_intensity() / current_intensity );
	}

	// Correction factor necessary because a finite amount of rays are simulated and scattering may lead to large drop in detected intensity
	const size_t pixel_hits = detected_ray_properties().size();
	const double correction_factor = pixel_hits > 1 ? 1. / static_cast<double>( pixel_hits ) : 1.;

	// Correction factor necessary because a finite amount of rays are simulated and scattering may lead to large drop in detected intensity
	return sum_of_line_integrals * correction_factor;
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