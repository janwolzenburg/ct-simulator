#pragma once
/*********************************************************************
 * @file   xRayTube.h
 * @brief  class for roentgen tube
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
#include "energySpectrum.h"
#include "ray.h"
#include "simulation.h"
#include "detectorPixel.h"


/*********************************************************************
   Definitions
*********************************************************************/


/*!
 * @brief parameter for x-Ray tube
*/
class XRayTubeProperties {
	
	friend class XRayTube;

	public:

	static const string FILE_PREAMBLE; /*!< string to prepend to file when storing as file*/
	
	/*!
	 * @brief enumeration for anode materials
	*/
	enum Material{
		Copper,
		Molybdenum,
		Thungsten
	};

	/*!
	 * @brief material map. combines enumeration with name and atomic number
	*/
	static const std::map<Material, pair<string, size_t>> materials;

	/*!
	 * @brief get enumeration from string
	 * @param material_string string to find material for
	 * @return material enumeration
	*/
	static Material GetMaterialEnum( const string material_string );

	/*!
	 * @brief constructor
	 * @param anode_Voltage_V anode voltage in V 
	 * @param anodeCurrent_A anode current in A
	 * @param anode_material anode material
	 * @param number_of_rays_per_pixel amount of rays to emit per pixel
	 * @param has_filter flag for roentgen-filter
	 * @param filter_cut_of_energy energy below which the tube has no photon output
	 * @param filter_gradient gradient of the filter curve
	*/
	XRayTubeProperties( const double anode_Voltage_V, const double anodeCurrent_A, const Material anode_material, const size_t number_of_rays_per_pixel, 
						const bool has_filter, const double filter_cut_of_energy, const double filter_gradient ) :
		anode_voltage_V( anode_Voltage_V ), anode_current_A( anodeCurrent_A ),
		anode_material( anode_material ), number_of_rays_per_pixel_( ForceToMin1( number_of_rays_per_pixel ) ),
		has_filter_( has_filter ), filter_cut_of_energy( ForceRange( filter_cut_of_energy, 0., 120000. ) ), filter_gradient( ForceRange( filter_gradient, .1, 10. ) ),
		spectral_energy_resolution( ( anode_voltage_V - minimum_energy_in_tube_spectrum ) / static_cast<double>( simulation_properties.number_of_points_in_spectrum - 1 ) )
	{};

	/*!
	 * @brief default constructor
	*/
	XRayTubeProperties( void ) : XRayTubeProperties{ 120000., .2, Thungsten, 1, true, al_filter_cut_off_energy_eV, al_filter_gradient_factor }
	{};

	/*!
	 * @brief constructor from serialized data
	 * @param binary_data reference to vector with binary data
	 * @param current_byte iterator to start of data in vector
	*/
	XRayTubeProperties( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	/*!
	 * @brief serialize this object
	 * @param binary_data reference to vector where data will be appended
	 * @return written bytes
	*/
	size_t Serialize( vector<char>& binary_data ) const;


	double anode_voltage_V;							/*!< anode Voltage in volts*/
	double anode_current_A;							/*!< current in ampere*/
	Material anode_material;						/*!< atomic number of anode material*/
	size_t number_of_rays_per_pixel_;		/*!< the amount of discrete rays to emit per detector pixel*/
	bool has_filter_;										/*!< flag for filter*/
	double filter_cut_of_energy;				/*!< energy under which all radiating is absorbed by filter*/
	double filter_gradient;							/*!< gradient of filter*/
	double spectral_energy_resolution;	/*!< resolution of equally spaced spectrum*/


};


/*!
 * @brief class for an x-ray tube
 */
class XRayTube{

	public:

	/*!
	 * @brief constructor
	 * @param coordinate_system tube's coordinate system
	 * @param properties tube properties
	*/
	XRayTube( CoordinateSystem* const coordinate_system, const XRayTubeProperties properties );

	/*!
	 * @brief get beam created by tube
	 * @param detector_pixel vector with all pixel
	 * @param detector_focus_distance distance from pixel to focus (this tube)
	 * @return vector with rays in XY-plane of tube's coordinate system and parallel to pixel normals
	*/
	vector<Ray> GetEmittedBeam( const vector<DetectorPixel> detector_pixel, const double detector_focus_distance ) const;

	/*!
	 * @brief get coordinate system
	 * @return pointer to coordinate system
	*/
	CoordinateSystem* coordinate_system( void ) const { return coordinate_system_; };

	/*!
	 * @brief get the set number of pixel per ray
	 * @return rays per pixel
	*/
	size_t number_of_rays_per_pixel( void ) const{ return properties_.number_of_rays_per_pixel_; };

	/*!
	 * @brief get the range of energies the tube emits
	 * @return energy range
	*/
	NumberRange GetEmittedEnergyRange( void ) const{ return NumberRange{ emitted_spectrum_.GetMinEnergy(), emitted_spectrum_.GetMaxEnergy() }; };

	/*!
	 * @brief get energy of beam when switched on for exposure time
	 * @param exposure_time time in seconds
	 * @return energy in joule
	*/
	double GetEmittedEnergy( const double exposure_time ) const{ return radiation_power_W_ * exposure_time; };

	/*!
	 * @brief get ray power
	 * @return ray power in watt
	*/
	double GetEmittedBeamPower( void ) const{ return radiation_power_W_; };

	/*!
	 * @brief get the electrical power
	 * @return power in watt
	*/
	double GetElectricalPower( void ) const{ return properties_.anode_voltage_V * properties_.anode_current_A; };

	/*!
	 * @brief get the energy resolution of spectrum
	 * @return energy resolution of spectrum
	*/
	double GetSpectralEnergyResolution( void ) const{ return properties_.spectral_energy_resolution; };

	/*!
	 * @brief get tube properties
	 * @return properties
	*/
	XRayTubeProperties properties( void ) const{ return properties_; };

	/*!
	 * @brief get the spectrum points
	 * @return the spectrum points
	*/
	VectorPair GetEnergySpectrumPoints( void ) const;

	/*!
	 * @brief get mean energy of spectrum
	 * @return mean energy
	*/
	double GetMeanEnergy( void ){ return emitted_spectrum_.mean_energy(); };

	/*!
	 * @brief update tube properties
	 * @param tube_properties new properties
	*/
	void UpdateProperties( const XRayTubeProperties tube_properties );


	private:
	
	CoordinateSystem* coordinate_system_;	/*!< coordinate system of tube*/

	XRayTubeProperties properties_;				/*!< tube properties*/
	size_t anode_material_atomic_number_;	/*!< atomic number_of_pixel of anode material*/

	double radiation_power_W_;						/*!< total radiation power of tube in watts*/
	double max_photon_energy_eV_;					/*!< maximum radiation energy in eV based on anode voltage*/ 

	EnergySpectrum emitted_spectrum_;			/*!< output spectrum of tube*/

 };