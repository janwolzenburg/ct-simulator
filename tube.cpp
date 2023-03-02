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
#include "detectorPixel.h"



/*********************************************************************
   Implementation
*********************************************************************/

tube::tube( cartCSys* const cSys_, const tubeParameter parameter_ ) :
	cSys( cSys_ ),
	anodeVoltage_V(Fpos( parameter_.anodeVoltage_V )),
	anodeCurrent_A(Fpos( parameter_.anodeCurrent_A )),
	anodeAtomicNumber(Fpos( parameter_.anodeAtomicNumber )),
	totalPower_W(k_1PerV* anodeAtomicNumber* anodeCurrent_A* pow(anodeVoltage_V, 2)),
	maxRadiationFrequency_Hz(e_As * anodeVoltage_V / h_Js)
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

range tube::getFrequencyRange( void ) const{
	
	return range{ xRay_spectrum.getMinFrequency(), xRay_spectrum.getMaxFrequency()};

}

vector<ray> tube::getBeam( const vector<pixel> detectorPixel, const double detectorFocusDistance, size_t raysPerPixel, const double exposureTime ) const{

	// Force minimum of one
	raysPerPixel = Fmin1( raysPerPixel );

	const size_t numRays = raysPerPixel * detectorPixel.size();

	// Split spectrum into the ray spectra. Multiply by exposure time in seconds to get energy spectra
	const spectrum raySpectrum = xRay_spectrum.getScaled( exposureTime / (double) numRays );

	// Properties of created rays
	const rayProperties beamProperties{ raySpectrum };


	// Vector with rays
	vector<ray> rays;

	// Iterate all pixel
	for( const pixel currentPixel : detectorPixel ){
		
		// Get points on the edge of pixel

		const pnt3 pMin = currentPixel.getPnt( currentPixel.AMin(), 0);		// Point on "minimum" edge
		const pnt3 pMax = currentPixel.getPnt( currentPixel.AMax(), 0 );	// Point on "maximum" edge
		const line connectionLine{ pMax - pMin, pMin };						// Line connection the edge points

		const double edgeDistance = ( pMax - pMin ).Length();								// Distance between edge points
		const double rayOriginDistanceDelta = edgeDistance / (double) ( raysPerPixel + 1 );	// Offset of ray origins on pixel

		// Iterate all rays hitting current pixel
		for( size_t currentRayIndex = 0; currentRayIndex < raysPerPixel; currentRayIndex++ ){
			
			// Offset of current ray origin
			const double currentOffset = (double) ( currentRayIndex + 1 ) * rayOriginDistanceDelta;

			// Current ray origin
			const pnt3 currentOrigin = connectionLine.getPnt( currentOffset );

			// Tempory line pointing from pixel to tube
			const line tempLine{ currentPixel.Normal().convertTo( cSys ), currentOrigin.convertTo( cSys ) };

			// Origin of ray with specific distance to pixel
			const pnt3 rayOrigin = tempLine.getPnt( detectorFocusDistance );

			// Add ray in tube's coordinate system to vector
			rays.emplace_back( -tempLine.R(), rayOrigin, beamProperties);

		}

	}


	return rays;

}