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

#include <thread>
using std::ref;
using std::cref;
#include <FL/Fl.H>
#include "filteredProjections.h"



/*********************************************************************
   Implementations
*********************************************************************/

FilteredProjections::FilteredProjections( const Projections projections, const BackprojectionFilter::TYPE filterType, Fl_Progress_Window* progress ) :
	DataGrid{ projections.data().size(), projections.data().start(), projections.data().resolution(), 0. },		// Data grids have equal size
	filter_{ NaturalNumberRange{ -( signed long long ) size().r + 1, ( signed long long ) size().r - 1 }, resolution().r, filterType }
{

	// Define variables for easy access
	
	const size_t nT = size().c;		// Number of angles
	const size_t nD = size().r;		// Number of distances

	const double dD = resolution().r;	// Distance resolution


	// Local copy of projection data_
	const DataGrid<> projectionsData = projections.data();

	// Iterate all thetas
	for( size_t t = 0; t < nT; t++ ){

		if( progress != nullptr )
			progress->ChangeLineText( 0, "Filtering angle " + ToString( t + 1 ) + " of " + ToString( nT ) );

		// Iterate all distances
		for( size_t n = 0; n < nD; n++ ){
			if( filter_.type() == BackprojectionFilter::TYPE::constant ){
				this->SetData( GridIndex{ t, n }, projectionsData.GetData( GridIndex{ t, n } ) );
				continue;
			}

			// Convolute filter with with projections
			double convolutionResult = 0;

			for( signed long long k = filter_.points_range().start(); k <= filter_.points_range().end(); k++ ){

				// Index of current row in projection data
				signed long long projection_row_index = static_cast<signed long long>( n ) - k;
				
				// Index out of bounds of input data -> add nothing is like padding input data with zero/
				if( projection_row_index < 0 || projection_row_index >= static_cast<signed long long>( nD ) ){
					continue;
				}

				// projection data
				const double P_T = projectionsData.GetData( GridIndex{ t, static_cast<size_t>( projection_row_index ) } );

				// filter value
				const double h_n = filter_( k );

				// Multiply
				convolutionResult += h_n * P_T;
			}
			convolutionResult *= dD;
			this->SetData( GridIndex{ t, n }, convolutionResult );

		}
		Fl::check();
	}

}


double FilteredProjections::GetValue( const size_t angleIdx, const double distance ) const{


	const double dD = resolution().r;	// Distance resolution
	const size_t nD = size().r;			// Number of distances

	double exactDistanceIdx = distance / dD + ( static_cast<double>( nD ) - 1. ) / 2.;		// Exact "index" of distance

	// Index must be in bounds
	exactDistanceIdx = ForceRange( exactDistanceIdx, 0., static_cast<double>( nD ) - 1.);

	// If the exact index is a whole number_of_pixel
	if( IsNearlyEqualDistance( round( exactDistanceIdx ), exactDistanceIdx ) ){
		// Return value at distance index
		return this->operator()( GridIndex{ angleIdx, static_cast<size_t>( exactDistanceIdx ) } );
	}

	// Interpolate
	const size_t distanceIdxFloor = static_cast<size_t>( floor( exactDistanceIdx ) );		// Lower index
	const size_t distanceIdxCeil = static_cast<size_t>( ceil( exactDistanceIdx ) );			// Upper index

	const double valueAtFloor = this->operator()( GridIndex{ angleIdx, distanceIdxFloor } );	// Value at floor index
	const double valueAtCeil = this->operator()( GridIndex{ angleIdx, distanceIdxCeil } );	// Value at ceil index

	// Return the interpolated value
	return valueAtFloor + ( valueAtCeil - valueAtFloor ) / static_cast<double>( distanceIdxCeil - distanceIdxFloor ) * ( exactDistanceIdx - static_cast<double>( distanceIdxFloor ) );

}
