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

randomNumberGenerator integerRNG{ 0, UINT32_MAX };

randomNumberGenerator::randomNumberGenerator( const unsigned int minValue, const unsigned int maxValue ) :
	generator{ (unsigned int) std::chrono::system_clock::now().time_since_epoch().count() },
	distribution{ minValue, maxValue }
{}


unsigned int randomNumberGenerator::getRandom( void ){

	mu.lock();
	const unsigned int randomNumber = distribution( generator );
	mu.unlock();

	return randomNumber;
}

bool randomNumberGenerator::eventHappend( const double eventPropability ){

	const unsigned int numberInterval = distribution.max() - distribution.min();
	
	const double singleValuePropability = 1. /  ( (double) numberInterval + 1. );

	const unsigned int eventIntervalSize = (unsigned int) (eventPropability / singleValuePropability);

	const unsigned int randomInteger = getRandom();


	if( randomInteger >= distribution.min() && randomInteger <= distribution.min() + eventIntervalSize ) return true;

	return false;

}



/*
	propabilityDistribution implementation
*/

propabilityDistribution::propabilityDistribution( const vector<Tuple2D> distribution_, const size_t maxNumberOfBins )
{
	// Normalise 
	distribution = Normalise( distribution_ );

	// Sort by GetAngle
	std::sort( distribution.begin(), distribution.end(), [] ( const Tuple2D& a, const Tuple2D& b ){ return a.x < b.x; } );


	// Sorted distribution by propability
	vector<Tuple2D> sortedDistribution = distribution;

	// Get the smallest probability
	std::sort( sortedDistribution.begin(), sortedDistribution.end(), [] ( const Tuple2D& a, const Tuple2D& b ){ return a.y < b.y; } );
	double smallestPropability = sortedDistribution.front().y;

	// Check against maximum number of bins
	if( 1. / smallestPropability > static_cast<double>( maxNumberOfBins ) ) smallestPropability = 1. / static_cast<double>( maxNumberOfBins );

	// Insert amount corrensponding to probability into uniform distribution
	for( const Tuple2D& currentValue : distribution ){

		const double currentProbabilty = currentValue.y;

		// How many elements of current value to add to vector
		const size_t currentBinAmount = (size_t) floor( 4. * currentProbabilty / smallestPropability + 0.5 );

		// Insert into vector
		uniformPropabilities.insert( uniformPropabilities.end(), currentBinAmount, currentValue.x );

	}
}

double propabilityDistribution::getRandom( void ) const{
	
	size_t randomIndex = integerRNG.getRandom() % uniformPropabilities.size();
	if( randomIndex >= uniformPropabilities.size() ) randomIndex = uniformPropabilities.size() - 1;
	return uniformPropabilities.at( randomIndex );
}