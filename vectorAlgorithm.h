#pragma once
/*********************************************************************
 * @file   vectorAlgorithm.h
 * @brief  algorithms for vectors
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include <vector>
using std::vector;

#include "generel.h"


 /*********************************************************************
	Functions
 *********************************************************************/

/*!
 * @brief create vector with equally spaced elements
 * @param start first element
 * @param end last element
 * @param number_of_points total amount of points
 * @return vector with elements
*/
vector<double> CreateLinearSpace( const double start, const double end, const size_t number_of_points );

/*!
 * @brief sum of all elements
 * @param vector_to_sum vector to sum
 * @return sum
*/
double Sum( const vector<double>& vector_to_sum );

/*!
 * @brief sum all y-values of vector elements
 * @param vector_to_sum vector of 2D points
 * @return sum of y-values
*/
double SumYValues( const vector<Tuple2D>& vector_to_sum );

/*!
 * @brief scale each element in vector
 * @param vector_to_scale vector with elements to scale
 * @param factor factor to scale by
*/
void Scale( vector<double>& vector_to_scale, const double factor );

/*!
 * @brief scale all y-values
 * @param vector_to_scale vector with 2D elements
 * @param factor factor to scael
*/
void ScaleYValues( vector<Tuple2D>& vector_to_scale, const double factor );

/*!
 * @brief normalise 2D vector. sum of y-values will be one afterwards
 * @param vector_to_normalise vector to normalise
*/
void Normalise( vector<Tuple2D>& vector_to_normalise );

/*!
 * @brief normalise 2D vector
 * @param vector_to_normalise vector to normalise
 * @return normalised vector
*/
vector<Tuple2D> Normalise( const vector<Tuple2D>& vector_to_normalise );

/*!
 * @brief search element in vector closest to value
 * @param vector_to_search vector to search. must be sorted ascending
 * @param value value to search for
 * @return index of closest element in vector
*/
size_t GetClosestElementIndex( const vector<double>& vector_to_search, const double value );

/*!
 * @brief get maximum element of vector
 * @param vector_to_search vector to search
 * @return maximum value
*/
double GetMaxElement( const vector<double>& vector_to_search );

/*!
 * @brief get minimum element of vector
 * @param vector_to_search vector to search
 * @return minimum value
*/
double GetMinElement( const vector<double>& vector_to_search );

/*!
 * @brief get maximum value in 2D vector
 * @param vector_to_search vector to search
 * @return maximum value
*/
double GetMaxElement( const vector<vector<double>>& vector_to_search );

/*! 
 * @brief get minimum value in 2D vector
 * @param vector_to_search vector to search
 * @return minimum value
*/
double GetMinElement( const vector<vector<double>>& vector_to_search );

/*! 
 * @brief add two tuples. when x-value match it adds the y-values. otherwise a new x-value is created
 * @param tuple_vector_1 vector 1  
 * @param tuple_vector_2 vector 2
 * @return sum of vectors
*/
vector<Tuple2D> AddTuple2D( const vector<Tuple2D>& tuple_vector_1, const vector<Tuple2D>& tuple_vector_2 );