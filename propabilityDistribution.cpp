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

#include "generelMath.h"
#include "propabilityDistribution.h"
#include "vectorAlgorithm.h"



/*********************************************************************
   Implementation
*********************************************************************/



/*
	randomNumberGenerator implementation
*/

//RandomNumberGenerator integer_random_number_generator{};

RandomNumberGenerator::RandomNumberGenerator(const unsigned long long int extra_seed) :
	//generator_{ (unsigned int)  },
	generator_state_{ static_cast<uint64_t>( std::chrono::system_clock::now().time_since_epoch().count() + extra_seed ) |
										( static_cast<uint64_t>( std::chrono::system_clock::now().time_since_epoch().count() + extra_seed ) << 32 ),
										static_cast<uint64_t>( std::chrono::system_clock::now().time_since_epoch().count() + extra_seed ) |
										( static_cast<uint64_t>( std::chrono::system_clock::now().time_since_epoch().count() + extra_seed ) << 32 ) }//,
	//distribution_{ minValue, maxValue }
{

}

unsigned int RandomNumberGenerator::GetRandomNumber( void ){

	// algorithm from https://prng.di.unimi.it/xoroshiro128starstar.c
	const unsigned long long int s0 = generator_state_[0];
	unsigned long long int s1 = generator_state_[1];
	const unsigned long long int result = std::rotl(s0 * 5, 7) * 9;
	s1 ^= s0;

	const unsigned long long int new_state_0 = std::rotl(s0, 24) ^ s1 ^ (s1 << 16);
	const unsigned long long int new_state_1 = std::rotl(s1, 37);
	
	//mutex_.lock();
	generator_state_[0] = new_state_0;
	generator_state_[1] = new_state_1;
	//mutex_.unlock();

	return static_cast<unsigned int>( result & 0x00000000FFFFFFFF );
}

unsigned short int RandomNumberGenerator::GetRandomShortNumber(void) {
	return static_cast<unsigned short int>( ( GetRandomNumber() & 0xFFFF0000 ) >> 16 );
}

bool RandomNumberGenerator::DidARandomEventHappen( const double event_propability ){

	const unsigned int interval = UINT32_MAX;
	const double single_value_propability = 1. / static_cast<double>( interval );
	const unsigned int event_interval = 
		static_cast<unsigned int>( event_propability / single_value_propability );

	const unsigned int random_integer = GetRandomNumber();

	if( random_integer < event_interval ) 
		return true;

	return false;
}



/*
	propabilityDistribution implementation
*/

PropabilityDistribution::PropabilityDistribution( vector<Tuple2D> distribution ) :	
	values_( distribution.size(), 0. )//,
	//generator_{ (unsigned int) std::chrono::system_clock::now().time_since_epoch().count() }		
{
	/*
	// vector with weights of variates
	vector<double> weights( distribution.size(), 0. );

	// get values and weight from distribution
	for( size_t value_index = 0; value_index < distribution.size(); value_index++ ){
	
		
		weights.at( value_index ) = distribution.at( value_index ).y;

	}*/

	std::sort(distribution.begin(), distribution.end(), [](Tuple2D a, Tuple2D b) { return a.y < b.y; });

	double smallest_weight = INFINITY;
	double weight_sum = 0.;

	for (const auto& [value, weight] : distribution) {

		if (weight < smallest_weight)
			smallest_weight = weight;

		weight_sum += weight;

	}

	const double weight_per_bin = weight_sum / static_cast<double>( number_of_elements );

	unsigned int current_index = 0;
	for (const auto& [value, weight] : distribution) {

		values_.at(current_index) = value;

		const unsigned int number_of_elements = static_cast<unsigned int>( round( weight / weight_per_bin) );

		discrete_distribution_.insert(discrete_distribution_.end(), number_of_elements, current_index++ );
	
	}

	if( number_of_elements > discrete_distribution_.size() )
		discrete_distribution_.insert(discrete_distribution_.end(), number_of_elements - discrete_distribution_.size(), current_index - 1);

	// build distribution from weights
	//distribution_ = std::discrete_distribution<unsigned int>( weights.begin(), weights.end() );

}

double PropabilityDistribution::GetRandomNumber( RandomNumberGenerator& generator ) const{//mutex& distribution_mutex ){

	//distribution_mutex.lock();
	//double value =  values_.at( distribution_( generator_ ) );
	unsigned short int random_number = generator.GetRandomShortNumber();
	unsigned int index = discrete_distribution_.at(random_number);
	const double value = values_.at(index);
	//distribution_mutex.unlock();
	return value;

}