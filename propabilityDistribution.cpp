/*********************************************************************
 * @file   propability.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   Febuary 2023
 *********************************************************************/



 /*********************************************************************
   Includes
*********************************************************************/

#include <chrono>

#include "propabilityDistribution.h"
#include "vectorAlgorithm.h"



/*********************************************************************
   Implementation
*********************************************************************/



/*
	randomNumberGenerator implementation
*/

// Minus one is important!
RandomNumberGenerator integer_random_number_generator{ 0, UINT32_MAX - 1 };

RandomNumberGenerator::RandomNumberGenerator( const unsigned int minValue, const unsigned int maxValue ) :
	generator_{ (unsigned int) std::chrono::system_clock::now().time_since_epoch().count() },
	distribution_{ minValue, maxValue }
{}


unsigned int RandomNumberGenerator::GetRandomNumber( void ){

	mutex_.lock();
	const unsigned int randomNumber = distribution_( generator_ );
	mutex_.unlock();

	return randomNumber;
}

bool RandomNumberGenerator::DidARandomEventHappen( const double event_propability ){

	const unsigned int interval = distribution_.max() - distribution_.min() + 1;
	const double single_value_propability = 1. / static_cast<double>( interval );
	const unsigned int event_interval = 
		static_cast<unsigned int>( event_propability / single_value_propability );

	const unsigned int random_integer = GetRandomNumber();

	if( random_integer >= distribution_.min() && 
			random_integer < distribution_.min() + event_interval ) 
		return true;

	return false;
}



/*
	propabilityDistribution implementation
*/

PropabilityDistribution::PropabilityDistribution( const vector<Tuple2D> distribution ) :	
	values_( distribution.size(), 0. ),
	generator_{ (unsigned int) std::chrono::system_clock::now().time_since_epoch().count() }		
{

	// Vector with weights of variates
	vector<double> weights( distribution.size(), 0. );

	// Get values and weight from distribution
	for( size_t value_index = 0; value_index < distribution.size(); value_index++ ){
	
		values_.at( value_index ) = distribution.at( value_index ).x;
		weights.at( value_index ) = distribution.at( value_index ).y;

	}

	// Build distribution from weights
	distribution_ = std::discrete_distribution<unsigned int>( weights.begin(), weights.end() );

}

double PropabilityDistribution::GetRandomNumber(  mutex& distribution_mutex ){

	distribution_mutex.lock();
	double value =  values_.at( distribution_( generator_ ) );
	distribution_mutex.unlock();
	return value;

}