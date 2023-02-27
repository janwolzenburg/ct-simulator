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



rayScattering::rayScattering( const size_t anglesAmount, const range frequencyRange_, const size_t frequencyAmount_, const uvec3 scatteredNormal_ ) :
	//angleResolution( Fmax( Fpos( angleResolution_ ), PI ) ),
	frequencyAmount( Fpos( frequencyAmount_ ) ),
	frequencyRange( frequencyRange_ ),
	frequencyResolution( ( frequencyRange_.end - frequencyRange_.start ) / (double) ( frequencyAmount - 1 ) ),
	scatteringNormal( scatteredNormal_ )
{
	// Iterate all frequencies
	for( size_t currentFrequencyIndex = 0; currentFrequencyIndex < frequencyAmount; currentFrequencyIndex++ ){

		const double currentFrequency = frequencyRange_.start + (double) currentFrequencyIndex * frequencyResolution;

		// Calculate pseudo propability distribution
		vector<v2> pseudoDistribution;

		// Initial photon energy
		const double a = h_Js * currentFrequency / ( m_0c2_J );


		const double angleResolution = ( PI + PI ) / (double) ( anglesAmount - 1 );

		// Iterate all angles
		for( size_t currentAngleIndex = 0; currentAngleIndex < anglesAmount - 1; currentAngleIndex++ ){

			const double t = -PI + (double) currentAngleIndex * angleResolution;

			const double pseudoProbability = ( 1. + pow( cos( t ), 2 ) ) / ( 2 * pow( 1. + a * ( 1. - cos( t ) ), 2 ) ) *
				( 1. + ( pow( a, 2 ) * pow( 1. - cos( t ), 2 ) ) / ( ( 1. + pow( cos( t ), 2 ) ) * ( 1. + a * ( 1. - cos( t ) ) ) ) );

			pseudoDistribution.emplace_back( t, pseudoProbability );

		}

		distributions.emplace_back( pseudoDistribution, 256 );
		frequencies.emplace_back( currentFrequency );
	}


};

ray rayScattering::scatterRay( const ray r, const pnt3 newOrigin ) const{

	const uvec3 newDirection = r.R().rotN( scatteringNormal, getRandomAngle( r.getMeanFrequency() ) );

	return ray{ newDirection, newOrigin, r.Properties() };

}

double rayScattering::getRandomAngle( const double frequency ) const{

	const size_t distributionIndex = Fmax( (size_t) floor( ( frequency - frequencyRange.start ) / frequencyResolution + 0.5 ), distributions.size() );
	
	return distributions.at( distributionIndex ).getRandom();

}

vector<v2> rayScattering::getDistribution( const double frequency ) const{

	const size_t distributionIndex = Fmax( (size_t) floor( ( frequency - frequencyRange.start ) / frequencyResolution + 0.5 ), distributions.size() );

	return distributions.at( distributionIndex ).getDistribution();

}