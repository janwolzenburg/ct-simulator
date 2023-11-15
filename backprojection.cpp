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

void Backprojection::ReconstructImageColumn(	size_t& current_angle_index, mutex& current_angle_index_mutex, Backprojection& image, 
											mutex& imageMutex, Fl_Progress_Window* progress, mutex& progressMutex, 
											const FilteredProjections projections ){

	const size_t number_of_distances = image.size().r;				// Number of distances
	const double distance_resolution = image.resolution().r;		// Distance resolution

	const size_t number_of_angles = projections.size().c;			// Number of angles
	const double angle_resolution = projections.resolution().c;		// Angle resolution


	while( current_angle_index < number_of_angles ){

		size_t angle_index;

		current_angle_index_mutex.lock();
		angle_index = current_angle_index++;
		current_angle_index_mutex.unlock();

		if( angle_index >= number_of_angles ) break;

		if( progress != nullptr ){
			progressMutex.lock();
			progress->ChangeLineText( 1, "Backprojection projection " + ToString( angle_index + 1 ) + " of " + ToString( number_of_angles ) );
			progressMutex.unlock();
		}
		
		const double angle = static_cast<double>( angle_index ) * angle_resolution;			// Current angle value
		const double cos_angle = cos( angle );
		const double sin_angle = sin( angle );

		for( size_t xIdx = 0; xIdx < number_of_distances; xIdx++ ){
			const double x = static_cast<double>( static_cast<signed long long>( xIdx ) - ( static_cast<signed long long>( number_of_distances ) - 1 ) / 2 ) * distance_resolution;		// x value on image

			for( size_t yIdx = 0; yIdx < number_of_distances; yIdx++ ){
				
				const double y = static_cast<double>( static_cast<signed long long>( yIdx ) - ( static_cast<signed long long>( number_of_distances ) - 1 ) / 2 ) * distance_resolution;		// y value on image
				const double t = x * cos_angle + y * sin_angle;	// Current "distance" or magnitude in polar Coordinates

				const double projectionValue = projections.GetValue( angle_index, t );
				const double new_value = ( image.GetData( GridIndex{ xIdx, yIdx } ) + projectionValue * PI / static_cast<double>( number_of_angles ) );
				
				imageMutex.lock();
				image.SetData( GridIndex{ xIdx, yIdx }, new_value );
				imageMutex.unlock();
			}
		}

		progressMutex.lock();
		Fl::check();
		progressMutex.unlock();
	}

}


Backprojection::Backprojection( const FilteredProjections projections, Fl_Progress_Window* progress )
{

	const double distance_range =  ( projections.size().r - 1 ) * projections.resolution().r ;
	const double side_length = distance_range / sqrt ( 2 );
	const GridCoordinates image_start{ -side_length / 2., -side_length / 2.  };


	double image_resolution = projections.resolution().r;

	const size_t number_of_pixel = ForceEven( static_cast<size_t>( 2.* floor( ( side_length / image_resolution + 1 ) ) ) );
	image_resolution = side_length / ( number_of_pixel - 1 );
	
	const GridIndex image_size{ number_of_pixel, number_of_pixel };


	*static_cast<DataGrid*>( this ) = DataGrid{ image_size, image_start, { image_resolution, image_resolution }, 0. };


	size_t current_angle_index = 0; 
	mutex current_angle_index_mutex, imageMutex, progressMutex;

	// Computation in threads
	vector<std::thread> threads;

	for( size_t threadIdx = 0; threadIdx < 1; threadIdx++ ){
		threads.emplace_back( ReconstructImageColumn, ref( current_angle_index ), ref( current_angle_index_mutex ), ref( *this ), ref( imageMutex ), progress, ref( progressMutex ), projections );
	}

	for( std::thread& currentThread : threads ) currentThread.join();

}

