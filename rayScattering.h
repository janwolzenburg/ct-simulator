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
	 * @param location Point where the scattering happens
	 * @return Scattered Ray
	*/
	Ray ScatterRay( const Ray ray, const Point3D location ) const;

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