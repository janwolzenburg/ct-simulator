/*********************************************************************
 * @file   scattering.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   Febuary 2023
 *********************************************************************/



 /*********************************************************************
   Includes
*********************************************************************/

#include "scattering.h"


/*********************************************************************
   Implementation
*********************************************************************/



scatteredAngles::scatteredAngles( const double angleResolution_, const range frequencyRange_, const double frequencyResolution_ ) :
	angleResolution( Fmax( Fpos( angleResolution_ ), PI ) ),
	frequencyRange( frequencyRange_ ),
	frequencyResolution( Fpos( frequencyResolution_ ) ){

	// Iterate all frequencies
	for( double currentFrequency = frequencyRange.start; currentFrequency < frequencyRange.end; currentFrequency += frequencyResolution ){

		// Calculate pseudo propability distribution
		vector<v2> pseudoDistribution;

		// Initial photon energy
		const double a = h_Js * currentFrequency / ( m_0c2_J );

		// Iterate all angles
		for( double t = 0; t <= PI; t += angleResolution ){

			const double pseudoProbability = ( 1. + pow( cos( t ), 2 ) ) / ( 2 * pow( 1. + a * ( 1. - cos( t ) ), 2 ) ) *
				( 1. + ( pow( a, 2 ) * pow( 1. - cos( t ), 2 ) ) / ( ( 1. + pow( cos( t ), 2 ) ) * ( 1. + a * ( 1. - cos( t ) ) ) ) );

			pseudoDistribution.emplace_back( t, pseudoProbability );

		}

		distributions.emplace_back( pseudoDistribution, 256 );
		frequencies.emplace_back( currentFrequency );
	}


};


double scatteredAngles::getRandomAngle( const double frequency ) const{

	const size_t distributionIndex = Fmax( (size_t) floor( ( frequency - frequencyRange.start ) / frequencyResolution + 0.5 ), distributions.size() );
	
	return distributions.at( distributionIndex ).getRandom();

}

vector<v2> scatteredAngles::getDistribution( const double frequency ) const{

	const size_t distributionIndex = Fmax( (size_t) floor( ( frequency - frequencyRange.start ) / frequencyResolution + 0.5 ), distributions.size() );

	return distributions.at( distributionIndex ).getDistribution();

}