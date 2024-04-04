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
 * @param double_vector vector to sum
 * @return sum
*/
double Sum( const vector<double> double_vector );

/*!
 * @brief sum all y-values of vector elements
 * @param tuple_vector vector of 2D points
 * @return sum of y-values
*/
double SumYValues( const vector<Tuple2D> tuple_vector );

/*!
 * @brief scale each element in vector
 * @param double_vector vector with elements to scale
 * @param factor Factor to scale by
*/
void Scale( vector<double>& double_vector, const double factor );

/*!
 * @brief scale all y-values
 * @param tuple_vector vector with 2D elements
 * @param factor Factor to scael
*/
void ScaleYValues( vector<Tuple2D>& tuple_vector, const double factor );

/*!
 * @brief normalise 2D vector. sum of y-values will be one afterwards
 * @param tuple_vector vector to Normalise
*/
void Normalise( vector<Tuple2D>& tuple_vector );

/*!
 * @brief normalise 2D vector
 * @param tuple_vector vector to Normalise
 * @return normalised vector
*/
vector<Tuple2D> Normalise( const vector<Tuple2D>& tuple_vector );

/*!
 * @brief search element in vector closest to value
 * @param double_vector vector to search. Must be sorted ascending
 * @param value value to search for
 * @return index of closest element in vector
*/
size_t GetClosestElementIndex( const vector<double>& double_vector, const double value );

/*!
 * @brief get maximum element of vector
 * @param double_vector vector to search
 * @return maximum value
*/
double GetMaxElement( const vector<double>& double_vector );

/*!
 * @brief get minimum element of vector
 * @param double_vector vector to search
 * @return minimum value
*/
double GetMinElement( const vector<double>& double_vector );

/*!
 * @brief get maximum value in 2D vector
 * @param double_2d_vector vector
 * @return maximum value
*/
double GetMaxElement( const vector<vector<double>>& double_2d_vector );

/*! 
 * @brief get minimum value in 2D vector
 * @param double_2d_vector vector
 * @return minimum value
*/
double GetMinElement( const vector<vector<double>>& double_2d_vector );

/*! 
 * @brief add two tuples. when x-value match it adds the y-values. otherwise a new x-value is created
 * @param tuple_vector_1 vector 1  
 * @param tuple_vector_2 vector 2
 * @return sum of vectors
*/
vector<Tuple2D> AddTuple2D( const vector<Tuple2D>& tuple_vector_1, const vector<Tuple2D>& tuple_vector_2 );