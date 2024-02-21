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


RayScattering::RayScattering(	const size_t anglesAmount, const NumberRange energyRange_, const size_t energyAmount_, 
								const UnitVector3D scatteredNormal_, const double max_angle_to_lie_in_plane ) :
	number_of_energies_( ForcePositive( energyAmount_ ) ),
	angle_resolution_( ( 2. * PI ) / static_cast<double>( anglesAmount - 1 ) ),
	energy_range_( energyRange_ ),
	energy_resolution_( ( energy_range_.end() - energy_range_.start() ) / static_cast<double>( number_of_energies_ - 1 ) ),
	scattering_plane_normal_( scatteredNormal_ ),
	max_angle_to_lie_in_plane_( max_angle_to_lie_in_plane )
{
	// Iterate all frequencies
	for( size_t currentEnergyIndex = 0; currentEnergyIndex < number_of_energies_; currentEnergyIndex++ ){

		const double currentEnergy = energy_range_.start() + static_cast<double>( currentEnergyIndex ) * energy_resolution_;

		// Calculate pseudo propability distribution
		vector<Tuple2D> pseudoDistribution;

		// Initial photon energy
		const double a = currentEnergy / me_c2_eV;
	
		// Iterate all angles
		for( size_t currentAngleIndex = 0; currentAngleIndex < anglesAmount; currentAngleIndex++ ){

			const double t = -PI + static_cast<double>( currentAngleIndex ) * angle_resolution_;

			const double pseudoProbability = ( 1. + pow( cos( t ), 2 ) ) / ( 2 * pow( 1. + a * ( 1. - cos( t ) ), 2 ) ) *
				( 1. + ( pow( a, 2 ) * pow( 1. - cos( t ), 2 ) ) / ( ( 1. + pow( cos( t ), 2 ) ) * ( 1. + a * ( 1. - cos( t ) ) ) ) );

			pseudoDistribution.emplace_back( t, pseudoProbability );

			if( currentAngleIndex == ( anglesAmount - 1 ) / 2 ) 
				pseudoDistribution.emplace_back( t, pseudoProbability );

		}

		scattering_angle_distributions_.emplace_back( currentEnergy, PropabilityDistribution{ pseudoDistribution, max_number_of_bins } );


	}


};

double RayScattering::GetRandomAngle( const double energy ) const{

	const size_t distributionIndex = ForceToMax( static_cast<size_t>( floor( ( energy - energy_range_.start() )  / energy_resolution_ + 0.5 ) ), scattering_angle_distributions_.size() - 1 );
	
	return scattering_angle_distributions_.at( distributionIndex ).second.GetRandomNumber();

}

ScatteringCrossSection& ScatteringCrossSection::GetInstance( void ){
	static ScatteringCrossSection instance;
	return instance;
}

double ScatteringCrossSection::GetCrossSection( const double energy ) const{
		
	const size_t energy_index = static_cast<size_t>( floor( 
		ForcePositive( energy - minimum_energy_in_tube_spectrum ) / 
		energy_resolution_ + 0.5 ) );
	if( energy_index >= number_of_energies_  ) return cross_sections_.back().y;

	return cross_sections_.at( energy_index ).y;
}

ScatteringCrossSection::ScatteringCrossSection( void ) : 
	number_of_energies_( static_cast<size_t>( ( ( maximum_energy_in_tube_spectrum - minimum_energy_in_tube_spectrum ) / desired_energy_resolution ) ) + 1 ),
	energy_resolution_( ( maximum_energy_in_tube_spectrum - minimum_energy_in_tube_spectrum ) / static_cast<double>( number_of_energies_ - 1 ) ),
	cross_sections_( number_of_energies_, Tuple2D{} )
{
	auto cross_sections_collision = cross_sections_;
	for( size_t current_energy_index = 0; current_energy_index < number_of_energies_; current_energy_index++ ){
			
		const double energy_eV = ( minimum_energy_in_tube_spectrum + static_cast<double>( current_energy_index ) * energy_resolution_ );
		const double reduced_energy = energy_eV / reduced_energy_divisor_eV;
		const double e = reduced_energy;

		const double collision_cross_section = 2. * PI * pow( r_e_mm, 2. ) * 
									( 
										( 
											( 1. + e ) / pow( e, 2. ) * 
											( 2. * ( 1. + e ) / ( 1. + 2. * e ) - log( 1. + 2 * e ) / e ) 
										) + 
										log( 1. + 2. * e ) / ( 2. * e ) -
										( 1. + 3. * e ) / pow( 1. + 2. * e, 2. )
									);

		cross_sections_.at( current_energy_index ) = Tuple2D{ energy_eV, collision_cross_section };
	}
}