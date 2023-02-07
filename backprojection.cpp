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
	data{ projections.Data().Size(), projections.Data().Start(), projections.Data().Resolution(), 0. }		// Data grids have equal size
{

	// Define variables for easy access
	
	size_t nT = data.Size().col;		// Number of angles
	size_t nD = data.Size().row;		// Number of distances

	double dD = data.Resolution().row;

	// Create filter kernel
	discreteFilter h{ Zrange{ -( signed long long ) nD + 1, ( signed long long ) nD - 1 }, dD, filterType };

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

			data( idx2CR{ t, n } ) = convolutionResult;

		}

	}

}


grid filteredProjections::Data( void ) const{
	return data;
}