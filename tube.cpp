/*********************************************************************
 * @file   tube.cpp
 * @brief  Implementations of tube classes
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



 /*********************************************************************
   Includes
*********************************************************************/

#include "vectorAlgotrithm.h"
#include "tube.h" 




/*********************************************************************
   Implementation
*********************************************************************/

tube::tube(const double anodeVoltage_, const double anodeCurrent_, const size_t anodeAtomicNumber_) :
	anodeVoltage_V(Fpos(anodeVoltage_)),
	anodeCurrent_A(Fpos(anodeCurrent_)),
	anodeAtomicNumber(Fpos(anodeAtomicNumber_)),
	totalPower_W(k_1PerV* anodeAtomicNumber* anodeCurrent_A* pow(anodeVoltage_V, 2)),
	maxRadiationFrequency_Hz(e_As* anodeVoltage_V / h_Js)
{

	// Frequencies
	vector<double> frequencies = linearSpace(alFilterCutOffFrequency, maxRadiationFrequency_Hz, numPointsInSpectrum);

	// Values
	vector<double> spectralPower(frequencies.size(), 0.);



	// Frequency to which the filter dominates spectral behavious
	double changeFrequency = frequencies.front() + (frequencies.back() - frequencies.front()) / (1. - alFilterGradiantFactor);

	// Fill value vector
	for (auto freqIt = frequencies.begin(); freqIt < frequencies.end(); freqIt++) {
		size_t curIdx = freqIt - frequencies.begin();	// Current index

		double bremsGradient = -1;											// Gradient of brems spectrum
		double filterGradient = alFilterGradiantFactor * bremsGradient;		// Gradient of filter spectrum

		// Filter dominates
		if (*freqIt < changeFrequency) {
			spectralPower.at(curIdx) = (*freqIt - frequencies.front()) * filterGradient;
			continue;
		}

		// Bremsspectrum dominates
		spectralPower.at(curIdx) = (frequencies.back() - *freqIt) * (-bremsGradient);
	}


	// Calculate correction factor for spectrum for its values to sum up to totalPower
	double currentSum = sum(spectralPower);
	double correctionFactor = totalPower_W / currentSum;

	// Correct values for sums to match
	scale(spectralPower, correctionFactor);

	// Write frequency and power values to spectrum
	xRay_spectrum = spectrum{ frequencies, spectralPower };

};