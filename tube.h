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


enum MATERIAL{
	COPPER,
	MOLYBDENUM,
	THUNGSTEN
};


/*!
 * @brief Parameter for x-ray tube
*/
class tubeParameter {
	
	public:

	static const string FILE_PREAMBLE;
	static const std::map<MATERIAL, std::pair<string, size_t>> material;

	public:
	tubeParameter( const double anodeVoltage_V_, const double anodeCurrent_A_, const MATERIAL anodeMaterial_ ) :
		anodeVoltage_V( anodeVoltage_V_ ),
		anodeCurrent_A( anodeCurrent_A_ ),
		anodeMaterial( anodeMaterial_ )
	{};

	tubeParameter( void ) :
		tubeParameter{ 53000., .2, THUNGSTEN }
	{};

	tubeParameter( const vector<char>& binData, vector<char>::const_iterator& it ) : 
		anodeVoltage_V( deSerializeBuildIn( 53000., binData, it ) ),
		anodeCurrent_A( deSerializeBuildIn( .2, binData, it ) ),
		anodeMaterial( (MATERIAL) deSerializeBuildIn( toUnderlying( MATERIAL::THUNGSTEN ), binData, it ) )
	{

	}


	static const MATERIAL getEnum( const string materialString ){
		for( auto& [ matEnum, value ] : tubeParameter::material ){

			if( materialString == value.first ){

				return matEnum;
				
			}
		}

		return THUNGSTEN;

	}

	/*!
		* @brief Serialize this object
		* @param binData Reference to vector where data will be appended
	*/
	size_t serialize( vector<char>& binData ) const{
		size_t numBytes = 0;

		numBytes += serializeBuildIn( anodeVoltage_V, binData );
		numBytes += serializeBuildIn( anodeCurrent_A, binData );
		numBytes += serializeBuildIn( toUnderlying( anodeMaterial ), binData );

		return numBytes;
	}

	double anodeVoltage_V;		/*!<Anode Voltage in volts*/
	double anodeCurrent_A;		/*!<Current in ampere*/
	MATERIAL anodeMaterial;	/*!<Atomic Number of anode material*/
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
	inline cartCSys* CSys( void ) const { return cSys; };

	/*!
	 * @brief Get the range of frequencies the tube emits
	 * @return Frequency Range
	*/
	range getFrequencyRange( void ) const;

	private:

	cartCSys* cSys;							/*!<Coordinate system of tube*/

	size_t numPointsInSpectrum = 20;	/*!<Amount of discrete datapoints in spectrum*/

	double anodeVoltage_V;					/*!<Anode voltage in volts*/
	double anodeCurrent_A;					/*!<Anode current in volts*/
	size_t anodeAtomicNumber;					/*!<Atomic number of anode material*/

	double totalPower_W;					/*!<Total radiation power of tube in watts*/
	double maxRadiationFrequency_Hz;		/*!<Maximum radiation frequency in Hz based on anode voltage*/ 

	spectrum xRay_spectrum;					/*!<Output spectrum of tube*/


 };