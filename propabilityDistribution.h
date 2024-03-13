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
	*/
	RandomNumberGenerator( void );

	/*!
	 * @brief Get a random number
	 * @return Random integer
	*/
	unsigned long long int GetRandomNumber( void );

	/*!
	 * @brief Check if an event with given propability "happened"
	 * @param event_propability Event propabilitx 
	 * @return True when event "happened"
	*/
	bool DidARandomEventHappen( const double event_propability );


	private:

	unsigned long long int generator_state_[2];
	//std::subtract_with_carry_engine<unsigned int, 24, 10, 12>   generator_;							/*!< Generator*/
	//std::uniform_int_distribution<unsigned long long int> distribution_;		/*!< Uniform distribution*/
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
	 * @param distribution Pseudo distribution. Sum of y values must not be equal to one
	*/
	PropabilityDistribution( const vector<Tuple2D> distribution );//, const size_t max_number_of_bins );

	/*!
	 * @brief Get a random value according to distribution
	*/
	double GetRandomNumber( mutex& scattering_properties_mutex );


	private:
	
	vector<double> values_;
	std::ranlux24_base   generator_;							/*!< Generator*/
	std::discrete_distribution<unsigned int> distribution_;

};