#pragma once
/*********************************************************************
 * @file   scattering.h
 * @brief  Classes for handling ray scattering
 *
 * @author Jan Wolzenburg
 * @date   Febuary 2023
 *********************************************************************/

#include "generel.h"
#include "propability.h"



#include "tube.h"


class scatteredAngles{
	
	public:

	scatteredAngles( const double angleResolution_, const range frequencyRange_, const double frequencyResolution_ );

	double getRandomAngle( const double frequency ) const;

	private:
	double angleResolution;
	range frequencyRange;
	double frequencyResolution;

	vector<propabilityDistribution> distributions;
	vector<double> frequencies;
 };