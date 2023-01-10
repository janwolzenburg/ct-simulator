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

#include "spectrum.h"



/*********************************************************************
   Implementations
*********************************************************************/

spectrum::spectrum(void) {

}

spectrum::spectrum(const vector<double> X, const vector<double> Y)
{
	if (X.size() != Y.size()) {
		data = vector<v2>{ v2{0, 0} };
		return;
	}

	for (size_t i = 0; i < X.size(); i++) {
		data.push_back(v2{ X.at(i), Y.at(i) });
	}

	// Sort data by x value
	sort( data.begin(), data.end(), []( const v2& d1, const v2& d2) { return d1.x < d2.x; } );

};


void spectrum::scale( const double factor ){

	for_each( data.begin(), data.end(), [ & ] ( v2& v ) { v.y *= factor; } );

}


spectrum spectrum::getScaled(const double factor) const {

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

	double sum = 0.;

	// Iterate all data
	for( auto dataIt = data.cbegin(); dataIt < data.cend() - 1; dataIt++ ){
		sum += dataIt->y;
	}
	return sum;
}