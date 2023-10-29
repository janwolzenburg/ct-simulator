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
#include "generel.h"



/*********************************************************************
   Definitions
*********************************************************************/

/*!
 * @brief Class for a generic random number_of_pixel generator with a uniform distribution between 0 and 2^32 - 1
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
	 * @brief Get a random number_of_pixel
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
	mutex mu;													/*!<Mutex for thread safety*/

};

extern randomNumberGenerator integerRNG;						/*!<One instance of integer random number_of_pixel generator*/


/*!
 * @brief Class to store a custom propability distribution
*/
class propabilityDistribution{


	public:

	/*!
	 * @brief Constructor
	 * @param distribution_ Pseudo distribution. Sum of y values can be not equal to one
	 * @param maxNumberBins Maximum amount of bins to put the distribution in. Low number_of_pixel may exclude low propabilites
	*/
	propabilityDistribution( const vector<Tuple2D> distribution_, const size_t maxNumberOfBins );

	/*!
	 * @brief Get a random value according to distribution
	*/
	double getRandom( void ) const;

	/*!
	 * @brief Get the complete distribution
	*/
	vector<Tuple2D> getDistribution( void ) const{ return distribution; };


	private:

	vector<Tuple2D> distribution;				/*!<Distribution*/
	vector<double> uniformPropabilities;	/*!<Vector with values from distribution reated according to their propability*/

};