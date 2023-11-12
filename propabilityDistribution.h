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


extern class RandomNumberGenerator integer_random_number_generator;		/*!< One instance of integer random number generator*/

/*!
 * @brief Class for a generic random number generator with a uniform distribution between 0 and 2^32 - 1
*/
class RandomNumberGenerator{

	public:
	
	/*!
	 * @brief Constructor
	 * @param min_value Minimum value
	 * @param max_value Maximum value
	*/
	RandomNumberGenerator( const unsigned int min_value, const unsigned int max_value );

	/*!
	 * @brief Get a random number_of_pixel
	 * @return Random integer
	*/
	unsigned int GetRandomNumber( void );

	/*!
	 * @brief Check if an event with given propability "happened"
	 * @param event_propability Event propabilitx 
	 * @return True when event "happened"
	*/
	bool DidARandomEventHappen( const double event_propability );


	private:

	std::default_random_engine generator_;							/*!< Generator*/
	std::uniform_int_distribution<unsigned int> distribution_;		/*!< Uniform distribution*/
	mutex mutex_;													/*!< Mutex for thread safety*/

};


/*!
 * @brief Class to store a custom propability distribution
 * @details The distribution is described by a vector of 2D-Tuples. The x-value is the variate and the y-value is the propability corrensponding to that variate.
 * From the distribution a vector is created. The vector contains repeatitions of the variates. The amount repeatitions ist proportional to the propability of the variate.
*/
class PropabilityDistribution{

	public:

	/*!
	 * @brief Constructor
	 * @param distribution Pseudo distribution. Sum of y values may not be equal to one
	 * @param max_number_of_bins Maximum amount of bins to put the distribution in. Low number may exclude low propabilites
	*/
	PropabilityDistribution( const vector<Tuple2D> distribution, const size_t max_number_of_bins );

	/*!
	 * @brief Get a random value according to distribution
	*/
	double GetRandomNumber( void ) const;

	/*!
	 * @brief Get the complete distribution
	*/
	vector<Tuple2D> distribution( void ) const{ return distribution_; };


	private:

	vector<Tuple2D> distribution_;				/*!< Distribution*/
	vector<double> uniform_propabilities_;		/*!< Vector with values from distribution reated according to their propability*/

};