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

#include "vectorAlgorithm.h"
#include "tube.h" 




/*********************************************************************
   Implementation
*********************************************************************/

tube::tube( cartCSys* const cSys_, const tubeParameter parameter_ ) :
	cSys( cSys_ ),
	anodeVoltage_V(Fpos( parameter_.anodeVoltage_V )),
	anodeCurrent_A(Fpos( parameter_.anodeCurrent_A )),
	anodeAtomicNumber(Fpos( parameter_.anodeAtomicNumber )),
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


vector<ray> tube::getBeam( const double beamAngle, const size_t numRays_ ) const{

	// Split spectrum into the ray spectra
	spectrum raySpectrum = xRay_spectrum.getScaled( 1. / (double) numRays_ );

	// Properties of created rays
	rayProperties beamProperties{ raySpectrum };


	// Vector with rays
	vector<ray> rays;

	// Create rays on circular arc 
	const double dAngle = beamAngle / ( (double) ( numRays_ - 1 ) );				// Angle between two rays

	// Iterate all rays to create
	for( size_t i = 0; i < numRays_; i++ ){
		const double yAngle = -( beamAngle / 2 ) + (double) i * dAngle;		// Angle between current ray and y-axis

		vec3 direction{ cSys->EyVec() };									// Start with y-axis as direction
		direction.rotZM( yAngle );											// Rotate vector around z-axis

		const ray r{ direction, cSys->OPnt(), beamProperties };				// Create ray

		rays.push_back( r );													// Store ray in ray collection
	}

	return rays;

}