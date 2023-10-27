/*********************************************************************
 * @file   spectrum.cpp
 * @brief  Implementations of spectrum class
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



 /*********************************************************************
   Includes
*********************************************************************/

#include <algorithm>
using std::sort;
using std::for_each;

#include <numeric>
#include "spectrum.h"
#include "generelMath.h"


/*********************************************************************
   Implementations
*********************************************************************/

spectrum::spectrum(const vector<double> X, const vector<double> Y)
{
	if (X.size() != Y.size() || X.size() < 2) {
		data_ = vector<Tuple2D>{ Tuple2D{0, 0}, Tuple2D{1, 0} };
		return;
	}

	for (size_t i = 0; i < X.size(); i++) {
		data_.push_back(Tuple2D{ X.at(i), Y.at(i) });
	}

	// Sort data_ by x value
	sort( data_.begin(), data_.end(), []( const Tuple2D& d1, const Tuple2D& d2) { return d1.x < d2.x; } );

	energyResolution = data_.at( 1 ).x - data_.at( 0 ).x;

	// Check consistency
	for( auto dataIt = data_.cbegin() + 1; dataIt < data_.cend(); dataIt++ ){

		double diff = ( dataIt )->x - ( dataIt - 1 )->x;

		if( !IsNearlyEqual( energyResolution, diff, .005, ComparisonMode::Relative ) ){
			cerr << "Spectrum must have energies equally spaced!" << endl;
		}


	}

	updateMean();
}

void spectrum::Scale( const double factor ){

	for_each( data_.begin(), data_.end(), [ & ] ( Tuple2D& v ) { v.y *= factor; } );
	updateMean();
}

spectrum spectrum::getScaled( const double factor ) const {

	spectrum scaledSpectrum{ *this };
	scaledSpectrum.Scale( factor );

	return scaledSpectrum;

}

double spectrum::getSum( void ) const{
	return std::accumulate( data_.cbegin(), data_.cend(), 0., [] ( const double& currentSum, const Tuple2D& currentValue ) { return currentSum + currentValue.y; });
}

void spectrum::updateMean( void ){

	// Get the sum of products. In principle an "expected value"
	const double expectedValue = std::accumulate( data_.cbegin(), data_.cend(), 0., [] ( const double& currentSum, const Tuple2D& currentValue ){ return currentSum + currentValue.x * currentValue.y; } );

	mean = expectedValue / getSum();	
}

void spectrum::modify( std::function<void( Tuple2D& )> modFunction ){
	for( Tuple2D& v : data_ ){
		modFunction( v );
	}

	this->updateMean();
}