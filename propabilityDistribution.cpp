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
RandomNumberGenerator integer_random_number_generator{};

RandomNumberGenerator::RandomNumberGenerator( void ) :
	//generator_{ (unsigned int)  },
	generator_state_{ static_cast<uint64_t>( std::chrono::system_clock::now().time_since_epoch().count() ) | 
										( static_cast<uint64_t>( std::chrono::system_clock::now().time_since_epoch().count() ) << 32 ),
										static_cast<uint64_t>( std::chrono::system_clock::now().time_since_epoch().count() ) | 
										( static_cast<uint64_t>( std::chrono::system_clock::now().time_since_epoch().count() ) << 32 ) }//,
	//distribution_{ minValue, maxValue }
{

}

unsigned long long int RandomNumberGenerator::GetRandomNumber( void ){

	// Algorithm from https://prng.di.unimi.it/xoroshiro128starstar.c
	const unsigned long long int s0 = generator_state_[0];
	unsigned long long int s1 = generator_state_[1];
	const unsigned long long int result = std::rotl(s0 * 5, 7) * 9;
	s1 ^= s0;

	const unsigned long long int new_state_0 = std::rotl(s0, 24) ^ s1 ^ (s1 << 16);
	const unsigned long long int new_state_1 = std::rotl(s1, 37);
	
	mutex_.lock();
	generator_state_[0] = new_state_0;
	generator_state_[1] = new_state_1;
	mutex_.unlock();


	return result;

}

bool RandomNumberGenerator::DidARandomEventHappen( const double event_propability ){

	const unsigned long long int interval = UINT64_MAX;
	const double single_value_propability = 1. / static_cast<double>( interval );
	const unsigned long long int event_interval = 
		static_cast<unsigned long long int>( event_propability / single_value_propability );

	const unsigned long long int random_integer = GetRandomNumber();

	if( random_integer < event_interval ) 
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