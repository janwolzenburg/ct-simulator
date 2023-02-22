#pragma once
/*********************************************************************
 * @file   test_rays.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   Febuary 2023
 *********************************************************************/


 /*********************************************************************
	 Includes
  *********************************************************************/

  #include "test_rays.h"
  #include "scattering.h"


bool test_scattered_angle_propabilities( void ){

	double angleResolution = 5. / 360 * 2 * PI;
	range frequencyRange = range{ alFilterCutOffFrequency, e_As * 150000. / h_Js };
	size_t numFrequencies = 20;
	double frequencyResolution = frequencyRange.Resolution( numFrequencies );

	scatteredAngles anglePropabilites{ angleResolution, frequencyRange, frequencyResolution };

}