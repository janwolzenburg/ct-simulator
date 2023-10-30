#pragma once
/*********************************************************************
 * @file   spectrum.h
 * @brief  Class for x Ray spectrum
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



 /*********************************************************************
   Includes
*********************************************************************/

#include <functional>
#include "generel.h"


/*********************************************************************
   Definitions
*********************************************************************/

/*!
 * @brief Class for storing a spectrum
*/
class EnergySpectrum {

	public:

	/*!
	 * @brief Default constructor
	*/
	EnergySpectrum(void) :
		energy_resolution_( 1. ), mean_energy_( 0. )
	{};

	/*!
	 * @brief Constructor
	 * @param energy_quantaties Energy values and their occurrences in spectrum
	*/
	EnergySpectrum( const VectorPair energy_quantaties );

	/*!
	 * @brief Get raw data_
	 * @return Vector of points
	*/
	vector<Tuple2D> data( void ) const { return data_; };

	/*!
	 * @brief Get the energy resolution
	 * @return Energy resolution
	*/
	double energy_resolution( void ) const { return energy_resolution_; };

	/*!
	 * @brief Get mean energy
	 * @return Mean energy by weightening with energy occurrence
	*/
	double mean_energy( void ) const{ return mean_energy_; };

	/*!
	 * @brief Get scaled version of this spectrum
	 * @param factor Scaling factor
	 * @return Scaled spectrum
	*/
	EnergySpectrum GetScaled( const double factor ) const;

	/*!
	 * @brief Get sum of all magintudes
	 * @return Sum of all magintudes
	*/
	double GetTotal( void ) const;

	/*!
	 * @brief Get minimum x value
	 * @return Minimum x value
	*/
	double GetMinEnergy( void ) const{ return data_.front().x; };

	/*!
	 * @brief Get maximum x value
	 * @return Maximum x value
	*/
	double GetMaxEnergy( void ) const{ return data_.back().x; };

	/*!
	 * @brief Modify spectrum
	 * @param modification_function Function taking reference to spectrum point to modify
	*/
	void Modify( std::function<void( Tuple2D& )> modification_function );
	
	/*!
	 * @brief Scale this spectrum
	 * @param factor Factor
	*/
	void Scale( const double factor );


	private:

	vector<Tuple2D> data_;		/*!<2D point data sorted by x value*/
	double energy_resolution_;	/*!<Reolution of energies in spectrum*/
	double mean_energy_;		/*!<Mean energy of spectrum*/

	/*!
	 * @brief Update mean value
	*/
	void UpdateMeanEnergy( void );

};