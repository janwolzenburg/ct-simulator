#pragma once
/*********************************************************************
 * @file   vectorAlgorithm.h
 * @brief  Algorithms for vectors
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
 * @brief Create vector with equally spaced elements
 * @param start First element
 * @param end Last element
 * @param number_of_points Total amount of points
 * @return Vector with elements
*/
vector<double> CreateLinearSpace(const double start, const double end, const size_t number_of_points);

/*!
 * @brief Sum of all elements
 * @param vec Vector to sum
 * @return Sum
*/
double Sum( const vector<double> vec );

/*!
 * @brief Sum all y-values of vector elements
 * @param vec Vector of 2D points
 * @return Sum of y-values
*/
double SumYValues( const vector<Tuple2D> vec );

/*!
 * @brief Scale each element in vector
 * @param vec Vector with elements to scale
 * @param factor Factor to scale by
*/
void Scale( vector<double>& vec, const double factor );

/*!
 * @brief Scale all y-values
 * @param vec Vector with 2D elements
 * @param factor Factor to scael
*/
void ScaleYValues( vector<Tuple2D>& vec, const double factor );

/*!
 * @brief Normalise 2D Vector. Sum of y-values will be one afterwards
 * @param vec Vector to Normalise
*/
void Normalise( vector<Tuple2D>& vec );

/*!
 * @brief Normalise 2D vector
 * @param vec Vector to Normalise
 * @return Normalised vector
*/
vector<Tuple2D> Normalise( const vector<Tuple2D>& vec );

/*!
 * @brief Search element in vector closest to value
 * @param vec Vector to search. Must be sorted ascending
 * @param value Value to search for
 * @return Index of closest element in vector
*/
size_t GetClosestElementIndex( const vector<double>& vec, const double value );

/*!
 * @brief Get maximum element of vector
 * @param v Vector to search
 * @return Maximum value
*/
double GetMaxElement( const vector<double>& vec );

/*!
 * @brief Get minimum element of vector
 * @param v Vector to search
 * @return Minimum value
*/
double GetMinElement( const vector<double>& vec );

/*!
 * @brief Get maximum value in 2D vector
 * @param v Vector
 * @return Maximum value
*/
double GetMaxElement( const vector<vector<double>>& vec );

/*! 
 * @brief Get minimum value in 2D vector
 * @param v Vector
 * @return Minimum value
*/
double GetMinElement( const vector<vector<double>>& vec );

/*! 
 * @brief Add two tuples. When x-value match it adds the y-values. Otherwise a new x-value is created
 * @param vector_1 Vector 1  
 * @param vector_2 Vector 1
 * @return Sum of vectors
*/
vector<Tuple2D> AddTuple2D( const vector<Tuple2D>& vector_1, const vector<Tuple2D>& vector_2 );