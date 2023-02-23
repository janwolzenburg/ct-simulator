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

static randomNumberGenerator integerRNG{ 0, INT32_MAX };

randomNumberGenerator::randomNumberGenerator( const int minValue, const int maxValue ) :
	generator{ std::chrono::system_clock::now().time_since_epoch().count() },
	distribution{ minValue, maxValue }
{}


int randomNumberGenerator::getRandom( void ){
	return distribution( generator );
}




propabilityDistribution::propabilityDistribution( const vector<v2> distribution_, const size_t maxNumberOfBins )
{
	// Normalize 
	distribution = normalize( distribution_ );

	// Sort by angle
	std::sort( distribution.begin(), distribution.end(), [] ( const v2& a, const v2& b ){ return a.x < b.x; } );


	// Sorted distribution by propability
	vector<v2> sortedDistribution = distribution;

	// Get the smallest probability
	std::sort( sortedDistribution.begin(), sortedDistribution.end(), [] ( const v2& a, const v2& b ){ return a.y < b.y; } );
	double smallestPropability = sortedDistribution.front().y;

	// Check against maximum number of bins
	//if( 1. / smallestPropability > maxNumberOfBins ) smallestPropability = 1. / maxNumberOfBins;

	// Insert amount corrensponding to probability into uniform distribution
	for( const v2& currentValue : distribution ){

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


vector<v2> propabilityDistribution::getDistribution( void ) const{
	return distribution;
}