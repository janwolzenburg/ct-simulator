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

RandomNumberGenerator integer_random_number_generator{ 0, UINT32_MAX };

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
	const unsigned int event_interval = static_cast<unsigned int>( event_propability / single_value_propability );

	const unsigned int random_integer = GetRandomNumber();

	if( random_integer >= distribution_.min() && random_integer < distribution_.min() + event_interval ) return true;

	return false;
}



/*
	propabilityDistribution implementation
*/

PropabilityDistribution::PropabilityDistribution( const vector<Tuple2D> distribution ) : //, const size_t max_number_of_bins ) :
	//distribution_( Normalise( distribution ) ), // Ensure that sum of values is one
	values_( distribution.size(), 0. )
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

	/*
	const double bin_amount_scaling = 4.;		// Factor for the number of bins. When it is greater, smaller properties can appear in the uniform propability

	// Sort distribution by x value ( variate of distribution )
	std::sort( distribution_.begin(), distribution_.end(), [] ( const Tuple2D& a, const Tuple2D& b ){ return a.x < b.x; } );


	// Sorted distribution by propability
	vector<Tuple2D> sorted_distribution = distribution_;

	// Get the smallest probability
	std::sort( sorted_distribution.begin(), sorted_distribution.end(), [] ( const Tuple2D& a, const Tuple2D& b ){ return a.y < b.y; } );
	double smallest_propability = sorted_distribution.front().y;

	// Check against maximum number of bins
	if( 1. / smallest_propability > bin_amount_scaling * static_cast<double>( max_number_of_bins ) ) 
		smallest_propability = 1. / ( bin_amount_scaling * static_cast<double>( max_number_of_bins ) );

	// Insert amount corrensponding to probability into uniform distribution
	for( const Tuple2D& currentValue : distribution_ ){

		const double current_probabilty = currentValue.y;

		// How many elements of current value to add to vector. The factor 4 is used
		const size_t current_bin_amount = static_cast<size_t>( floor( bin_amount_scaling * current_probabilty / smallest_propability + 0.5 ) );

		// Insert into vector
		uniform_propabilities_.insert( uniform_propabilities_.end(), current_bin_amount, currentValue.x );

	}*/
}

double PropabilityDistribution::GetRandomNumber( void ){
	
	//size_t randomIndex = integer_random_number_generator.GetRandomNumber() % uniform_propabilities_.size();
	//if( randomIndex >= uniform_propabilities_.size() ) randomIndex = uniform_propabilities_.size() - 1;
	//return uniform_propabilities_.at( randomIndex );

	return values_.at( distribution_( integer_random_number_generator.generator() ) );

}