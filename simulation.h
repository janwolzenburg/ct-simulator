#pragma once
/*********************************************************************
 * @file   simulation.h
 * @brief  simulation parameter
 *
 * @author Jan Wolzenburg
 * @date   Febuary 2023
 *********************************************************************/

#include "generelMath.h"
#include <vector>
using std::vector;


constexpr double max_distance_error_for_equality_mm = 1E-9;	/*!< error for equality determination of floating point numbers in mm */

constexpr double al_filter_cut_off_energy_eV = 20000.;	/*!< cut of energy in roentgen spectrum by 2mm aluminium filter */
constexpr double al_filter_gradient_factor = 3.5;				/*!< quotient of filter gradient to negative bremsstrahlung gradient */

constexpr double max_detetector_arc_angle_rad = 2. * PI * ( 120. / 360. );	/*!< maximum angle of detector arc*/
constexpr double min_detetector_arc_angle_rad = 2. * PI * ( 40. / 360. );		/*!< minimum angle of detector arc*/

constexpr double default_scatter_propability_correction = 1.;												/*!< correction factor for scatter propability*/
constexpr size_t default_max_radiation_loops = 1;																		/*!< how often can a Ray be scattered*/
constexpr double default_max_ray_angle_allowed_by_structure = 5. / 360. * 2. * PI;	/*!< default maximum rotation_angle between ray and pixel normal allowed by anti scattering structure*/

constexpr double minimum_energy_in_tube_spectrum = 10000.;		/*!< maximum energy in tube's spectrum*/
constexpr double maximum_energy_in_tube_spectrum = 210000.;		/*!< minimum energy in tube's spectrum*/

/*!
 * @brief class for storing some simulation properties
 */
class SimulationProperties{

	public:
	
	static const string FILE_PREAMBLE;					/*!< preamble to store in front of an exported file*/
	
	size_t quality;															/*!< the simulation quality*/
	double ray_step_size_mm;										/*!< stepsize during ray iteration in ray direction vector's unit*/
	size_t number_of_points_in_spectrum;				/*!< amount of discrete datapoints in spectrum*/
	size_t number_of_energies_for_scattering;		/*!< amount of discrete angles available for scattering - should be odd to include 0°*/
	size_t number_of_scatter_angles;						/*!< amount of energies used */
	size_t bins_per_energy;											/*!< when scattering: into how many bins should the photons with the same energy be divided into*/

	/*!
	 * @brief constructor
	 * @param simulation_quality quality of simulation between 0 and 99
	 */
	SimulationProperties( size_t simulation_quality );

	/*!
	 * @brief default constructor
	 */
	SimulationProperties( void ) : SimulationProperties{ 20 } {};

	/*!
	 * @brief construct from serialsized data
	 * @param binary_data data
	 * @param current_byte iterator pointing to next byte
	 */
	SimulationProperties( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );
	
	/*!
   * @brief serialize this instance
   * @param binary_data data to write to
   * @return bytes written
   */
	size_t Serialize( vector<char>& binary_data ) const;

};


extern SimulationProperties simulation_properties;		/*!< global instance of simulation properties*/