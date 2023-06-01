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



/*
	tubeParameter
*/

const string tubeParameter::FILE_PREAMBLE{ "TUBEPARAMETER_FILE_PREAMBLE" };


const std::map < MATERIAL, std::pair<string, size_t>> tubeParameter::material{
		{ COPPER,		std::make_pair( "COPPER", 29 ) },
		{ MOLYBDENUM,	std::make_pair( "MOLYBDENUM", 42 ) },
		{ THUNGSTEN,	std::make_pair( "THUNGSTEN", 74 ) }
};


tubeParameter::tubeParameter( const double anodeVoltage_V_, const double anodeCurrent_A_, const MATERIAL anodeMaterial_ ) :
	anodeVoltage_V( anodeVoltage_V_ ),
	anodeCurrent_A( anodeCurrent_A_ ),
	anodeMaterial( anodeMaterial_ )
{}


tubeParameter::tubeParameter( void ) :
	tubeParameter{ 53000., .2, THUNGSTEN }
{}


tubeParameter::tubeParameter( const vector<char>& binData, vector<char>::const_iterator& it ) :
	anodeVoltage_V( deSerializeBuildIn( 53000., binData, it ) ),
	anodeCurrent_A( deSerializeBuildIn( .2, binData, it ) ),
	anodeMaterial( (MATERIAL) deSerializeBuildIn( toUnderlying( MATERIAL::THUNGSTEN ), binData, it ) )
{}


const MATERIAL tubeParameter::getEnum( const string materialString ){
	for( auto& [matEnum, value] : tubeParameter::material ){

		if( materialString == value.first ){

			return matEnum;

		}
	}

	return THUNGSTEN;

}


size_t tubeParameter::serialize( vector<char>& binData ) const{
	size_t numBytes = 0;


	numBytes += serializeBuildIn( FILE_PREAMBLE, binData );
	numBytes += serializeBuildIn( anodeVoltage_V, binData );
	numBytes += serializeBuildIn( anodeCurrent_A, binData );
	numBytes += serializeBuildIn( toUnderlying( anodeMaterial ), binData );

	return numBytes;
}



/*
	tube
*/

tube::tube( cartCSys* const cSys_, const tubeParameter parameter_ ) :
	cSys( cSys_ ),
	anodeVoltage_V( Fpos( parameter_.anodeVoltage_V )),
	anodeCurrent_A( Fpos( parameter_.anodeCurrent_A )),
	anodeAtomicNumber( Fpos( tubeParameter::material.at( parameter_.anodeMaterial ).second ) ),
	totalPower_W( k_1PerV * anodeAtomicNumber * anodeCurrent_A * pow( anodeVoltage_V, 2 ) ),
	maxRadiationEnergy_eV( anodeVoltage_V )
{

	// Frequencies
	vector<double> energies = linearSpace( alFilterCutOffEnergy_eV, maxRadiationEnergy_eV, numPointsInSpectrum);

	// Values
	vector<double> spectralPower( energies.size(), 0.);

	// Frequency to which the filter dominates spectral behavious
	double changeEnergy = energies.front() + ( energies.back() - energies.front()) / (1. - alFilterGradiantFactor);

	// Fill value vector
	for (auto energyIt = energies.begin(); energyIt < energies.end(); energyIt++) {
		size_t curIdx = energyIt - energies.begin();	// Current index

		double bremsGradient = -1;											// Gradient of brems spectrum
		double filterGradient = alFilterGradiantFactor * bremsGradient;		// Gradient of filter spectrum

		// Filter dominates
		if ( *energyIt < changeEnergy ) {
			spectralPower.at(curIdx) = ( *energyIt - energies.front() ) * filterGradient;
			continue;
		}

		// Bremsspectrum dominates
		spectralPower.at(curIdx) = ( energies.back() - *energyIt ) * ( -bremsGradient );
	}


	// Calculate correction factor for spectrum for its values to sum up to totalPower
	double currentSum = sum(spectralPower);
	double correctionFactor = totalPower_W / currentSum;

	// Correct values for sums to match
	scale(spectralPower, correctionFactor);

	// Write frequency and power values to spectrum
	xRay_spectrum = spectrum( energies, spectralPower );

};

range tube::getEnergyRange( void ) const{
	
	return range{ xRay_spectrum.getMin(), xRay_spectrum.getMax()};

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


vectorPair tube::spectrumPoints( const bool integral ) const{

	vectorPair points;
	const vector<v2> spectrumPoints = xRay_spectrum.rawData();

	double xCorrection = 1.;
	double yCorrection = 1.;

	if( integral ){
		yCorrection *= 1 / xRay_spectrum.EnergyResolution();
	}


	for( auto& point : spectrumPoints ){

		points.first.push_back( point.x );


		if( integral ){
			points.second.push_back( point.y * yCorrection );
		}
		else{
			points.second.push_back( point.y );
		}
	}

	return points;
}