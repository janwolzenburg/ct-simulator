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

#include "rayScattering.h"
#include "simulation.h"


/*********************************************************************
   Implementation
*********************************************************************/


RayScattering::RayScattering( const size_t anglesAmount, const NumberRange energyRange_, const size_t energyAmount_, const UnitVector3D scatteredNormal_ ) :
	//angleResolution( Fmax( Fpos( angleResolution_ ), PI ) ),
	number_of_energies_( ForcePositive( energyAmount_ ) ),
	energy_range_( energyRange_ ),
	energy_resolution_( ( energy_range_.end() - energy_range_.start() ) / (double) ( number_of_energies_ - 1 ) ),
	scattering_plane_normal_( scatteredNormal_ )
{
	
	// GetResolution of angles
	const double angleResolution = ( 2. * PI ) / (double) ( anglesAmount - 1 );

	// Iterate all frequencies
	for( size_t currentEnergyIndex = 0; currentEnergyIndex < number_of_energies_; currentEnergyIndex++ ){

		const double currentEnergy = energy_range_.start() + (double) currentEnergyIndex * energy_resolution_;

		// Calculate pseudo propability distribution
		vector<Tuple2D> pseudoDistribution;

		// Initial photon energy
		const double a = currentEnergy / m_0c2_eV;
	
		// Iterate all angles
		for( size_t currentAngleIndex = 0; currentAngleIndex < anglesAmount - 1; currentAngleIndex++ ){

			const double t = -PI + (double) currentAngleIndex * angleResolution;

			const double pseudoProbability = ( 1. + pow( cos( t ), 2 ) ) / ( 2 * pow( 1. + a * ( 1. - cos( t ) ), 2 ) ) *
				( 1. + ( pow( a, 2 ) * pow( 1. - cos( t ), 2 ) ) / ( ( 1. + pow( cos( t ), 2 ) ) * ( 1. + a * ( 1. - cos( t ) ) ) ) );

			pseudoDistribution.emplace_back( t, pseudoProbability );

		}

		scattering_angle_distributions_.emplace_back( currentEnergy, PropabilityDistribution{ pseudoDistribution, max_number_of_bins } );


		//distributions.emplace_back( pseudoDistribution, 256 );
		//energies.push_back( currentEnergy );
	}


};

Ray RayScattering::ScatterRay( const Ray r, const Point3D newOrigin ) const{

	const UnitVector3D newDirection = r.direction().RotateConstant( scattering_plane_normal_, GetRandomAngle( r.GetMeanFrequencyOfSpectrum() ) );

	return Ray{ newDirection, newOrigin, r.properties() };

}

double RayScattering::GetRandomAngle( const double energy ) const{

	const size_t distributionIndex = ForceToMax( (size_t) floor( ( energy - energy_range_.start() ) / energy_resolution_ + 0.5 ), scattering_angle_distributions_.size() );
	
	return scattering_angle_distributions_.at( distributionIndex ).second.GetRandomNumber();

}
