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
#include <mutex>

#include "propability.fwd.h"
#include "generel.h"


/*********************************************************************
   Definitions
*********************************************************************/

/*!
 * @brief Class for a generic random number generator with a uniform distribution between 0 and 2^32 - 1
*/
class randomNumberGenerator{

	public:
	
	/*!
	 * @brief Constructor
	 * @param minValue Minimum value
	 * @param maxValue Maximum value
	*/
	randomNumberGenerator( const unsigned int minValue, const unsigned int maxValue );

	/*!
	 * @brief Get a random number
	 * @return Random integer
	*/
	unsigned int getRandom( void );

	/*!
	 * @brief Check if an event with given propability "happened"
	 * @param eventPropability Event propabilitx 
	 * @return True when event "happened"
	*/
	bool eventHappend( const double eventPropability );

	private:
	std::default_random_engine generator;						/*!<Generator*/
	std::uniform_int_distribution<unsigned int> distribution;	/*!<Uniform distribution*/
	std::mutex mu;												/*!<Mutex for thread safety*/

};


extern randomNumberGenerator integerRNG;

class propabilityDistribution{


	public:

	propabilityDistribution( const vector<v2> distribution_, const size_t maxNumberOfBins );

	double getRandom( void ) const;

	vector<v2> getDistribution( void ) const;

	private:

	vector<v2> distribution;
	vector<double> uniformPropabilities;

};