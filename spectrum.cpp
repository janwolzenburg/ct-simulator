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



/*********************************************************************
   Implementations
*********************************************************************/

spectrum::spectrum(void) : 
	mean( 0. ),
	frequencyResolution( 1. )
{

}

spectrum::spectrum(const vector<double> X, const vector<double> Y)
{
	if (X.size() != Y.size() || X.size() < 2) {
		data = vector<v2>{ v2{0, 0}, v2{1, 0} };
		return;
	}

	for (size_t i = 0; i < X.size(); i++) {
		data.push_back(v2{ X.at(i), Y.at(i) });
	}

	// Sort data by x value
	sort( data.begin(), data.end(), []( const v2& d1, const v2& d2) { return d1.x < d2.x; } );

	frequencyResolution = data.at( 1 ).x - data.at( 0 ).x;

	// Check consistency
	for( auto dataIt = data.cbegin() + 1; dataIt < data.cend(); dataIt++ ){

		double diff = ( dataIt )->x - ( dataIt - 1 )->x;

		if( !isEqErrPercent( frequencyResolution, diff, .005 ) ){
			cerr << "Spectrum must have frequency equally spaced!" << endl;
		}


	}

	updateMean();
};


void spectrum::scale( const double factor ){

	for_each( data.begin(), data.end(), [ & ] ( v2& v ) { v.y *= factor; } );
	updateMean();
}


spectrum spectrum::getScaled( const double factor ) const {

	spectrum scaledSpectrum{ *this };
	
	scaledSpectrum.scale( factor );

	return scaledSpectrum;

}


double spectrum::getIntegral( void ) const{

	double integral = 0.;

	// Iterate all data
	for( auto dataIt = data.cbegin(); dataIt < data.cend() - 1; dataIt++ ){

		double xDelta = (dataIt + 1)->x - dataIt->x;
		double yValue = dataIt->y;

		integral += xDelta * yValue;

	}

	return integral;
}


double spectrum::getSum( void ) const{
	return std::accumulate( data.cbegin(), data.cend(), 0., [] ( const double& currentSum, const v2& currentValue ) { return currentSum + currentValue.y; });
}

double spectrum::getMean( void ) const{
	return mean;
}

void spectrum::updateMean( void ){

	// Get the sum of products. In principle an "expected value"
	const double expectedValue = std::accumulate( data.cbegin(), data.cend(), 0., [] ( const double& currentSum, const v2& currentValue ){ return currentSum + currentValue.x * currentValue.y; } );

	mean = expectedValue / getSum();	
}


double spectrum::getMin( void ) const{
	return data.front().x;
}

double spectrum::getMax( void ) const{
	return data.back().x;
}

void spectrum::modify( std::function<void( v2& )> modFunction ){
	for( v2& v : data ){
		modFunction( v );
	}

	this->updateMean();
}