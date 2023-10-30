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
#include "backprojection.h"



/*********************************************************************
   Implementations
*********************************************************************/

filteredProjections::filteredProjections( const RadonTransformation projections, const BackprojectionFilter::TYPE filterType, Fl_Progress_Window* progress ) :
	DataGrid{ projections.data().size(), projections.data().start(), projections.data().resolution(), 0. },		// Data grids have equal size_
	filter{ NaturalNumberRange{ -( signed long long ) Size().r + 1, ( signed long long ) Size().r - 1 }, Resolution().r, filterType }
{

	// Define variables for easy access
	
	const size_t nT = Size().c;		// Number of angles
	const size_t nD = Size().r;		// Number of distances

	const double dD = Resolution().r;	// Distance resolution


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
				const double h_n = filter( ( signed long long ) n - ( signed long long ) l );

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


double filteredProjections::getValue( const size_t angleIdx, const double distance ) const{


	double dD = Resolution().r;	// Distance resolution
	size_t nD = Size().r;			// Number of distances

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

void reconstrucedImage::reconstructColumn(	size_t& currentX, mutex& currentXMutex, reconstrucedImage& image, 
											mutex& imageMutex, Fl_Progress_Window* progress, mutex& progressMutex, 
											const filteredProjections& projections ){

	const size_t nD = image.size().r;					// Number of distances
	const double dD = image.resolution().r;			// Distance resolution

	const size_t nT = projections.Size().c;			// Number of angles
	const double dT = projections.Resolution().c;		// Distance resolution

	// Iterate all points on image. Points are spaced by the distance resolution in filtered projections
	while( currentX < nD ){

		size_t xIdx;

		currentXMutex.lock();
		xIdx = currentX++;
		currentXMutex.unlock();

		if( xIdx >= nD ) break;


		if( progress != nullptr ){
			progressMutex.lock();
			progress->changeLineText( 1, "Reconstructing column " + ToString( xIdx + 1 ) + " of " + ToString( nD ) );
			progressMutex.unlock();
		}
			
		for( size_t yIdx = 0; yIdx < nD; yIdx++ ){

			const double x = (double) ( ( signed long long ) xIdx - ( ( signed long long ) nD - 1 ) / 2 ) * dD;		// x value on image
			const double y = (double) ( ( signed long long ) yIdx - ( ( signed long long ) nD - 1 ) / 2 ) * dD;		// y value on image

			double currentValue = image.GetData( GridIndex{ xIdx, yIdx } );
			 
			// Iterate and sum filtered projections over all angles
			for( size_t angleIdx = 0; angleIdx < nT; angleIdx++ ){

				const double arc_angle = (double) angleIdx * dT;			// Current angle value
				const double t = x * cos( arc_angle ) + y * sin( arc_angle );	// Current "distance" or magnitude in polar Coordinates

				// Get the projection value and add to current value
				const double projectionValue = projections.getValue( angleIdx, t );
				currentValue += projectionValue;
			}
			
			imageMutex.lock();
			image.SetData( GridIndex{ xIdx, yIdx }, currentValue * PI / (double) nT );
			imageMutex.unlock();
		}

		progressMutex.lock();
		Fl::check();
		progressMutex.unlock();
	}

}


reconstrucedImage::reconstrucedImage( const filteredProjections projections, Fl_Progress_Window* progress ) :
	DataGrid( GridIndex{ projections.Size().r, projections.Size().r },
		  GridCoordinates{ projections.Start().r, projections.Start().r }, 
		  GridCoordinates{ projections.Resolution().r, projections.Resolution().r }, 0. )
{

	size_t currentX = 0; 
	mutex currentXMutex, imageMutex, progressMutex;

	// Computation in threads
	vector<std::thread> threads;

	for( size_t threadIdx = 0; threadIdx < std::thread::hardware_concurrency(); threadIdx++ ){
		threads.emplace_back( reconstructColumn, ref( currentX ), ref( currentXMutex ), ref( *this ), ref( imageMutex ), progress, ref( progressMutex ), projections );
	}

	for( std::thread& currentThread : threads ) currentThread.join();

}

