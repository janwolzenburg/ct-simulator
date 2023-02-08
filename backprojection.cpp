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

double filteredProjections::getValue( const size_t angleIdx, const double distance ) const{


	double dD = Resolution().row;	// Distance resolution
	size_t nD = Size().row;			// Number of distances

	double exactDistanceIdx = distance / dD + ( (double) nD - 1. ) / 2.;		// Exact "index" of distance

	// Index must be in bounds
	exactDistanceIdx = Fmin( exactDistanceIdx, 0. );
	exactDistanceIdx = Fmax( exactDistanceIdx, (double) nD - 1.);

	// If the exact index is a whole number
	if( iseqErr( round( exactDistanceIdx ), exactDistanceIdx ) ){
		// Return value at distance index
		return this->operator()( idx2CR{ angleIdx, (size_t) exactDistanceIdx });
	}

	// Interpolate
	size_t distanceIdxFloor = (size_t) floor( exactDistanceIdx );		// Lower index
	size_t distanceIdxCeil = (size_t) ceil( exactDistanceIdx );			// Upper index

	double valueAtFloor = this->operator()( idx2CR{ angleIdx, distanceIdxFloor } );	// Value at floor index
	double valueAtCeil = this->operator()( idx2CR{ angleIdx, distanceIdxCeil } );	// Value at ceil index

	// Return the interpolated value
	return valueAtFloor + ( valueAtCeil - valueAtFloor ) / ( (double) distanceIdxCeil - distanceIdxFloor ) * ( exactDistanceIdx - (double) distanceIdxFloor );

}


reconstrucedImage::reconstrucedImage( const filteredProjections projections ) :
	grid{ idx2CR{ projections.Size().row, projections.Size().row },
		  v2CR{ projections.Start().row, projections.Start().row }, 
		  v2CR{ projections.Resolution().row, projections.Resolution().row }, 0. }
{

	size_t nD = Size().row;			// Number of distances
	double dD = Resolution().row;	// Distance resolution

	size_t nT = projections.Size().col;			// Number of angles
	double dT = projections.Resolution().col;	// Distance resolution

	// Iterate all points on image. Points are spaced by the distance resolution in filtered projections
	for( size_t xIdx = 0; xIdx < nD; xIdx++ ){
		for( size_t yIdx = 0; yIdx < nD; yIdx++ ){

			double x = (double) ( ( signed long long ) xIdx - ( ( signed long long ) nD - 1 ) / 2 ) * dD;		// x value on image
			double y = (double) ( ( signed long long ) yIdx - ( ( signed long long ) nD - 1 ) / 2 ) * dD;		// y value on image

			// Iterate and sum filtered projections over all angles
			for( size_t angleIdx = 0; angleIdx < nT; angleIdx++ ){

				double angle = (double) angleIdx * dT;			// Current angle value
				double t = x * cos( angle ) + y * sin( angle );	// Current "distance" or magnitude in polar coordinates

				// Get the projection value
				double projectionValue = projections.getValue( angleIdx, t );

				this->operator()( idx2CR{ xIdx, yIdx } ) += projectionValue;
			}

			this->operator()( idx2CR{ xIdx, yIdx } ) *= PI / (double) nT;
		}
	}

}

