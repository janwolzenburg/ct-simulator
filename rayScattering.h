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
	
	static constexpr size_t max_number_of_bins = 256; /*!< Maximum amount of bins used to created uniform angle propability distribution*/

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

	size_t number_of_energies_;				/*!< Amount of frequencies*/
	NumberRange energy_range_;				/*!< Range of frequencies*/
	double energy_resolution_;				/*!< Frequency resolution*/

	vector<pair<double, PropabilityDistribution>> scattering_angle_distributions_;	/*!< Vector with energies and the correspondeing angle distribution*/

	UnitVector3D scattering_plane_normal_;	/*!< Rotation normal for scattered rays*/
 };



/*!
 * @brief Class to store precomputed cross sections of electrons which scatter photons
*/
class Compton_Cross_Section{

	public:
	
	static constexpr double energy_start_eV = 20000;
	static constexpr double energy_end_eV = 200000;
	static constexpr double desired_energy_resolution = 1000;
	
	static Compton_Cross_Section& GetInstance( void );


	Compton_Cross_Section( const Compton_Cross_Section& ) = delete;
	
	Compton_Cross_Section& operator=( const Compton_Cross_Section& ) = delete;
	
	/*!
	 * @brief Get compton cross section for photon with given energy
	 * @param energy Energy of photon
	 * @return Cross section in mm^2
	*/
	double GetCrossSection( const double energy ) const;


	private:
	
	size_t number_of_energies_;			/*!< Amount of energies calculated*/
	double energy_resolution_;			/*!< Resolution of energies*/
	vector<Tuple2D> cross_sections_;	/*!< Vector with energies in eV and corresponding cross sections in mm^2 */

	/*!
	 * @brief Constructor
	*/
	Compton_Cross_Section( void );

 };