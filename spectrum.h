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
	vector<Tuple2D> data( void ) const{ return photonflow_per_energy_; };

	/*!
	 * @brief Get the energy resolution
	 * @return Energy resolution
	*/
	double energy_resolution( void ) const{ return energy_resolution_; };

	/*!
	 * @brief Get mean energy
	 * @return Mean energy by weightening with energy occurrence
	*/
	double mean_energy( void );

	/*!
	 * @brief Get sum of photons per second
	 * @return Photons per second
	*/
	double GetSum( void ) const;

	/*!
	 * @brief Get scaled version of this spectrum
	 * @param factor Scaling factor
	 * @return Scaled spectrum
	*/
	EnergySpectrum GetEvenlyScaled( const double factor ) const;
	
	/*!
	 * @brief Get total power in eV per second. Sum of photonflow multiplied with the energy
	 * @return Total power in seconds
	*/
	double GetTotalPowerIn_eVPerSecond( void ) const;

	/*!
	 * @brief Get total power in watt 
	 * @return Total Power in watt
	*/
	double GetTotalPower( void ) const{ return GetTotalPowerIn_eVPerSecond() * J_Per_eV; };

	/*!
	 * @brief Get minimum energy
	 * @return Minimum energy
	*/
	double GetMinEnergy( void ) const{ return photonflow_per_energy_.front().x; };

	/*!
	 * @brief Get maximum energy
	 * @return Maximum energy
	*/
	double GetMaxEnergy( void ) const{ return photonflow_per_energy_.back().x; };

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
	 * @brief Scale this spectrum energy indipendent
	 * @param factor Scalar
	*/
	void ScaleEvenly( const double factor );

	/*!
	 * @brief Get mean compton cross section of photons in spectrum
	 * @return Mean compton cross section in mm^2
	*/
	double GetMeanComptonCrossSection( void ) const;


	private:

	vector<Tuple2D> photonflow_per_energy_;		/*!< 2D data sorted by energy. x is energy. y is the number of photons per second with energy in the interval dE */
	double energy_resolution_;	/*!< Resolution of energies in spectrum*/
	double mean_energy_;		/*!< Mean energy of spectrum*/
	bool mean_energy_valid_;	/*!< Flag to track whether mean energy is valid*/

	/*!
	 * @brief Update mean energy
	*/
	void UpdateMeanEnergy( void );

};