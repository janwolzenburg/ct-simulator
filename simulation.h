#pragma once
/*********************************************************************
 * @file   simulation.h
 * @brief  Simulation parameter
 *
 * @author Jan Wolzenburg
 * @date   Febuary 2023
 *********************************************************************/


 #include "generelMath.h"

constexpr double max_distance_error_for_equality_mm = 1E-9;					/*!< Error for equality determination of floating point numbers in mm */

constexpr double al_filter_cut_off_energy_eV = 20000.;						/*!< Cut of energy in roentgen spectrum by 2mm aluminium filter */
constexpr double al_filter_gradient_factor = 3.5;							/*!< Quotient of filter gradient to negative bremsstrahlung gradient */

constexpr double max_detetector_arc_angle_rad = 2. * PI * ( 120. / 360. );	/*!< Maximum angle of detector arc*/
constexpr double min_detetector_arc_angle_rad = 2. * PI * ( 40. / 360. );	/*!< Minimum angle of detector arc*/

constexpr double default_ray_step_size_mm = 1e-3;							/*!< Stepsize during Ray iteration in Ray direction vector's unit*/
constexpr double default_scatter_propability_correction = 0.1;				/*!< Correction factor for scatter propability*/

constexpr size_t default_max_radiation_loops = 1;							/*!< How often can a Ray be scattered*/

constexpr double default_max_ray_angle_allowed_by_structure = 5. / 360. * 2. * PI;	/*!< Default maximum arc_angle between ray and pixel normal allowed by anti scattering structure*/

constexpr size_t number_of_scatter_angles = 128;							/*!< Amount of discrete angles available for scattering*/
constexpr size_t number_of_energies_for_scattering = 64;					/*!< Amount of energies used */

constexpr size_t bins_per_energy = 16;		/*!< When scattering: Into how many bins should the photons with the same energy be divided into*/

constexpr double scattered_ray_intensity_factor = 0.1;	/*!< Additional factor to apply to the simple intensity of scattered ray. Used to make real and simple result look similar. */