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

static randomNumberGenerator integerRNG;


class randomNumberGenerator{

	public:
	randomNumberGenerator( const unsigned int minValue, const unsigned int maxValue );

	unsigned int getRandom( void );


	private:
	std::default_random_engine generator;
	std::uniform_int_distribution<unsigned int> distribution;

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