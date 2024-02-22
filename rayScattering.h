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
#include "vector3D.h"
#include "propabilityDistribution.h"



 /*********************************************************************
	Definitions
 *********************************************************************/

/*!
 * @brief Class describing Ray scattering inside a model
*/
class RayScattering{
	
	static constexpr size_t max_number_of_bins = 512; /*!< Maximum amount of bins used to created uniform angle propability distribution*/

	public:

	/*!
	 * @brief Constructor
	 * @param number_of_angles How many angles in the interval from -pi to pi should be calculated
	 * @param energy_range How large is the energy range to calculate propabilities for
	 * @param number_of_energies For how many energies should  the probabilities be calculated
	 * @param scatter_plane_normal Normal of the plane in which rays are scattered
	 * @param max_angle_to_lie_in_plane Maximum angle between a scattered ray and the scattering plane for a ascttered ray to not be discarded
	*/
	RayScattering(	const size_t number_of_angles, const NumberRange energy_range, const size_t number_of_energies, 
					const UnitVector3D scatter_plane_normal, const double max_angle_to_lie_in_plane );

	/*!
	 * @brief Get scattering plane normal
	 * @return Plane normal
	*/
	UnitVector3D scattering_plane_normal( void ) const{ return scattering_plane_normal_; };

	/*!
	 * @brief Get angle resolution
	 * @return Resolution in rad
	*/
	double angle_resolution( void ) const{ return angle_resolution_; };

	double max_angle_to_lie_in_plane( void ) const{ return max_angle_to_lie_in_scatter_plane_; };

	/*!
	 * @brief Get a random angle to given energy
	 * @param energy Mean energy of Ray
	 * @return Random angöe
	*/
	double GetRandomAngle( const double energy_eV,  mutex& scattering_properties_mutex );


	private:

	size_t number_of_energies_;				/*!< Amount of energies*/
	double angle_resolution_;				/*!< Angle resolution*/
	NumberRange energy_range_;				/*!< Range of energies*/
	double energy_resolution_;				/*!< Energy resolution*/

	vector<pair<double, PropabilityDistribution>> scattering_angle_distributions_;	/*!< Vector with energies and the correspondeing angle distribution*/

	UnitVector3D scattering_plane_normal_;	/*!< Rotation normal for scattered rays*/
 
	double max_angle_to_lie_in_scatter_plane_;		/*!< Maximum angle between a scattered ray and the scattering plane for a ascttered ray to not be discarded*/
 };



/*!
 * @brief Class to store precomputed cross sections of electrons which scatter photons
*/
class ScatteringCrossSection{

	public:
	
	static constexpr double desired_energy_resolution = 1000;
	
	static ScatteringCrossSection& GetInstance( void );


	ScatteringCrossSection( const ScatteringCrossSection& ) = delete;
	
	ScatteringCrossSection& operator=( const ScatteringCrossSection& ) = delete;
	
	/*!
	 * @brief Get cross section for photon with given energy
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
	ScatteringCrossSection( void );

 };