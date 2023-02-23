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
#include "rays.h"



class scatteredAngles{
	
	public:

	scatteredAngles( const double angleResolution_, const range frequencyRange_, const double frequencyResolution_, const uvec3 scatteredNormal_ );

	ray scatterRay( const ray r, const pnt3 newOrigin, const double frequency ) const;

	double getRandomAngle( const double frequency ) const;

	vector<v2> getDistribution( const double frequency ) const;


	private:
	double angleResolution;
	range frequencyRange;
	double frequencyResolution;

	vector<propabilityDistribution> distributions;
	vector<double> frequencies;

	uvec3 scatteringNormal;
 };