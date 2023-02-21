#pragma once
/*********************************************************************
 * @file   scattering.h
 * @brief  Classes for handling ray scattering
 *
 * @author Jan Wolzenburg
 * @date   Febuary 2023
 *********************************************************************/

 #include "generel.h"
 #include "vectorAlgorithm.h"
 #include <algorithm>
 #include "generelMath.h"
 #include "tube.h"

class propabilityDistrubution{


	public:

	propabilityDistrubution( const vector<v2> distribution_, const size_t maxNumberOfBins ) : 
		//numberOfBins( Fpos( numberOfBins_ ) ),
		distribution( normalize(distribution_) )
	{
		// Sorted distribution by propability
		vector<v2> sortedDistribution = distribution;

		// Get the smallest probability
		std::sort( sortedDistribution.begin(), sortedDistribution.end(), [] ( const v2& a, const v2& b ) { return a.y < b.y; } );
		double smallestPropability = sortedDistribution.begin()->y;

		// Check against maximum number of bins
		if( 1. / smallestPropability > maxNumberOfBins ) smallestPropability = 1. / maxNumberOfBins;

		// Insert amount corrensponding to probability into uniform distribution
		for( auto currentValue : sortedDistribution ){

			const double currentProbabilty = currentValue.y;

			const size_t currentBinAmount = (size_t) currentProbabilty / smallestPropability;

			uniformPropabilities.insert( uniformPropabilities.end(), currentBinAmount, currentValue.x );

		}
	
	};


	double getRandom( void ) const {

	};

	private:

	vector<v2> distribution;

	size_t numberOfBins;
	vector<double> uniformPropabilities;

 };

class scatteredAngles{
	
	public:

	double angleResolution = 5. / 360 * 2 * PI;
	range frequencyRange = range{ alFilterCutOffFrequency, e_As * 150000. / h_Js };
	size_t numFrequencies = 20;
	double frequencyResolution = frequencyRange.Resolution( numFrequencies );

	

 };