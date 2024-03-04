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
	const size_t number_of_angles = size().c;			// Number of angles
	const size_t number_of_distances = size().r;	// Number of distances

	const double distance_resolution = resolution().r;	// Distance resolution
	

	// Local copy of projection data
	const DataGrid<> projections_data = std::move( projections.data() );

	// Iterate all angles
	for( size_t angle_index = 0; angle_index < number_of_angles; angle_index++ ){

		if( progress_window != nullptr )
			progress_window->ChangeLineText( 0, "Filtering angle " 
																					+ ToString( angle_index + 1 )
																				  + " of " + ToString( number_of_angles ) );

		// Iterate all distances
		for( size_t distance_index = 0; 
								distance_index < number_of_distances; 
								distance_index++ ){

			if( filter_.type() == BackprojectionFilter::TYPE::constant ){
				this->SetData( 
					GridIndex{ angle_index, distance_index }, 
					projections_data.GetData( GridIndex{ angle_index, distance_index } ) );
				continue;
			}

			// Convolute filter with with projections
			double convolution_sum = 0;

			for( signed long long kernel_index = filter_.points_range().start(); 
														kernel_index <= filter_.points_range().end(); 
														kernel_index++ ){

				// Index of current row in projection data
				signed long long projection_row_index = 
					static_cast<signed long long>( distance_index ) - kernel_index;
				
				// Index out of bounds of input data -> add nothing is like padding input data with zero
				if( projection_row_index < 0 || 
						projection_row_index >= static_cast<signed long long>( number_of_distances ) ){
					continue;
				}

				// projection data
				const double projection_value = projections_data.GetData( 
					GridIndex{ angle_index, static_cast<size_t>( projection_row_index ) } );

				// filter value
				const double kernel_value = filter_( kernel_index );

				// Multiply
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

	const double distance_resolution = resolution().r;	// Distance resolution
	const size_t number_of_distances = size().r;				// Number of distances

	// Exact "index" of distance. Ideally we want to get the value at this index
	const double exact_index = ForceRange( 
								distance / distance_resolution 
								+ static_cast<double>( number_of_distances - 1 ) / 2.,
								0., static_cast<double>( number_of_distances ) - 1. );		

	// If the exact index is a whole number skip interpolation
	if( IsNearlyEqualDistance( round( exact_index ), exact_index ) ){
		// Return value at distance index
		return this->operator()( GridIndex{ angle_index, static_cast<size_t>( exact_index ) } );
	}

	// Interpolate
	const size_t index_floor = static_cast<size_t>( floor( exact_index ) );		// Lower index
	const size_t index_ceil  = static_cast<size_t>( ceil( exact_index ) );			// Upper index

	const double value_floor = this->operator()( 
		GridIndex{ angle_index, index_floor } );	// Value at floor index
	const double value_ceil  = this->operator()( 
		GridIndex{ angle_index, index_ceil } );	// Value at ceil index

	// Return the interpolated value
	return value_floor + ( value_ceil - value_floor ) 
										   / static_cast<double>( index_ceil - index_floor ) 
											 * ( exact_index - static_cast<double>( index_floor ) );

}
