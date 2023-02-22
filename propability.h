#pragma once
/*********************************************************************
 * @file   propability.h
 * @brief  Classes for randomness and probability distributions
 *
 * @author Jan Wolzenburg
 * @date   Febuary 2023
 *********************************************************************/


 /*********************************************************************
   Includes
*********************************************************************/

#include <random>
#include <vector>
using std::vector;

#include "propability.fwd.h"
#include "generel.h"


/*********************************************************************
   Definitions
*********************************************************************/

constexpr size_t MAX_GENERATORS = 20;

class randomNumberGenerator{

	public:
	randomNumberGenerator( const int minValue, const int maxValue );

	int getRandom( void );


	private:
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution;

};


class propabilityDistribution{


	public:

	propabilityDistribution( const vector<v2> distribution_, const size_t maxNumberOfBins );

	double getRandom( void ) const;

	vector<v2> getDistribution( void ) const;

	private:

	vector<v2> distribution;
	vector<double> uniformPropabilities;

};