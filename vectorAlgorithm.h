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
#include "rgbImage.h"



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
vector<double> linearSpace(const double start, const double end, const size_t numPoints);

/*!
 * @brief Sum of all elements
 * @param vec Vector to sum
 * @return Sum
*/
double sum( const vector<double> vec );

/*!
 * @brief Sum all y-values of vector elements
 * @param vec Vector of 2D points
 * @return Sum of y-values
*/
double sumY( const vector<v2> vec );

/*!
 * @brief Scale each element in vector
 * @param vec Vector with elements to scale
 * @param factor Factor to scale by
*/
void scale( vector<double>& vec, const double factor );

/*!
 * @brief Scale all y-values
 * @param vec Vector with 2D elements
 * @param factor Factor to scael
*/
void scaleY( vector<v2>& vec, const double factor );

/*!
 * @brief Normalize 2D Vector. Sum of y-values will be one afterwards
 * @param vec Vector to normalize
*/
void normalizeThis( vector<v2>& vec );

/*!
 * @brief Normalize 2D vector
 * @param vec Vector to normalize
 * @return Normalized vector
*/
vector<v2> normalize( const vector<v2>& vec );

/*!
 * @brief Search element in vector closest to value
 * @param vec Vector to search. Must be sorted ascending
 * @param val Value to search for
 * @return Index of closest element in vector
*/
size_t closest( const vector<double>& vec, const double val );

/*!
 * @brief Sort vector and remove adjecent equivelent elements
 * @param v Vector to process
*/
void sortUnique( vector<double>& v );

/*!
 * @brief Get maximum element of vector
 * @param v Vector to search
 * @return Maximum value
*/
double Max( const vector<double>& v );

/*!
 * @brief Get minimum element of vector
 * @param v Vector to search
 * @return Minimum value
*/
double Min( const vector<double>& v );

/*! 
 * @brief Get minimum value in 2D vector
 * @param v Vector
 * @return Minimum value
*/
double Min( const vector<vector<double>>& v );

/*!
 * @brief Get maximum value in 2D vector
 * @param v Vector
 * @return Maximum value
*/
double Max( const vector<vector<double>>& v );