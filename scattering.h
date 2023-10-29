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
#include "propability.h"
#include "ray.h"



 /*********************************************************************
	Definitions
 *********************************************************************/

/*!
 * @brief Class describing Ray scattering inside a model
*/
class rayScattering{
	
	public:

	/*!
	 * @brief Constructor
	 * @param anglesAmount How many angles in the interval from -pi to pi should be calculated
	 * @param energyRange_ How large is the energy range to calculate propabilities for
	 * @param energiesAmount_ For how many energies should  the probabilities be calculated
	 * @param scatteredNormal_ GetCenterNormal around which rays shoul´d be rpteated wehen scattered
	*/
	rayScattering( const size_t anglesAmount, const NumberRange energyRange_, const size_t energiesAmount_, const UnitVector3D scatteredNormal_ );

	/*!
	 * @brief Scatter given Ray
	 * @param direction_ Ray to scatter
	 * @param newOrigin Point where the scattering happens
	 * @return Scattered Ray
	*/
	Ray scatterRay( const Ray r, const Point3D newOrigin ) const;

	/*!
	 * @brief Get a random angle to given frequency
	 * @param frequency Frequency of Ray
	 * @return Random angöe
	*/
	double getRandomAngle( const double frequency ) const;

	/*!
	 * @brief Get the distribution to given frequency
	 * @param frequency Frequency
	 * @return Propability distribution for given frequency
	*/
	vector<Tuple2D> getDistribution( const double frequency ) const;


	private:

	size_t energiesAmount;			/*!<Amount of frequencies*/
	NumberRange energyRange;				/*!<Range of frequencies*/
	double energyResolution;		/*!<Frequency GetResolution*/

	vector<propabilityDistribution> distributions;	/*!<Vector with propability distributions*/
	vector<double> energies;						/*!<Vector with frequencies the vector with distribution describes*/

	UnitVector3D scatteringNormal;							/*!<Rotation normal for scattered rays*/
 };