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

#include <vector>
using std::vector;
#include <map>

#include "generelMath.h"
#include "cartesian.h"
#include "spectrum.h"
#include "rays.h"
#include "detectorPixel.h"
#include "simulation.h"



/*********************************************************************
   Definitions
*********************************************************************/


constexpr double k_1PerV = 1.1E-9;									// k constant for roentgen power


/*!
 * @brief Parameter for x-ray tube
*/
class tubeParameter {
	
	public:

	static const string FILE_PREAMBLE;
	

	enum MATERIAL{
		COPPER,
		MOLYBDENUM,
		THUNGSTEN
	};

	/*!
	 * @brief Material map. Combines enumeration with name and atomic number
	*/
	static const std::map<MATERIAL, std::pair<string, size_t>> material;

	public:

	/*!
	 * @brief Constructor
	 * @param anodeVoltage_V_ Anode voltage in V 
	 * @param anodeCurrent_A_ Anode Current in A
	 * @param anodeMaterial_ Anode material
	*/
	tubeParameter( const double anodeVoltage_V_, const double anodeCurrent_A_, const MATERIAL anodeMaterial_ ) :
		anodeVoltage_V( anodeVoltage_V_ ),
		anodeCurrent_A( anodeCurrent_A_ ),
		anodeMaterial( anodeMaterial_ )
	{};

	tubeParameter( void ) : 
		tubeParameter{ 53000., .2, THUNGSTEN }
	{};

	tubeParameter( const vector<char>& binData, vector<char>::const_iterator& it );

	/*!
	 * @brief Get enumeration from string
	 * @param materialString String to find material for
	 * @return Material
	*/
	static const MATERIAL getEnum( const string materialString );

	/*!
		* @brief Serialize this object
		* @param binData Reference to vector where data will be appended
	*/
	size_t serialize( vector<char>& binData ) const;


	public:

	double anodeVoltage_V;		/*!<Anode Voltage in volts*/
	double anodeCurrent_A;		/*!<Current in ampere*/
	MATERIAL anodeMaterial;		/*!<Atomic Number of anode material*/

};


class tube{

	public:


	/*!
	 * @brief Constructor
	 * @param parameter_ Tube parameters
	*/
	tube( cartCSys* const cSys_, const tubeParameter parameter_ );

	/*!
	 * @brief Get beam created by tube
	 * @param detectorPixel Vector with all pixel
	 * @param detectorFocusDistance Distance from pixel to focus (this tube)
	 * @param raysPerPixel Amount of rays per pixel
	 * @param exposureTime Exposure time in seconds. For energy calculation
	 * @return Vector with rays in XY-plane of tube's coordinate system and parallel to pixel normals
	*/
	vector<ray> getBeam( const vector<pixel> detectorPixel, const double detectorFocusDistance, const size_t raysPerPixel, const double exposureTime ) const;

	/*!
	 * @brief Get coordinate system
	 * @return Pointer to coordinate system
	*/
	cartCSys* CSys( void ) const { return cSys; };

	/*!
	 * @brief Get the range of energies the tube emits
	 * @return Energy Range
	*/
	range getEnergyRange( void ) const{ return range{ xRay_spectrum.getMin(), xRay_spectrum.getMax() }; };

	double getEnergy( const double exposureTime ) const;

	double rayPower( void ) const{ return totalPower_W; };

	double electricalPower( void ) const{ return anodeVoltage_V * anodeCurrent_A; };

	/*!
	 * @brief Get the spectrum points
	 * @param integral Flag whether the sum or the integral should be equal to the tube power. 
	 * @return 
	*/
	vectorPair spectrumPoints( const bool integral = false ) const;


	private:

	cartCSys* cSys;							/*!<Coordinate system of tube*/

	size_t numPointsInSpectrum = 30;		/*!<Amount of discrete datapoints in spectrum*/

	double anodeVoltage_V;					/*!<Anode voltage in volts*/
	double anodeCurrent_A;					/*!<Anode current in volts*/
	size_t anodeAtomicNumber;				/*!<Atomic number of anode material*/

	double totalPower_W;					/*!<Total radiation power of tube in watts*/
	double maxRadiationEnergy_eV;			/*!<Maximum radiation energy in eV based on anode voltage*/ 

	spectrum xRay_spectrum;					/*!<Output spectrum of tube*/


 };