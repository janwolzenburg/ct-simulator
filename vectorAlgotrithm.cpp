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


vector<double> CreateLinearSpace(const double start, const double end, const size_t numPoints) {

	if (start > end || numPoints < 2) return vector<double>{ start };

	vector<double> vec( numPoints, 0. );
	const double delta = ( end - start ) / static_cast<double>( numPoints - 1 );

	for (size_t i = 0; i < numPoints; i++) {
		vec.at( i ) = ( start + static_cast<double>( i ) * delta );
	}

	return vec;
}

double Sum( const vector<double> vec ) {
	return std::accumulate( vec.begin(), vec.end(), 0. );
}

double SumYValues( const vector<Tuple2D> vec ){
	return std::accumulate( vec.begin(), vec.end(), 0., [] ( const double& a, const Tuple2D& b ){ return a + b.y; } );
}

void Scale(vector<double>& vec, const double factor) {
	std::for_each( vec.begin(), vec.end(), [&]( double& d ) { d *= factor; } );
}


void ScaleYValues( vector<Tuple2D>& vec, const double factor ){
	std::for_each( vec.begin(), vec.end(), [&] ( Tuple2D& d ){ d.y *= factor; } );
}


void Normalise( vector<Tuple2D>& vec ){
	ScaleYValues( vec, 1. / SumYValues( vec ) );
}

vector<Tuple2D> Normalise( const vector<Tuple2D>& vec ){

	vector<Tuple2D> normalisedVector{ vec };

	Normalise( ref( normalisedVector ) );

	return normalisedVector;

}

size_t GetClosestElementIndex( const vector<double>& vec, const double val ){

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

double GetMaxElement( const vector<double>& v ){

	if( v.size() == 0 ) return 0;

	return *std::max_element( v.begin(), v.end() );
}

double GetMinElement( const vector<double>& v ){
	
	if( v.size() == 0 ) return 0;

	return *std::min_element( v.begin(), v.end() );
}



double GetMinElement( const vector<vector<double>>& v ){

	vector<double> minima( v.size(), 0. );

	for( vector<vector<double>>::const_iterator sV = v.cbegin(); sV < v.cend(); sV++ ){
		
		size_t index = sV - v.cbegin();

		minima.at( index ) = GetMinElement( *sV );

	}

	return GetMinElement( minima );
}



double GetMaxElement( const vector<vector<double>>& v ){

	vector<double> maxima( v.size(), 0. );

	for( vector<vector<double>>::const_iterator sV = v.cbegin(); sV < v.cend(); sV++ ){

		size_t index = sV - v.cbegin();

		maxima.at( index ) = GetMaxElement( *sV );

	}

	return GetMaxElement( maxima );

}
