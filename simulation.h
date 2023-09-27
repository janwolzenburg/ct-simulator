#pragma once
/*********************************************************************
 * @file   simulation.h
 * @brief  Simulation parameter
 *
 * @author Jan Wolzenburg
 * @date   Febuary 2023
 *********************************************************************/


 #include "generelMath.h"

constexpr double EPSILON = 1E-9;									/*!< Error for equality determination of floating point numbers in mm */

//constexpr double alFilterCutOffFrequency = 16000. * e_As / h_Js;	/*!< Cut of energy / frequency in roentgen spectrum by 2mm aluminium filter */
constexpr double alFilterCutOffEnergy_eV = 16000.;					/*!< Cut of energy in roentgen spectrum by 2mm aluminium filter */
constexpr double alFilterGradiantFactor = -3.5;						/*!< Quotient of filter gradient to bremsstrahlung gradient */

constexpr double MAX_DETECTOR_ANGLE = 2. * PI * ( 60. / 360. );		/*!<Maximum angle of detector arc*/
constexpr double MIN_DETECTOR_ANGLE = 2. * PI * ( 40. / 360. );		/*!<Minimum angle of detector arc*/



constexpr double rayStepSize_Def = 5e-2;						/*!<Stepsize during ray iteration in ray direction vector's unit*/
constexpr double completeModelScatterPropability_Def = 0.05;	/*!<Propability that a ray is scattered when transmitting through complete model*/

constexpr size_t maxRadiationLoops_Def = 16;					/*!<How often can a ray be scattered*/


