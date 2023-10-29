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

#include "propability.h"
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

bool RandomNumberGenerator::DidARandomEventHappen( const double eventPropability ){

	const unsigned int numberInterval = distribution_.max() - distribution_.min();
	
	const double singleValuePropability = 1. /  ( (double) numberInterval + 1. );

	const unsigned int eventIntervalSize = (unsigned int) (eventPropability / singleValuePropability);

	const unsigned int randomInteger = GetRandomNumber();


	if( randomInteger >= distribution_.min() && randomInteger <= distribution_.min() + eventIntervalSize ) return true;

	return false;

}



/*
	propabilityDistribution implementation
*/

PropabilityDistribution::PropabilityDistribution( const vector<Tuple2D> distribution_, const size_t maxNumberOfBins )
{
	// Normalise 
	distribution_ = Normalise( distribution_ );

	// Sort by angle
	std::sort( distribution_.begin(), distribution_.end(), [] ( const Tuple2D& a, const Tuple2D& b ){ return a.x < b.x; } );


	// Sorted distribution by propability
	vector<Tuple2D> sortedDistribution = distribution_;

	// Get the smallest probability
	std::sort( sortedDistribution.begin(), sortedDistribution.end(), [] ( const Tuple2D& a, const Tuple2D& b ){ return a.y < b.y; } );
	double smallestPropability = sortedDistribution.front().y;

	// Check against maximum number_of_pixel of bins
	if( 1. / smallestPropability > static_cast<double>( maxNumberOfBins ) ) smallestPropability = 1. / static_cast<double>( maxNumberOfBins );

	// Insert amount corrensponding to probability into uniform distribution
	for( const Tuple2D& currentValue : distribution_ ){

		const double currentProbabilty = currentValue.y;

		// How many elements of current value to add to vector
		const size_t currentBinAmount = (size_t) floor( 4. * currentProbabilty / smallestPropability + 0.5 );

		// Insert into vector
		uniform_propabilities_.insert( uniform_propabilities_.end(), currentBinAmount, currentValue.x );

	}
}

double PropabilityDistribution::getRandom( void ) const{
	
	size_t randomIndex = integer_random_number_generator.GetRandomNumber() % uniform_propabilities_.size();
	if( randomIndex >= uniform_propabilities_.size() ) randomIndex = uniform_propabilities_.size() - 1;
	return uniform_propabilities_.at( randomIndex );
}