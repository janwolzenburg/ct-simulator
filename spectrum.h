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
#include "voxel.h"


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
		energy_resolution_( 1. ), mean_energy_( 0. ), mean_energy_valid_( false )
	{};

	/*!
	 * @brief Constructor
	 * @param energy_quantaties Energy values and their occurrences in spectrum
	*/
	EnergySpectrum( const VectorPair energy_quantaties );

	/*!
	 * @brief Get raw data
	 * @return Vector of points
	*/
	vector<Tuple2D> data( void ) const{ return data_; };

	/*!
	 * @brief Get the energy resolution
	 * @return Energy resolution
	*/
	double energy_resolution( void ){ return energy_resolution_; };

	/*!
	 * @brief Get mean energy
	 * @return Mean energy by weightening with energy occurrence
	*/
	double mean_energy( void );

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
	 * @brief Get minimum energy
	 * @return Minimum energy
	*/
	double GetMinEnergy( void ) const{ return data_.front().x; };

	/*!
	 * @brief Get maximum energy
	 * @return Maximum energy
	*/
	double GetMaxEnergy( void ) const{ return data_.back().x; };

	/*!
	 * @brief Modify spectrum
	 * @param modification_function Function taking reference to spectrum point to modify
	*/
	void Modify( std::function<void( Tuple2D& )> modification_function );
	
	/*!
	 * @brief Attenuate spectrum according to voxel data
	 * @param voxel_data Voxel data
	 * @param distance Distance traveled in voxel
	*/
	void Attenuate( const VoxelData& voxel_data, const double distance );

	/*!
	 * @brief Scale this spectrum
	 * @param factor Scalar
	*/
	void Scale( const double factor );


	private:

	vector<Tuple2D> data_;		/*!<2D data sorted by energy*/
	double energy_resolution_;	/*!<Resolution of energies in spectrum*/
	double mean_energy_;		/*!<Mean energy of spectrum*/
	bool mean_energy_valid_;	/*!<Flag to track whether mean energy is valid*/

	/*!
	 * @brief Update mean energy
	*/
	void UpdateMeanEnergy( void );

};