#pragma once
/*********************************************************************
 * @file   tube.h
 * @brief  Class for roentgen tube
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


 #include "generelMath.h"
 #include "spectrum.h"



 constexpr double k_1PerV = 1.1E-9;						// k constant for roentgen power
 constexpr double alFilterCutOffFrequency = 16000. * e_As / h_Js;	// Cut of energy/frequency in roentgen spectrum by 2mm aluminium filter
 constexpr double alFilterGradiantFactor = -3.5;		// Quotient of filter gradient to bremsstrahlung gradient




class tube{

	public:


	tube( const double anodeVoltage_, const double anodeCurrent_, const size_t anodeAtomicNumber_ ) ;



	private:

	const size_t numPointsInSpectrum = 20; /*!<Amount of discrete datapoints in spectrum*/

	double anodeVoltage_V;					/*!<Anode voltage in volts*/
	double anodeCurrent_A;					/*!<Anode current in volts*/
	size_t anodeAtomicNumber;				/*!<Atomic number of anode material*/

	double totalPower_W;					/*!<Total radiation power of tube in watts*/
	double maxRadiationFrequency_Hz;		/*!<Maximum radiation frequency in Hz based on anode voltage*/ 



	spectrum xRay_spectrum;					/*!<Output spectrum of tube*/

 };