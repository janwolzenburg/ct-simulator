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
	DataGrid{ projections.data().size(), projections.data().start(), projections.data().resolution(), 0. },		// Data grids have equal size_
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
			progress->changeLineText(0,"Filtering angle " + ToString( t + 1 ) + " of " + ToString( nT ) );

		// Iterate all distances
		for( size_t n = 0; n < nD; n++ ){

			// Convolute filter with with projections
			double convolutionResult = 0;
			for( size_t l = 0; l < nD; l++ ){

				// projection value
				const double P_T = projectionsData( GridIndex{ t, l } );

				// filter value
				const double h_n = filter_( ( signed long long ) n - ( signed long long ) l );

				// Multiply
				if( h_n != 0.)
					convolutionResult += h_n * P_T;
			}
			convolutionResult *= dD;
			this->SetData( GridIndex{ t, n }, convolutionResult );

		}
		Fl::check();
	}
}


double FilteredProjections::GetValue( const size_t angleIdx, const double distance ) const{


	double dD = resolution().r;	// Distance resolution
	size_t nD = size().r;			// Number of distances

	double exactDistanceIdx = distance / dD + ( (double) nD - 1. ) / 2.;		// Exact "index" of distance

	// Index must be in bounds
	exactDistanceIdx = ForceToMin( exactDistanceIdx, 0. );
	exactDistanceIdx = ForceToMax( exactDistanceIdx, (double) nD - 1.);

	// If the exact index is a whole number_of_pixel
	if( IsNearlyEqualDistance( round( exactDistanceIdx ), exactDistanceIdx ) ){
		// Return value at distance index
		return this->operator()( GridIndex{ angleIdx, (size_t) exactDistanceIdx });
	}

	// Interpolate
	size_t distanceIdxFloor = (size_t) floor( exactDistanceIdx );		// Lower index
	size_t distanceIdxCeil = (size_t) ceil( exactDistanceIdx );			// Upper index

	double valueAtFloor = this->operator()( GridIndex{ angleIdx, distanceIdxFloor } );	// Value at floor index
	double valueAtCeil = this->operator()( GridIndex{ angleIdx, distanceIdxCeil } );	// Value at ceil index

	// Return the interpolated value
	return valueAtFloor + ( valueAtCeil - valueAtFloor ) / ( static_cast<double>( distanceIdxCeil - distanceIdxFloor ) ) * ( exactDistanceIdx - static_cast<double>( distanceIdxFloor ) );

}
