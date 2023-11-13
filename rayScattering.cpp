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
	energy_resolution_( ( energy_range_.end() - energy_range_.start() ) / static_cast<double>( number_of_energies_ - 1 ) ),
	scattering_plane_normal_( scatteredNormal_ )
{
	
	// GetResolution of angles
	const double angleResolution = ( 2. * PI ) / static_cast<double>( anglesAmount - 1 );

	// Iterate all frequencies
	for( size_t currentEnergyIndex = 0; currentEnergyIndex < number_of_energies_; currentEnergyIndex++ ){

		const double currentEnergy = energy_range_.start() + static_cast<double>( currentEnergyIndex ) * energy_resolution_;

		// Calculate pseudo propability distribution
		vector<Tuple2D> pseudoDistribution;

		// Initial photon energy
		const double a = currentEnergy / me_c2_eV;
	
		// Iterate all angles
		for( size_t currentAngleIndex = 0; currentAngleIndex < anglesAmount - 1; currentAngleIndex++ ){

			const double t = -PI + static_cast<double>( currentAngleIndex ) * angleResolution;

			const double pseudoProbability = ( 1. + pow( cos( t ), 2 ) ) / ( 2 * pow( 1. + a * ( 1. - cos( t ) ), 2 ) ) *
				( 1. + ( pow( a, 2 ) * pow( 1. - cos( t ), 2 ) ) / ( ( 1. + pow( cos( t ), 2 ) ) * ( 1. + a * ( 1. - cos( t ) ) ) ) );

			pseudoDistribution.emplace_back( t, pseudoProbability );

		}

		scattering_angle_distributions_.emplace_back( currentEnergy, PropabilityDistribution{ pseudoDistribution, max_number_of_bins } );


		//distributions.emplace_back( pseudoDistribution, 256 );
		//energies.push_back( currentEnergy );
	}


};

bool RayScattering::ScatterRay( Ray& r, const VoxelData voxel_data, const double distance_traveled_mm, const double propability_correction, const Point3D newOrigin ) const{


	// Check if ray is scattered

	// Calculate compton cross-section
	const double ray_energy_eV = r.GetMeanEnergyOfSpectrum();
	const double compton_cross_section = Compton_Cross_Section::GetInstance().GetCrossSection( ray_energy_eV );

	// Get the "attenuatuion coefficient" and the "propability"
	const double electron_density = electron_density_water_1Permm3 * voxel_data.GetAttenuationAtEnergy( reference_energy_for_mu_eV ) / mu_water;

	const double coefficient_1Permm = electron_density * compton_cross_section;

	const double scatter_propability = 1. - exp( -coefficient_1Permm * distance_traveled_mm );

	// Scattering happened?
	if( !integer_random_number_generator.DidARandomEventHappen( scatter_propability * propability_correction ) ) return false;

	
	// Ray is scattered
	
	// Scattering angle
	const double angle = GetRandomAngle( ray_energy_eV );
	
	// Calculate anergy loss
	
	const double energy_after_scatter = 1. / ( Per_me_c2_eV * ( 1. - cos( angle ) ) + 1. / ray_energy_eV );
	const double relative_energy_loss = energy_after_scatter / ray_energy_eV;

	// Scale spectrum according to energy loss
	r.ScaleSpectrum( relative_energy_loss );

	const UnitVector3D newDirection = r.direction().RotateConstant( scattering_plane_normal_, angle );
	r.SetOrigin( newOrigin );
	r.SetDirection( newDirection );

	return true;

}

double RayScattering::GetRandomAngle( const double energy ) const{

	const size_t distributionIndex = ForceToMax( static_cast<size_t>( floor( ( energy - energy_range_.start() ) ) / energy_resolution_ + 0.5 ), scattering_angle_distributions_.size() );
	
	return scattering_angle_distributions_.at( distributionIndex ).second.GetRandomNumber();

}
