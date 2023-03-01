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


/*!
 * @brief Class describing ray scattering inside a model
*/
class rayScattering{
	
	public:

	/*!
	 * @brief Constructor
	 * @param anglesAmount How many angles in the interval from -pi to pi should be calculated
	 * @param frequencyRange_ How large is the frequency range to calculate propabilities for
	 * @param frequencyAmount_ For how many frequencies shoul  the probabilities be calculated
	 * @param scatteredNormal_ Normal around which rays shoul´d be rpteated wehen scattered
	*/
	rayScattering( const size_t anglesAmount, const range frequencyRange_, const size_t frequencyAmount_, const uvec3 scatteredNormal_ );

	/*!
	 * @brief Scatter given ray
	 * @param r Ray to scatter
	 * @param newOrigin Point where the scattering happens
	 * @return Scattered ray
	*/
	ray scatterRay( const ray r, const pnt3 newOrigin ) const;

	/*!
	 * @brief Get a random angle to given frequency
	 * @param frequency Frequency of ray
	 * @return Random angöe
	*/
	double getRandomAngle( const double frequency ) const;

	/*!
	 * @brief Get the distribution to given frequency
	 * @param frequency Frequency
	 * @return Propability distribution for given frequency
	*/
	vector<v2> getDistribution( const double frequency ) const;


	private:

	size_t frequencyAmount;			/*!<Amount of frequencies*/
	range frequencyRange;			/*!<Range of frequencies*/
	double frequencyResolution;		/*!<Frequency Resolution*/

	vector<propabilityDistribution> distributions;	/*!<Vector with propability distributions*/
	vector<double> frequencies;						/*!<Vector with frequencies the vector with distribution describes*/

	uvec3 scatteringNormal;							/*!<Rotation normal for scattered rays*/
 };