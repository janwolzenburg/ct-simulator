#pragma once
/*********************************************************************
 * @file   simulation.h
 * @brief  Simulation parameter
 *
 * @author Jan Wolzenburg
 * @date   Febuary 2023
 *********************************************************************/


 #include "generelMath.h"

constexpr double max_distance_error_for_equality_mm = 1E-9;							/*!< Error for equality determination of floating point numbers in mm */

constexpr double al_filter_cut_off_energy_eV = 16000.;						/*!< Cut of energy in roentgen spectrum by 2mm aluminium filter */
constexpr double al_filter_gradiant_factor = -3.5;							/*!< Quotient of filter gradient to bremsstrahlung gradient */

constexpr double max_detetector_arc_angle_rad = 2. * PI * ( 90. / 360. );	/*!<Maximum GetAngle of detector arc*/
constexpr double min_detetector_arc_angle_rad = 2. * PI * ( 40. / 360. );	/*!<Minimum GetAngle of detector arc*/

constexpr double default_ray_step_size_mm = 5e-2;							/*!<Stepsize during ray iteration in ray direction vector's unit*/
constexpr double default_complete_model_scatter_propability = 0.05;			/*!<Propability that a ray is scattered when transmitting through complete model*/

constexpr size_t default_max_radiation_loops = 16;							/*!<How often can a ray be scattered*/