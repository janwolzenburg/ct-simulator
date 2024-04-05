/*********************************************************************
 * @file   vectorAlgorithm.cpp
 * @brief  implementations of algorithms for vectors
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


vector<double> CreateLinearSpace( const double start, const double end, const size_t number_of_Points ) {

	if (start > end || number_of_Points < 2) return vector<double>{ start };

	vector<double> linear_space( number_of_Points, 0. );
	const double delta = ( end - start ) / static_cast<double>( number_of_Points - 1 );

	for (size_t i = 0; i < number_of_Points; i++) {
		linear_space.at( i ) = ( start + static_cast<double>( i ) * delta );
	}

	return linear_space;
}

double Sum( const vector<double>& vector_to_sum ) {
	return std::accumulate( vector_to_sum.begin(), vector_to_sum.end(), 0. );
}

double SumYValues( const vector<Tuple2D>& vector_to_sum ){
	return std::accumulate( vector_to_sum.begin(), vector_to_sum.end(), 0., [] ( const double& current_sum, const Tuple2D& element ){ return current_sum + element.y; } );
}

void Scale( vector<double>& vector_to_scale, const double factor ) {
	std::for_each( vector_to_scale.begin(), vector_to_scale.end(), [&]( double& value ) { value *= factor; } );
}


void ScaleYValues( vector<Tuple2D>& vector_to_scale, const double factor ){
	std::for_each( vector_to_scale.begin(), vector_to_scale.end(), [&] ( Tuple2D& tuple ){ tuple.y *= factor; } );
}


void Normalise( vector<Tuple2D>& vector_to_normalise ){
	const double sum = SumYValues( vector_to_normalise );
	ScaleYValues( vector_to_normalise, 1. / sum );
}

vector<Tuple2D> Normalise( const vector<Tuple2D>& vector_to_normalise ){

	vector<Tuple2D> normalisedvector{ vector_to_normalise };

	Normalise( ref( normalisedvector ) );

	return normalisedvector;

}

size_t GetClosestElementIndex( const vector<double>& vector_to_search, const double val ){

	//!!! vec must be sorted!

	// iterator to element which is greater or equal to value
	vector<double>::const_iterator greater_or_equal_element = std::lower_bound( vector_to_search.cbegin(), vector_to_search.cend(), val );

	// first element in vec is greater or equal => return index of first element
	if( greater_or_equal_element == vector_to_search.cbegin() ) return 0;

	// lower_bound return last if no element is found. Closest is then the last element
	if( greater_or_equal_element == vector_to_search.cend() - 1 ) return vector_to_search.size() - 1;

	// compare differences to value for two consecutive elements
	if( std::abs( val - *greater_or_equal_element ) > std::abs( val - *( greater_or_equal_element - 1 ) ) ) return ( greater_or_equal_element - vector_to_search.cbegin() ) - 1;

	return greater_or_equal_element - vector_to_search.cbegin();
}

double GetMaxElement( const vector<double>& vector_to_search ){

	if( vector_to_search.size() == 0 ) return 0;

	return *std::max_element( vector_to_search.begin(), vector_to_search.end() );
}

double GetMinElement( const vector<double>& vector_to_search ){
	
	if( vector_to_search.size() == 0 ) return 0;

	return *std::min_element( vector_to_search.begin(), vector_to_search.end() );
}



double GetMinElement( const vector<vector<double>>& vector_to_search ){

	vector<double> minima_in_subvectors( vector_to_search.size(), 0. );

	for( vector<vector<double>>::const_iterator current_subvector = vector_to_search.cbegin(); current_subvector < vector_to_search.cend(); current_subvector++ ){
		
		const size_t index = current_subvector - vector_to_search.cbegin();

		minima_in_subvectors.at( index ) = GetMinElement( *current_subvector );

	}

	return GetMinElement( minima_in_subvectors );
}



double GetMaxElement( const vector<vector<double>>& vector_to_search ){

	vector<double> maxima_in_subvectors( vector_to_search.size(), 0. );

	for( vector<vector<double>>::const_iterator current_subvector = vector_to_search.cbegin(); current_subvector < vector_to_search.cend(); current_subvector++ ){

		const size_t index = current_subvector - vector_to_search.cbegin();

		maxima_in_subvectors.at( index ) = GetMaxElement( *current_subvector );

	}

	return GetMaxElement( maxima_in_subvectors );

}



vector<Tuple2D> AddTuple2D( const vector<Tuple2D>& vector_1, const vector<Tuple2D>& vector_2 ){

	vector<Tuple2D> vector_sum = vector_1;
	std::sort( vector_sum.begin(), vector_sum.end(), []( auto element_1, auto element_2 ){ return element_1.x < element_2.x; } );

	for( const auto& [x, y] : vector_2 ){

		bool vector_sum_changed = false;

		for( auto current_element = vector_sum.begin(); current_element < vector_sum.end(); current_element++ ){
			// exact match
			if( IsNearlyEqual( x, current_element->x, 0.001, ComparisonMode::Relative ) ){
				current_element->y += y;		// add y-value
				vector_sum_changed = true;
				break;
			}

			if( x < current_element->x ){
				vector_sum.insert( current_element, { x, y } );
				vector_sum_changed = true;
				break;
			}
		}

		// current x value is greater than each element
		if( !vector_sum_changed ){
			vector_sum.insert( vector_sum.end(), { x, y } );
		}


	}

	return vector_sum;

}