/*********************************************************************
 * @file   backprojection.cpp
 * @brief  Implementations of backprojection classes
 *
 * @author Jan Wolzenburg
 * @date   Febuary 2023
 *********************************************************************/


 /*********************************************************************
  Includes
*********************************************************************/

#include "backprojection.h"




/*********************************************************************
   Implementations
*********************************************************************/


filteredProjections::filteredProjections( const radonTransformed projections, const discreteFilter::TYPE filterType ) :
	grid{ projections.Data().Size(), projections.Data().Start(), projections.Data().Resolution(), 0. }		// Data grids have equal size
{

	// Define variables for easy access
	
	size_t nT = Size().col;		// Number of angles
	size_t nD = Size().row;		// Number of distances

	double dD = Resolution().row;	// Distance resolution

	// Create filter kernel. Range of discrete arguments must fir convolution
	discreteFilter h{ Zrange{ -( signed long long ) nD + 1, ( signed long long ) nD - 1 }, dD, filterType };

	// Local copy of projection data
	grid projectionsData = projections.Data();

	// Iterate all thetas
	for( size_t t = 0; t < nT; t++ ){

		// Iterate all distances
		for( size_t n = 0; n < nD; n++ ){

			// Convolute filter with with projections
			double convolutionResult = 0;
			for( size_t l = 0; l < nD; l++ ){

				// projection value
				double P_T = projectionsData( idx2CR{ t, l } );

				// filter value
				double h_n = h( ( signed long long ) n - ( signed long long ) l );

				// Multiply
				convolutionResult += h_n * P_T;
			}

			convolutionResult *= dD;

			this->operator()( idx2CR{ t, n } )  = convolutionResult;

		}

	}

}


reconstrucedImage::reconstrucedImage( const filteredProjections projections ) :
	grid{ projections.Size(), projections.Start(), projections.Resolution(), 0. }{

	size_t nT = Size().col;			// Number of angles
	size_t nD = Size().row;			// Number of distances
	double dD = Resolution().row;	// Distance resolution
	double dT = Resolution().col;	// Distance resolution

	// Iterate all points on image. Points are spaced by the distance resolution in filtered projections
	for( size_t xIdx = 0; xIdx < nD; xIdx++ ){
		for( size_t yIdx = 0; yIdx < nD; yIdx++ ){

			double x = (double) ( ( signed long long ) xIdx - ( ( signed long long ) nD - 1 ) / 2 ) * dD;		// x value on image
			double y = (double) ( ( signed long long ) yIdx - ( ( signed long long ) nD - 1 ) / 2 ) * dD;		// y value on image

			// Iterate and sum filtered projections over all angles
			for( size_t angleIdx = 0; angleIdx < nT; angleIdx++ ){

				double angle = (double) angleIdx * dT;			// Current angle value
				double t = x * cos( angle ) + y * sin( angle );	// Current "distance" or magnitude in polar coordinates

				// Find the distance index in filtered projections corresponding to t
				//TODO: complete routine

			}


		}
	}

}

