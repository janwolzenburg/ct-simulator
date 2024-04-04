#pragma once
/*********************************************************************
 * @file   spectrum.h
 * @brief  class for x Ray spectrum
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
 * @brief class for storing a spectrum
*/
class EnergySpectrum {

	public:

	/*!
	 * @brief default constructor
	*/
	EnergySpectrum(void) :
		mean_energy_( 0. ), mean_energy_valid_( false )
	{};

	/*!
	 * @brief constructor
	 * @param energy_quantaties Energy values and their occurrences in spectrum
	*/
	EnergySpectrum( const VectorPair& energy_quantaties );

	/*!
	 * @brief constructr
	 * @param energy_quantaties  Energy values and their occurrences in spectrum
	 */
	EnergySpectrum( const vector<Tuple2D>& energy_quantaties );

	/*!
	 * @brief get raw data
	 * @return vector of points
	*/
	vector<Tuple2D> data( void ) const{ return photonflow_per_energy_; };

	/*!
	 * @brief get mean energy
	 * @return mean energy by weightening with energy occurrence
	*/
	double mean_energy( void );

	/*!
	 * @brief get sum of photons per second
	 * @return Photons per second
	*/
	double GetSum( void ) const;

	/*!
	 * @brief get scaled version of this spectrum
	 * @param factor Scaling factor
	 * @return scaled spectrum
	*/
	EnergySpectrum GetEvenlyScaled( const double factor ) const;
	
	/*!
	 * @brief get total power in eV per second. Sum of photonflow multiplied with the energy
	 * @return total power in seconds
	*/
	double GetTotalPowerIn_eVPerSecond( void ) const;

	/*!
	 * @brief get total power in watt 
	 * @return total Power in watt
	*/
	double GetTotalPower( void ) const{ return GetTotalPowerIn_eVPerSecond() * J_Per_eV; };

	/*!
	 * @brief get minimum energy
	 * @return minimum energy
	*/
	double GetMinEnergy( void ) const{ return photonflow_per_energy_.front().x; };

	/*!
	 * @brief get maximum energy
	 * @return maximum energy
	*/
	double GetMaxEnergy( void ) const{ return photonflow_per_energy_.back().x; };

	/*!
	 * @brief get number of discrete energies in spectrum
	 * @return number of energies
	*/
	size_t GetNumberOfEnergies( void ) const{ return photonflow_per_energy_.size(); };

	/*!
	 * @brief get photonflow for energy
	 * @param energy Energy
	 * @return Photonflow for given energy
	*/
	double GetPhotonflow( const double energy ) const;

	/*!
	 * @brief get index of energy
	 * @param energy Energy
	 * @return index of energy in data
	*/
	size_t GetEnergyIndex( const double energy ) const;

	/*!
	 * @brief get energy value at index
	 * @param index Index of energy
	 * @return Energy value at index
	*/
	double GetEnergy( const size_t index ) const;

	/*!
	 * @brief Modify spectrum
	 * @param modification_function Function taking reference to spectrum point to modify
	*/
	void Modify( std::function<void( Tuple2D& )> modification_function );
	
	/*!
	 * @brief attenuate spectrum according to voxel data
	 * @param voxel_data Voxel data
	 * @param distance Distance traveled in voxel
	*/
	void GetAbsorped( const VoxelData& voxel_data, const double distance );

	/*!
	 * @brief scale this spectrum energy indipendent
	 * @param factor Scalar
	*/
	void ScaleEvenly( const double factor );

	/*!
	 * @brief scale specific photonflow
	 * @param energy_index Energy index
	 * @param factor Factor to scale with
	*/
	void ScaleEnergy( const size_t energy_index, const double factor );

	/*!
	 * @brief scale specific photonflow
	 * @param energy Energy
	 * @param factor Factor to scale with
	*/
	void ScaleEnergy( const double energy, const double factor );


	private:

	vector<Tuple2D> photonflow_per_energy_;		/*!< 2D data sorted by energy. x is energy. y is the number of photons per second with energy in the interval dE */
	double mean_energy_;		/*!< mean energy of spectrum*/
	bool mean_energy_valid_;	/*!< Flag to track whether mean energy is valid*/

	/*!
	 * @brief Update mean energy
	*/
	void UpdateMeanEnergy( void );

};