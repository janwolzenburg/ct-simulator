/*********************************************************************
 * @file   reconstructedImage.cpp
 * @brief  
 *
 * @author Jan Wolzenburg
 * @date   October 2023
 *********************************************************************/


 /*********************************************************************
  Includes
*********************************************************************/

#include <thread>
using std::ref;
using std::cref;
#include <FL/Fl.H>
#include "backprojection.h"



/*********************************************************************
   Implementations
*********************************************************************/


const string Backprojection::FILE_PREAMBLE{ "BACKPROJECTION_FILE_PREAMBLE" };

void Backprojection::ReconstructImageColumn(	
										 size_t& current_angle_index, mutex& current_angle_index_mutex, 
										 Backprojection& image, mutex& image_mutex, 
										 Fl_Progress_Window* progress, mutex& progress_mutex, 
										 const FilteredProjections& projections ){

	const size_t number_of_distances = image.size().r;			// number of distances
	const double distance_resolution = image.resolution().r;// distance resolution

	const size_t number_of_angles = projections.size().c;				// number of angles
	const double angle_resolution = projections.resolution().c;	// angle resolution

	while( current_angle_index < number_of_angles ){

		size_t angle_index;

		current_angle_index_mutex.lock();
		angle_index = current_angle_index++;
		current_angle_index_mutex.unlock();

		if( angle_index >= number_of_angles ) break;

		if( progress != nullptr ){
			progress_mutex.lock();
			progress->ChangeLineText( 1, "Backprojection projection " 
																	 + ToString( angle_index + 1 ) + " of " 
																	 + ToString( number_of_angles ) );
			progress_mutex.unlock();
		}

		// current angle value
		const double angle = static_cast<double>( angle_index ) 
												 * angle_resolution + angle_resolution / 2.;			
		const double cos_angle = cos( angle );
		const double sin_angle = sin( angle );

		// iterate image columns
		for( size_t column_index = 0; column_index < number_of_distances; column_index++ ){
			const double column_coordinate = image.GetColCoordinate( column_index );

			// iterate image rows
			for( size_t row_index = 0; row_index < number_of_distances; row_index++ ){
				const double row_coordinate = image.GetRowCoordinate( row_index );
				
				// pixel's "distance" or magnitude in polar Coordinates
				const double s = column_coordinate * cos_angle + row_coordinate * sin_angle;	

				// projection value for given angle and distance
				const double projection_value = projections.GetValue( angle_index, s );
				
				// value to add to pixel value
				double new_value = projection_value * PI / static_cast<double>( number_of_angles );
				
				image_mutex.lock();
				new_value += image.GetData( GridIndex{ column_index, row_index } );
				image.SetData( GridIndex{ column_index, row_index }, new_value );
				image_mutex.unlock();
			}
		}

		progress_mutex.lock();
		Fl::check(); 
		progress_mutex.unlock();
	}
}


Backprojection::Backprojection( const FilteredProjections projections, Fl_Progress_Window* progress )
{

	const double distance_range =  ( projections.size().r - 1 ) * projections.resolution().r ;
	const double side_length = distance_range;// / sqrt ( 2 );
	const GridCoordinates image_start{ -side_length / 2., -side_length / 2.  };

	const size_t number_of_pixel = static_cast<size_t>( 2.* floor( ( side_length / projections.resolution().r + 1 ) ) );
	const double image_resolution = side_length / ( number_of_pixel - 1 );
	
	const GridIndex image_size{ number_of_pixel, number_of_pixel };


	*static_cast<DataGrid*>( this ) = DataGrid{ image_size, image_start, { image_resolution, image_resolution }, 0. };


	size_t current_angle_index = 0; 
	mutex current_angle_index_mutex, imageMutex, progressMutex;

	// computation in threads
	vector<std::thread> threads;

	for( size_t threadIdx = 0; threadIdx < std::thread::hardware_concurrency(); threadIdx++ ){
		threads.emplace_back( ReconstructImageColumn, ref( current_angle_index ), ref( current_angle_index_mutex ), ref( *this ), ref( imageMutex ), progress, ref( progressMutex ), projections );
	}

	for( std::thread& currentThread : threads ) currentThread.join();

}

