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

#include <algorithm>
#include <numeric>

#include "vectorAlgorithm.h"
#include "generelMath.h"



 /*********************************************************************
	Functions
 *********************************************************************/


vector<double> linearSpace(const double start, const double end, const size_t numPoints) {

	if (start > end || numPoints < 2) return vector<double>{ start };

	vector<double> vec( numPoints, 0. );
	const double delta = ( end - start ) / ( (double) numPoints - 1 );

	for (size_t i = 0; i < numPoints; i++) {
		vec.at( i ) = ( start + (double) i * delta );
	}

	return vec;
}

double sum( const vector<double> vec ) {
	return std::accumulate( vec.begin(), vec.end(), 0. );
}

double sumY( const vector<v2> vec ){
	return std::accumulate( vec.begin(), vec.end(), 0., [] ( const double& a, const v2& b ){ return a + b.y; } );
}

void scale(vector<double>& vec, const double factor) {
	std::for_each( vec.begin(), vec.end(), [&]( double& d ) { d *= factor; } );
}


void scaleY( vector<v2>& vec, const double factor ){
	std::for_each( vec.begin(), vec.end(), [&] ( v2& d ){ d.y *= factor; } );
}


void normalizeThis( vector<v2>& vec ){
	scaleY( vec, 1. / sumY( vec ) );
}

vector<v2> normalize( const vector<v2>& vec ){

	vector<v2> normalizedVector{ vec };

	normalizeThis( normalizedVector );

	return normalizedVector;

}

size_t closest( const vector<double>& vec, const double val ){

	//!!! vec must be sorted!

	// Iterator to element which is greater or equal to value
	vector<double>::const_iterator it_geq = std::lower_bound( vec.cbegin(), vec.cend(), val );

	// First element in vec is greater or equal => return index of first element
	if( it_geq == vec.cbegin() ) return 0;

	// lower_bound return last if no element is found. Closest is then the last element
	if( it_geq == vec.cend() - 1 ) return vec.size() - 1;

	// Compare differences to value for two consecutive elements
	if( std::abs( val - *it_geq ) > std::abs( val - *( it_geq - 1 ) ) ) return ( it_geq - vec.cbegin() ) - 1;

	return it_geq - vec.cbegin();
}


void sortUnique( vector<double>& v ){
	// Sort ascending
	std::sort( v.begin(), v.end() );

	// Remove duplicates
	vector<double>::const_iterator last = std::unique( v.begin(), v.end(), [] ( const double& d1, const double& d2 ){ return iseqErr( d1, d2 ); } );
	v.erase( last, v.end() );
}


double Max( const vector<double>& v ){

	if( v.size() == 0 ) return 0;

	return *std::max_element( v.begin(), v.end() );
}

double Min( const vector<double>& v ){
	
	if( v.size() == 0 ) return 0;

	return *std::min_element( v.begin(), v.end() );
}



double Min( const vector<vector<double>>& v ){

	vector<double> minima( v.size(), 0. );

	for( vector<vector<double>>::const_iterator sV = v.cbegin(); sV < v.cend(); sV++ ){
		
		size_t index = sV - v.cbegin();

		minima.at( index ) = Min( *sV );

	}

	return Min( minima );
}



double Max( const vector<vector<double>>& v ){

	vector<double> maxima( v.size(), 0. );

	for( vector<vector<double>>::const_iterator sV = v.cbegin(); sV < v.cend(); sV++ ){

		size_t index = sV - v.cbegin();

		maxima.at( index ) = Max( *sV );

	}

	return Max( maxima );

}
