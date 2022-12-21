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

};

spectrum spectrum::getScaled(const double factor) const {

	spectrum scaledSpectrum{ *this };

	//for (size_t i = 0; i < scaledSpectrum.data.size(); i++) scaledSpectrum.data.at( i ).y *= factor;
	
	std::for_each(scaledSpectrum.data.begin(), scaledSpectrum.data.end(), [&]( v2& v ) { v.y *= factor; });

	return scaledSpectrum;

}