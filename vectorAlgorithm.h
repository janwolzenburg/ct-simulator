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
 * @param numPoints Total amount of points
 * @return Vector with elements
*/
vector<double> CreateLinearSpace(const double start, const double end, const size_t numPoints);

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
 * @param val Value to search for
 * @return Index of closest element in vector
*/
size_t GetClosestElementIndex( const vector<double>& vec, const double val );

/*!
 * @brief Get maximum element of vector
 * @param v Vector to search
 * @return Maximum value
*/
double MaxElement( const vector<double>& v );

/*!
 * @brief Get minimum element of vector
 * @param v Vector to search
 * @return Minimum value
*/
double MinElement( const vector<double>& v );

/*!
 * @brief Get maximum value in 2D vector
 * @param v Vector
 * @return Maximum value
*/
double MaxElement( const vector<vector<double>>& v );

/*! 
 * @brief Get minimum value in 2D vector
 * @param v Vector
 * @return Minimum value
*/
double MinElement( const vector<vector<double>>& v );