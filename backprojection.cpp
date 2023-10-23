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

filteredProjections::filteredProjections( const radonTransformed projections, const discreteFilter::TYPE filterType, Fl_Progress_Window* progress ) :
	grid{ projections.Data().Size(), projections.Data().Start(), projections.Data().Resolution(), 0. },		// Data grids have equal size
	filter{ Zrange{ -( signed long long ) Size().row + 1, ( signed long long ) Size().row - 1 }, Resolution().row, filterType }
{

	// Define variables for easy access
	
	const size_t nT = Size().col;		// Number of angles
	const size_t nD = Size().row;		// Number of distances

	const double dD = Resolution().row;	// Distance resolution


	// Local copy of projection data
	const grid<> projectionsData = projections.Data();

	// Iterate all thetas
	for( size_t t = 0; t < nT; t++ ){

		if( progress != nullptr )
			progress->changeLineText(0,"Filtering angle " + toString( t + 1 ) + " of " + toString( nT ) );

		// Iterate all distances
		for( size_t n = 0; n < nD; n++ ){

			// Convolute filter with with projections
			double convolutionResult = 0;
			for( size_t l = 0; l < nD; l++ ){

				// projection value
				const double P_T = projectionsData( idx2CR{ t, l } );

				// filter value
				const double h_n = filter( ( signed long long ) n - ( signed long long ) l );

				// Multiply
				if( h_n != 0.)
					convolutionResult += h_n * P_T;
			}
			convolutionResult *= dD;
			this->setData( idx2CR{ t, n }, convolutionResult );

		}
		Fl::check();
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
	return valueAtFloor + ( valueAtCeil - valueAtFloor ) / ( static_cast<double>( distanceIdxCeil - distanceIdxFloor ) ) * ( exactDistanceIdx - static_cast<double>( distanceIdxFloor ) );

}

void reconstrucedImage::reconstructColumn(	size_t& currentX, mutex& currentXMutex, reconstrucedImage& image, 
											mutex& imageMutex, Fl_Progress_Window* progress, mutex& progressMutex, 
											const filteredProjections& projections ){

	const size_t nD = image.Size().row;					// Number of distances
	const double dD = image.Resolution().row;			// Distance resolution

	const size_t nT = projections.Size().col;			// Number of angles
	const double dT = projections.Resolution().col;		// Distance resolution

	// Iterate all points on image. Points are spaced by the distance resolution in filtered projections
	while( currentX < nD ){

		size_t xIdx;

		currentXMutex.lock();
		xIdx = currentX++;
		currentXMutex.unlock();

		if( xIdx >= nD ) break;


		if( progress != nullptr ){
			progressMutex.lock();
			progress->changeLineText( 1, "Reconstructing column " + toString( xIdx + 1 ) + " of " + toString( nD ) );
			progressMutex.unlock();
		}
			
		for( size_t yIdx = 0; yIdx < nD; yIdx++ ){

			const double x = (double) ( ( signed long long ) xIdx - ( ( signed long long ) nD - 1 ) / 2 ) * dD;		// x value on image
			const double y = (double) ( ( signed long long ) yIdx - ( ( signed long long ) nD - 1 ) / 2 ) * dD;		// y value on image

			double currentValue = image.getData( idx2CR{ xIdx, yIdx } );
			 
			// Iterate and sum filtered projections over all angles
			for( size_t angleIdx = 0; angleIdx < nT; angleIdx++ ){

				const double angle = (double) angleIdx * dT;			// Current angle value
				const double t = x * cos( angle ) + y * sin( angle );	// Current "distance" or magnitude in polar coordinates

				// Get the projection value and add to current value
				const double projectionValue = projections.getValue( angleIdx, t );
				currentValue += projectionValue;
			}
			
			imageMutex.lock();
			image.setData( idx2CR{ xIdx, yIdx }, currentValue * PI / (double) nT );
			imageMutex.unlock();
		}

		progressMutex.lock();
		Fl::check();
		progressMutex.unlock();
	}

}


reconstrucedImage::reconstrucedImage( const filteredProjections projections, Fl_Progress_Window* progress ) :
	grid( idx2CR{ projections.Size().row, projections.Size().row },
		  v2CR{ projections.Start().row, projections.Start().row }, 
		  v2CR{ projections.Resolution().row, projections.Resolution().row }, 0. )
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

