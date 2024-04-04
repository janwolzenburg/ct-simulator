#pragma once
/*********************************************************************
 * @file   propabilityDistribution.h
 * @brief  classes for randomness and probability distributions
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
 * @brief class for a generic random number generator with a uniform distribution between 0 and 2^32 - 1
*/
class RandomNumberGenerator{

	public:
	
	/*!
	 * @brief constructor
	*/
	RandomNumberGenerator( const unsigned long long int extra_seed = 0 );

	/*!
	 * @brief get a random number
	 * @return a random integer
	*/
	unsigned int GetRandomNumber( void );

	/*!
	 * @brief get a random short
	 * @return a random short integer
	*/
	unsigned short int GetRandomShortNumber(void);

	/*!
	 * @brief check if an event with given propability "happened"
	 * @param event_propability event propabilitx 
	 * @return true when event "happened"
	*/
	bool DidARandomEventHappen( const double event_propability );


	private:

	unsigned long long int generator_state_[2];		/*!< current generator state*/

};


/*!
 * @brief class to store a custom propability distribution
*/
class PropabilityDistribution{

	public:

	/*!
	 * @brief constructor
	 * @param distribution Pseudo distribution. sum of y values must not be equal to one
	*/
	PropabilityDistribution( vector<Tuple2D> distribution );

	/*!
	 * @brief get a random value according to distribution
	*/
	double GetRandomNumber( RandomNumberGenerator& generator ) const; 


	private:
	
	static constexpr unsigned int number_of_elements = USHRT_MAX + 1;	/*!< number of bins*/

	vector<double> values_;												/*!< values of distribution*/
	vector<unsigned int> discrete_distribution_;	/*!< vector filled with indices for the values_ vector according to their propability*/

};