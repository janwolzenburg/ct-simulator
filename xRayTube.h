#pragma once
/*********************************************************************
 * @file   tube.h
 * @brief  Class for roentgen tube
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


 /*********************************************************************
  Includes
*********************************************************************/

#include <map>

#include "generel.h"
#include "coordinateSystem.h"
#include "spectrum.h"
#include "ray.h"
#include "simulation.h"
#include "detectorPixel.h"


/*********************************************************************
   Definitions
*********************************************************************/


constexpr double efficiancy_constant_PerV = 1.1E-9;			// efficiancy constant for roentgen power


/*!
 * @brief Parameter for x-Ray tube
*/
class XRayTubeProperties {
	
	public:

	static const string FILE_PREAMBLE; /*!< String to prepend to file when storing as file*/
	
	/*!
	 * @brief Enumeration for anode materials
	*/
	enum Material{
		Copper,
		Molybdenum,
		Thungsten
	};

	/*!
	 * @brief Material map. Combines enumeration with name_ and atomic number_of_pixel
	*/
	static const std::map<Material, pair<string, size_t>> materials;

	/*!
	 * @brief Get enumeration from string
	 * @param material_string String to find material for
	 * @return Material enumeration
	*/
	static Material GetMaterialEnum( const string material_string );

	/*!
	 * @brief Constructor
	 * @param anode_Voltage_V Anode voltage in V 
	 * @param anodeCurrent_A Anode Current in A
	 * @param anode_material Anode material
	 * @param number_of_rays_per_pixel Amount of rays to emit per pixel
	 * @param has_filter Flag for Al-filter
	*/
	XRayTubeProperties( const double anode_Voltage_V, const double anodeCurrent_A, const Material anode_material, const size_t number_of_rays_per_pixel, 
						const bool has_filter, const double filter_cut_of_energy, const double filter_strength ) :
		anode_voltage_V( anode_Voltage_V ), anode_current_A( anodeCurrent_A ),
		anode_material( anode_material ), number_of_rays_per_pixel_( ForceToMin1( number_of_rays_per_pixel ) ),
		has_filter_( has_filter ), filter_cut_of_energy( ForceRange( filter_cut_of_energy, 0., 120000. ) ), filter_strength( ForceRange( filter_strength, .1, 10. ) )
	{};

	/*!
	 * @brief Default constructor
	*/
	XRayTubeProperties( void ) : XRayTubeProperties{ 120000., .2, Thungsten, 1, true, al_filter_cut_off_energy_eV, al_filter_gradient_factor }
	{};

	/*!
	 * @brief Constructor from serialized data_
	 * @param binary_data Reference to vector with binary data_
	 * @param current_byte Iterator to start of data in vector
	*/
	XRayTubeProperties( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	/*!
	* @brief Serialize this object
	* @param binary_data Reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;


	double anode_voltage_V;				/*!< Anode Voltage in volts*/
	double anode_current_A;				/*!< Current in ampere*/
	Material anode_material;			/*!< Atomic Number of anode material*/
	size_t number_of_rays_per_pixel_;	/*!< The amount of discrete rays to emit per detector pixel*/
	bool has_filter_;					/*!< Flag for Al-Filter*/
	double filter_cut_of_energy;
	double filter_strength;
};


class XRayTube{

	public:

	/*!
	 * @brief Constructor
	 * @param properties Tube properties
	*/
	XRayTube( CoordinateSystem* const coordinate_system, const XRayTubeProperties properties );

	/*!
	 * @brief Get beam created by tube
	 * @param detector_pixel Vector with all pixel
	 * @param detector_focus_distance Distance from pixel to focus (this tube)
	 * @return Vector with rays in XY-plane of tube's coordinate system and parallel to pixel normals
	*/
	vector<Ray> GetEmittedBeam( const vector<DetectorPixel> detector_pixel, const double detector_focus_distance ) const;

	/*!
	 * @brief Get coordinate system
	 * @return Pointer to coordinate system
	*/
	CoordinateSystem* coordinate_system( void ) const { return coordinate_system_; };

	/*!
	 * @brief Get the set number_of_pixel of pixel per ray
	 * @return Rays per Pixel
	*/
	size_t number_of_rays_per_pixel( void ) const{ return properties_.number_of_rays_per_pixel_; };

	/*!
	 * @brief Get the range of energies the tube emits
	 * @return Energy Range
	*/
	NumberRange GetEmittedEnergyRange( void ) const{ return NumberRange{ emitted_spectrum_.GetMinEnergy(), emitted_spectrum_.GetMaxEnergy() }; };

	/*!
	 * @brief Get energy of beam when switched on for exposure time
	 * @param exposure_time Time in seconds
	 * @return Energy in joule
	*/
	double GetEmittedEnergy( const double exposureTime ) const{ return radiation_power_W_ * exposureTime; };

	/*!
	 * @brief Get Ray power
	 * @return Ray power in watt
	*/
	double GetEmittedBeamPower( void ) const{ return radiation_power_W_; };

	/*!
	 * @brief Get the electrical power
	 * @return Power in watt
	*/
	double GetElectricalPower( void ) const{ return properties_.anode_voltage_V * properties_.anode_current_A; };

	/*!
	 * @brief Get the energy resolution of spectrum
	 * @return Energy resolution of spectrum
	*/
	double GetSpectralEnergyResolution( void ) const{ return emitted_spectrum_.energy_resolution(); };

	/*!
	 * @brief Get the spectrum points
	 * @return The spectrum points
	*/
	VectorPair GetEnergySpectrumPoints( void ) const;

	/*!
	 * @brief Update tube properties
	 * @param tube_properties New properties
	*/
	void UpdateProperties( const XRayTubeProperties tube_properties );


	private:
	
	static constexpr size_t number_of_points_in_spectrum_ = 30;		/*!< Amount of discrete datapoints in spectrum*/

	CoordinateSystem* coordinate_system_;	/*!< Coordinate system of tube*/

	XRayTubeProperties properties_;			/*!< Tube properties*/
	size_t anode_material_atomic_number_;	/*!< Atomic number_of_pixel of anode material*/

	double radiation_power_W_;				/*!< Total radiation power of tube in watts*/
	double max_photon_energy_eV_;			/*!< Maximum radiation energy in eV based on anode voltage*/ 

	EnergySpectrum emitted_spectrum_;		/*!< Output spectrum of tube*/

 };