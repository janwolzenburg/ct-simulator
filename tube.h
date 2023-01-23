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

#include "generelMath.h"
#include "cartesian.h"
#include "spectrum.h"
#include "rays.h"




/*********************************************************************
   Definitions
*********************************************************************/


constexpr double k_1PerV = 1.1E-9;									// k constant for roentgen power
constexpr double alFilterCutOffFrequency = 16000. * e_As / h_Js;	// Cut of energy/frequency in roentgen spectrum by 2mm aluminium filter
constexpr double alFilterGradiantFactor = -3.5;						// Quotient of filter gradient to bremsstrahlung gradient


/*!
 * @brief Parameter for x-ray tube
*/
struct tubeParameter {
	double anodeVoltage_V;		/*!<Anode Voltage in volts*/
	double anodeCurrent_A;		/*!<Current in ampere*/
	size_t anodeAtomicNumber;	/*!<Atomic Number of anode material*/
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
	 * @param beamAngle Beam angle
	 * @param numRays_ Amount of rays
	 * @return Vector with rays in XY-plane of tube's coordinate system and symmetrical to y-axis
	*/
	vector<ray> getBeam( const double beamAngle, const size_t numRays_ ) const;

	cartCSys* CSys( void ) const;

	private:

	cartCSys* cSys;							/*!<Coordinate system of tube*/

	const size_t numPointsInSpectrum = 20;	/*!<Amount of discrete datapoints in spectrum*/

	double anodeVoltage_V;					/*!<Anode voltage in volts*/
	double anodeCurrent_A;					/*!<Anode current in volts*/
	size_t anodeAtomicNumber;				/*!<Atomic number of anode material*/

	double totalPower_W;					/*!<Total radiation power of tube in watts*/
	double maxRadiationFrequency_Hz;		/*!<Maximum radiation frequency in Hz based on anode voltage*/ 

	spectrum xRay_spectrum;					/*!<Output spectrum of tube*/


 };