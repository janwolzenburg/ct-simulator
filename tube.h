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

	tubeParameter( const double anodeVoltage_V_, const double anodeCurrent_A_, const MATERIAL anodeMaterial_ );

	tubeParameter( void );

	tubeParameter( const vector<char>& binData, vector<char>::const_iterator& it );

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
	inline cartCSys* CSys( void ) const { return cSys; };

	/*!
	 * @brief Get the range of frequencies the tube emits
	 * @return Frequency Range
	*/
	range getFrequencyRange( void ) const;

	vectorPair spectrumPoints( void ) const{

		vectorPair points;
		const vector<v2> spectrumPoints = xRay_spectrum.rawData();


		for( auto& point : spectrumPoints ){
			points.first.push_back( point.x );
			points.second.push_back( point.y );
		}

		return points;
	}


	private:

	cartCSys* cSys;							/*!<Coordinate system of tube*/

	size_t numPointsInSpectrum = 30;		/*!<Amount of discrete datapoints in spectrum*/

	double anodeVoltage_V;					/*!<Anode voltage in volts*/
	double anodeCurrent_A;					/*!<Anode current in volts*/
	size_t anodeAtomicNumber;					/*!<Atomic number of anode material*/

	double totalPower_W;					/*!<Total radiation power of tube in watts*/
	double maxRadiationFrequency_Hz;		/*!<Maximum radiation frequency in Hz based on anode voltage*/ 

	spectrum xRay_spectrum;					/*!<Output spectrum of tube*/


 };