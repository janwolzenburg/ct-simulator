/*********************************************************************
 * @file   filteredProjections.cpp
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


const string FilteredProjections::FILE_PREAMBLE{ "FILTEREDPROJECTIONS_FILE_PREAMBLE" };

FilteredProjections::FilteredProjections( 
											const Projections& projections, 
											const BackprojectionFilter::TYPE filter_type, 
											Fl_Progress_Window* progress_window ) :

	DataGrid{ projections.data().size(), projections.data().start(), 
						projections.data().resolution(), 0. },
	filter_{ NaturalNumberRange{ -static_cast<signed long long>( size().r + 1 ), 
																static_cast<signed long long>( size().r - 1 ) }, 
																resolution().r, filter_type }
{
	const size_t number_of_angles = size().c;			// number of angles
	const size_t number_of_distances = size().r;	// number of distances

	const double distance_resolution = resolution().r;	// distance resolution
	

	// local copy of projection data
	const DataGrid<> projections_data = projections.data();

	// iterate all angles
	for( size_t angle_index = 0; angle_index < number_of_angles; angle_index++ ){

		if( progress_window != nullptr )
			progress_window->ChangeLineText( 0, "Filtering angle " 
																					+ ConvertToString( angle_index + 1 )
																				  + " of " + ConvertToString( number_of_angles ) );

		// iterate all distances
		for( size_t distance_index = 0; 
								distance_index < number_of_distances; 
								distance_index++ ){

			if( filter_.type() == BackprojectionFilter::TYPE::constant ){
				this->SetData( 
					GridIndex{ angle_index, distance_index }, 
					projections_data.GetData( GridIndex{ angle_index, distance_index } ) );
				continue;
			}

			// convolute filter with with projections
			double convolution_sum = 0;

			for( signed long long kernel_index = filter_.points_range().start(); 
														kernel_index <= filter_.points_range().end(); 
														kernel_index++ ){

				// index of current row in projection data
				signed long long projection_row_index = 
					static_cast<signed long long>( distance_index ) - kernel_index;
				
				// index out of bounds of input data -> 
				// add nothing is like padding input data with zero
				if( projection_row_index < 0 || 
						projection_row_index >= static_cast<signed long long>( number_of_distances ) ){
					continue;
				}

				// projection data
				const double projection_value = projections_data.GetData( 
					GridIndex{ angle_index, static_cast<size_t>( projection_row_index ) } );

				// filter value
				const double kernel_value = filter_( kernel_index );

				// multiply
				convolution_sum += kernel_value * projection_value;
			}
			convolution_sum *= distance_resolution;
			this->SetData( GridIndex{ angle_index, distance_index }, convolution_sum );

		}
		Fl::check();
	}
}


double FilteredProjections::GetValue( const size_t angle_index,
																			const double distance ) const{

	const double distance_resolution = resolution().r;	// distance resolution
	const size_t number_of_distances = size().r;				// number of distances

	// exact 'index' of distance. ideally we want to get the value at this index
	const double exact_index = ForceRange( 
								distance / distance_resolution + 
								static_cast<double>( number_of_distances - 1 ) / 2.,
								0., static_cast<double>( number_of_distances ) - 1. );		

	// if the exact index is a whole number skip interpolation
	if( IsNearlyEqualDistance( round( exact_index ), exact_index ) ){
		// return value at distance index
		return (*this)( GridIndex{ angle_index, static_cast<size_t>( exact_index ) } );
	}

	// interpolate
	const size_t index_floor = static_cast<size_t>( floor( exact_index ) );	// lower index
	const size_t index_ceil  = static_cast<size_t>( ceil( exact_index ) );	// upper index

	const double value_floor = this->operator()( 
		GridIndex{ angle_index, index_floor } );	// value at floor index
	const double value_ceil  = this->operator()( 
		GridIndex{ angle_index, index_ceil } );		// value at ceil index

	// return the interpolated value
	return value_floor + ( value_ceil - value_floor ) * 
											 ( exact_index - static_cast<double>( index_floor ) );

}