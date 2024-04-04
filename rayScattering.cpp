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


RayScattering::RayScattering(	const size_t number_of_angles, 
															const NumberRange energy_range, 
															const size_t number_of_energies, 
															const Unitvector3D scattering_normal, 
															const double maximum_angle_to_lie_in_scatter_plane ) :
	number_of_energies_( ForcePositive( number_of_energies ) ),
	angle_resolution_( ( 2. * PI ) / static_cast<double>( number_of_angles - 1 ) ),
	energy_range_( energy_range ),
	energy_resolution_( ( energy_range_.end() - energy_range_.start() ) / 
											static_cast<double>( number_of_energies_ - 1 ) ),
	scattering_plane_normal_( scattering_normal ),
	max_angle_to_lie_in_scatter_plane_( maximum_angle_to_lie_in_scatter_plane )
{
	// iterate all energies
	for( size_t energy_index = 0; energy_index < number_of_energies_; energy_index++ ){

		const double energy = energy_range_.start() + 
													static_cast<double>( energy_index ) * energy_resolution_;

		// calculate pseudo propability distribution
		vector<Tuple2D> pseudo_distribution;

		// initial photon energy
		const double a = energy / me_c2_eV;
	
		// iterate all angles
		for( size_t angle_index = 0; angle_index < number_of_angles; angle_index++ ){
			
			const double angle = -PI + static_cast<double>( angle_index ) * angle_resolution_;
			const double t = angle;
			const double pseudo_probability = 
				( 1. + pow( cos( t ), 2 ) ) / 
				( 2 * pow( 1. + a * ( 1. - cos( t ) ), 2 ) ) *
				( 1. + 
					( pow( a, 2 ) * pow( 1. - cos( t ), 2 ) ) / 
					( ( 1. + pow( cos( t ), 2 ) ) * ( 1. + a * ( 1. - cos( t ) ) ) ) 
				);

			pseudo_distribution.emplace_back( t, pseudo_probability );

			if( angle_index == ( number_of_angles - 1 ) / 2 ) 
				pseudo_distribution.emplace_back( t, pseudo_probability );

		}
		scattering_angle_distributions_.emplace_back( energy, 
																			PropabilityDistribution{ pseudo_distribution } );
	}
}

double RayScattering::GetRandomAngle( const double energy, RandomNumberGenerator& dedicated_rng ){

	const size_t distributionIndex = ForceToMax( static_cast<size_t>( floor( ( energy - energy_range_.start() )  / energy_resolution_ + 0.5 ) ), scattering_angle_distributions_.size() - 1 );

	return scattering_angle_distributions_.at( distributionIndex ).second.GetRandomNumber( dedicated_rng );

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

vector<double> ScatteringCrossSection::GetCrossSections( void ) const{
	vector<double> cross_sections;
	for( const auto& single_cross_section : cross_sections_ ){
		cross_sections.push_back( single_cross_section.y );
	}

	return cross_sections;
}