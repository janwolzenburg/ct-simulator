/*********************************************************************
 * @file   vectorAlgorithm.cpp
 * @brief  Implementations of algorithms for vectors
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



  /*********************************************************************
	Includes
 *********************************************************************/

#include <vector>
using std::vector;

#include <algorithm>
#include <numeric>

#include "generelMath.h" 



 /*********************************************************************
	Functions
 *********************************************************************/


vector<double> linearSpace(const double start, const double end, const size_t numPoints) {

	if (start > end || numPoints < 2) return vector<double>{ start };

	vector<double> vec;
	const double delta = ( end - start ) / ( (double) numPoints - 1 );

	for (size_t i = 0; i < numPoints; i++) {
		vec.push_back( start + (double) i * delta );
	}

	return vec;
}

double sum( const vector<double> vec ) {

	return std::accumulate( vec.begin(), vec.end(), 0. );

}


void scale(vector<double>& vec, const double factor) {

	std::for_each( vec.begin(), vec.end(), [&]( double& d ) { d *= factor; } );

}


size_t closest( const vector<double> vec, const double val ){

	//std::sort( vec.begin(), vec.end() );

	// Iterator to element which is greater or equal to value
	vector<double>::const_iterator it_geq = std::lower_bound( vec.begin(), vec.end(), val );

	// First element in vec is greater or equal => return iterator to first element
	if( it_geq == vec.begin() ) return 0;

	// Compare differences to value for two consecutive elements
	if( std::abs( val - *it_geq ) > std::abs( val - *( it_geq - 1 ) ) ) return ( it_geq - vec.begin() ) - 1;

	return it_geq - vec.begin();
}


void sortUnique( vector<double>& v ){
	// Sort ascending
	std::sort( v.begin(), v.end() );

	// Remove duplicates
	vector<double>::const_iterator last = std::unique( v.begin(), v.end(), [] ( const double& d1, const double& d2 ){ return iseqErr( d1, d2 ); } );
	v.erase( last, v.end() );
}


double sortUniqueMeanDiff( vector<double> v ){
	sortUnique( v );

	// Adjecent differences
	vector<double> diff( v.size() );
	std::adjacent_difference( v.begin(), v.end(), diff.begin() );
	diff.erase( diff.begin() );

	// Sort differences ascending
	std::sort( diff.begin(), diff.end() );

	size_t quantileStart = (size_t) ( 0.88 * (double) diff.size() );
	size_t quantileEnd = (size_t) ( 0.98 * (double) diff.size() );

	// Mean of differences
	double meanDiff = std::accumulate( diff.begin() + quantileStart, diff.begin() + quantileEnd, 0. ) / (double) ( quantileEnd - quantileStart );

	return meanDiff;
}


double max( const vector<double> v ){
	return *std::max_element( v.begin(), v.end() );
}

double min( const vector<double> v ){
	return *std::min_element( v.begin(), v.end() );
}
