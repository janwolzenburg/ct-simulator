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

void Backprojection::ReconstructImageColumn(	size_t& currentX, mutex& currentXMutex, Backprojection& image, 
											mutex& imageMutex, Fl_Progress_Window* progress, mutex& progressMutex, 
											const FilteredProjections projections ){

	const size_t nD = image.size().r;					// Number of distances
	const double dD = image.resolution().r;			// Distance resolution

	const size_t nT = projections.size().c;			// Number of angles
	const double dT = projections.resolution().c;		// Distance resolution

	// Iterate all points on image. Points are spaced by the distance resolution in filtered projections
	while( currentX < nD ){

		size_t xIdx;

		currentXMutex.lock();
		xIdx = currentX++;
		currentXMutex.unlock();

		if( xIdx >= nD ) break;


		if( progress != nullptr ){
			progressMutex.lock();
			progress->ChangeLineText( 1, "Reconstructing column " + ToString( xIdx + 1 ) + " of " + ToString( nD ) );
			progressMutex.unlock();
		}
			
		for( size_t yIdx = 0; yIdx < nD; yIdx++ ){

			const double x = static_cast<double>( static_cast<signed long long>( xIdx ) - ( static_cast<signed long long>( nD ) - 1 ) / 2 ) * dD;		// x value on image
			const double y = static_cast<double>( static_cast<signed long long>( yIdx ) - ( static_cast<signed long long>( nD ) - 1 ) / 2 ) * dD;		// y value on image

			double currentValue = image.GetData( GridIndex{ xIdx, yIdx } );
			 
			// Iterate and sum filtered projections over all angles
			for( size_t angleIdx = 0; angleIdx < nT; angleIdx++ ){

				const double arc_angle = static_cast<double>( angleIdx ) * dT;			// Current angle value
				const double t = x * cos( arc_angle ) + y * sin( arc_angle );	// Current "distance" or magnitude in polar Coordinates

				// Get the projection value and add to current value
				const double projectionValue = projections.GetValue( angleIdx, t );
				currentValue += projectionValue;
			}
			
			imageMutex.lock();
			image.SetData( GridIndex{ xIdx, yIdx }, currentValue * PI / static_cast<double>( nT ) );
			imageMutex.unlock();
		}

		progressMutex.lock();
		Fl::check();
		progressMutex.unlock();
	}

}


Backprojection::Backprojection( const FilteredProjections projections, Fl_Progress_Window* progress ) :
	DataGrid{ GridIndex{ projections.size().r, projections.size().r },
		  GridCoordinates{ projections.start().r, projections.start().r }, 
		  GridCoordinates{ projections.resolution().r, projections.resolution().r }, 0. }
{

	size_t currentX = 0; 
	mutex currentXMutex, imageMutex, progressMutex;

	// Computation in threads
	vector<std::thread> threads;

	for( size_t threadIdx = 0; threadIdx < std::thread::hardware_concurrency(); threadIdx++ ){
		threads.emplace_back( ReconstructImageColumn, ref( currentX ), ref( currentXMutex ), ref( *this ), ref( imageMutex ), progress, ref( progressMutex ), projections );
	}

	for( std::thread& currentThread : threads ) currentThread.join();

}

