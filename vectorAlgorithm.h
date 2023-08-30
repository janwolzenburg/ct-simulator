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


vector<double> linearSpace(const double start, const double end, const size_t numPoints);

double sum( const vector<double> vec );

double sum( const vector<v2> vec );

void scale( vector<double>& vec, const double factor );

void scale( vector<v2>& vec, const double factor );

void normalizeThis( vector<v2>& vec );

vector<v2> normalize( const vector<v2> vec );

size_t closest( const vector<double>& vec, const double val );









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

double Min( const vector<vector<double>> v );