#pragma once
/*********************************************************************
 * @file   scattering.h
 * @brief  Classes for handling Ray scattering
 *
 * @author Jan Wolzenburg
 * @date   Febuary 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include "generel.h"
#include "propabilityDistribution.h"
#include "ray.h"



 /*********************************************************************
	Definitions
 *********************************************************************/

/*!
 * @brief Class describing Ray scattering inside a model
*/
class RayScattering{
	
	static constexpr size_t max_number_of_bins = 256; /*!<Maximum amount of bins used to created uniform angle propability distribution*/

	public:

	/*!
	 * @brief Constructor
	 * @param number_of_angles How many angles in the interval from -pi to pi should be calculated
	 * @param energy_range How large is the energy range to calculate propabilities for
	 * @param number_of_energies For how many energies should  the probabilities be calculated
	 * @param scatter_plane_normal Normal of the plane in which rays are scattered
	*/
	RayScattering( const size_t number_of_angles, const NumberRange energy_range, const size_t number_of_energies, const UnitVector3D scatter_plane_normal );

	/*!
	 * @brief Scatter given Ray
	 * @param ray Ray to scatter
	 * @param voxel_data Voxel properties of voxel the ray is inside
	 * @param distance_traveled Distance the ray traveled in voxel in mm
	 * @param propability_correction Correction factor for scattering propability
	 * @param location Point where the scattering happens
	 * @return True when scattered
	*/
	bool ScatterRay( Ray& ray, const VoxelData voxel_data, const double distance_traveled, const double propability_correction, const Point3D location ) const;

	/*!
	 * @brief Get a random angle to given energy
	 * @param energy Mean energy of Ray
	 * @return Random angöe
	*/
	double GetRandomAngle( const double energy_eV ) const;


	private:

	size_t number_of_energies_;				/*!<Amount of frequencies*/
	NumberRange energy_range_;				/*!<Range of frequencies*/
	double energy_resolution_;				/*!<Frequency resolution*/

	vector<pair<double, PropabilityDistribution>> scattering_angle_distributions_;	/*!<Vector with energies and the correspondeing angle distribution*/

	UnitVector3D scattering_plane_normal_;	/*!<Rotation normal for scattered rays*/
 };

/*!
 * @brief Class to store precomputed cross sections of electrons which scatter photons
*/
class Compton_Cross_Section{

	public:
	
	static constexpr double energy_start_eV = 20000;
	static constexpr double energy_end_eV = 200000;
	static constexpr double desired_energy_resolution = 1000;

	Compton_Cross_Section( const Compton_Cross_Section& ) = delete;
	
	Compton_Cross_Section& operator=( const Compton_Cross_Section& ) = delete;
	
	static Compton_Cross_Section& GetInstance( void ){
		static Compton_Cross_Section instance;
		return instance;
	}

	double GetCrossSection( const double energy ) const{
		
		const size_t energy_index = static_cast<size_t>( floor( ForcePositive( energy - energy_start_eV ) / energy_resolution_ + 0.5 ) );
		if( energy_index >= number_of_energies_  ) return cross_sections_.back().y;

		return cross_sections_.at( energy_index ).y;
	}

	private:
	
	size_t number_of_energies_;			/*!< Amount of energies calculated*/
	double energy_resolution_;			/*!< Resolution of energies*/
	vector<Tuple2D> cross_sections_;	/*!< Vector with energies in eV and corresponding cross sections in mm^2 */

	Compton_Cross_Section( void ) : 
		number_of_energies_( static_cast<size_t>( ( ( energy_end_eV - energy_start_eV ) / desired_energy_resolution ) ) + 1 ),
		energy_resolution_( ( energy_end_eV - energy_start_eV ) / static_cast<double>( number_of_energies_ - 1 ) ),
		cross_sections_( number_of_energies_, Tuple2D{} )
	{
		
		for( size_t current_energy_index = 0; current_energy_index < number_of_energies_; current_energy_index++ ){
			
			const double energy_eV = ( energy_start_eV + static_cast<double>( current_energy_index ) * energy_resolution_ );
			const double reduced_energy = energy_eV / reduced_energy_divisor_eV;
			const double e = reduced_energy;

			const double cross_section = 2. * PI * pow( r_e_mm, 2. ) * 
										( 
											( 
												( ( 1. + e ) / pow( e, 2. ) ) * 
												( 2. * ( 1. + e ) / ( 1. + 2. * pow( e, 2. ) ) - log( 1. + 2 * e ) / e ) 
											) + 
											log( 1. + 2. * e ) / ( 2. * e ) -
											( 1. + 3. * e ) / pow( 1. + 2. * e, 2. )
										);

			cross_sections_.at( current_energy_index ) = Tuple2D{ energy_eV, cross_section };

		}
	}


 };