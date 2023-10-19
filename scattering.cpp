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


rayScattering::rayScattering( const size_t anglesAmount, const range energyRange_, const size_t energyAmount_, const uvec3 scatteredNormal_ ) :
	//angleResolution( Fmax( Fpos( angleResolution_ ), PI ) ),
	energiesAmount( Fpos( energyAmount_ ) ),
	energyRange( energyRange_ ),
	energyResolution( ( energyRange.end - energyRange.start ) / (double) ( energiesAmount - 1 ) ),
	scatteringNormal( scatteredNormal_ )
{
	
	// Resolution of angles
	const double angleResolution = ( 2. * PI ) / (double) ( anglesAmount - 1 );

	// Iterate all frequencies
	for( size_t currentEnergyIndex = 0; currentEnergyIndex < energiesAmount; currentEnergyIndex++ ){

		const double currentEnergy = energyRange.start + (double) currentEnergyIndex * energyResolution;

		// Calculate pseudo propability distribution
		vector<v2> pseudoDistribution;

		// Initial photon energy
		const double a = currentEnergy / m_0c2_eV;
	
		// Iterate all angles
		for( size_t currentAngleIndex = 0; currentAngleIndex < anglesAmount - 1; currentAngleIndex++ ){

			const double t = -PI + (double) currentAngleIndex * angleResolution;

			const double pseudoProbability = ( 1. + pow( cos( t ), 2 ) ) / ( 2 * pow( 1. + a * ( 1. - cos( t ) ), 2 ) ) *
				( 1. + ( pow( a, 2 ) * pow( 1. - cos( t ), 2 ) ) / ( ( 1. + pow( cos( t ), 2 ) ) * ( 1. + a * ( 1. - cos( t ) ) ) ) );

			pseudoDistribution.emplace_back( t, pseudoProbability );

		}

		distributions.emplace_back( pseudoDistribution, 256 );
		energies.push_back( currentEnergy );
	}


};

ray rayScattering::scatterRay( const ray r, const pnt3 newOrigin ) const{

	const uvec3 newDirection = r.R().rotN( scatteringNormal, getRandomAngle( r.getMeanFrequency() ) );

	return ray{ newDirection, newOrigin, r.Properties() };

}

double rayScattering::getRandomAngle( const double energy ) const{

	const size_t distributionIndex = Fmax( (size_t) floor( ( energy - energyRange.start ) / energyResolution + 0.5 ), distributions.size() );
	
	return distributions.at( distributionIndex ).getRandom();

}

vector<v2> rayScattering::getDistribution( const double energy ) const{

	const size_t distributionIndex = Fmax( (size_t) floor( ( energy - energyRange.start ) / energyResolution + 0.5 ), distributions.size() );

	return distributions.at( distributionIndex ).getDistribution();

}