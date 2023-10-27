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
#include "simulation.h"
#include "serialization.h"


/*********************************************************************
   Implementation
*********************************************************************/



/*
	tubeParameter
*/

const string tubeParameter::FILE_PREAMBLE{ "TUBEPARAMETER_FILE_PREAMBLE" };


const std::map < tubeParameter::MATERIAL, std::pair<string, size_t>> tubeParameter::material{
		{ COPPER,		std::make_pair( "COPPER", 29 ) },
		{ MOLYBDENUM,	std::make_pair( "MOLYBDENUM", 42 ) },
		{ THUNGSTEN,	std::make_pair( "THUNGSTEN", 74 ) }
};

tubeParameter::tubeParameter( const vector<char>& binary_data, vector<char>::const_iterator& it ) :
	anodeVoltage_V( DeSerializeBuildIn( 53000., binary_data, it ) ),
	anodeCurrent_A( DeSerializeBuildIn( .2, binary_data, it ) ),
	anodeMaterial( (MATERIAL) DeSerializeBuildIn( ToUnderlying( MATERIAL::THUNGSTEN ), binary_data, it ) )
{}


tubeParameter::MATERIAL tubeParameter::getEnum( const string materialString ){
	for( auto& [matEnum, value] : tubeParameter::material ){
		if( materialString == value.first )
			return matEnum;
	}

	return THUNGSTEN;
}

size_t tubeParameter::Serialize( vector<char>& binary_data ) const{
	size_t num_bytes = 0;


	num_bytes += SerializeBuildIn( FILE_PREAMBLE, binary_data );
	num_bytes += SerializeBuildIn( anodeVoltage_V, binary_data );
	num_bytes += SerializeBuildIn( anodeCurrent_A, binary_data );
	num_bytes += SerializeBuildIn( ToUnderlying( anodeMaterial ), binary_data );

	return num_bytes;
}


/*
	tube implementation
*/

tube::tube( CoordinateSystem* const coordinate_system, const tubeParameter parameter_ ) :
	cSys( coordinate_system ),
	anodeVoltage_V( ForcePositive( parameter_.anodeVoltage_V )),
	anodeCurrent_A( ForcePositive( parameter_.anodeCurrent_A )),
	anodeAtomicNumber( ForcePositive( tubeParameter::material.at( parameter_.anodeMaterial ).second ) ),
	totalPower_W( k_1PerV * static_cast<double>( anodeAtomicNumber ) * anodeCurrent_A * pow( anodeVoltage_V, 2 ) ),
	maxRadiationEnergy_eV( anodeVoltage_V )
{

	// Frequencies
	vector<double> energies = CreateLinearSpace( al_filter_cut_off_energy_eV, maxRadiationEnergy_eV, numPointsInSpectrum);

	// Values
	vector<double> spectralPower( energies.size(), 0.);

	// Frequency to which the filter dominates spectral behavious
	double changeEnergy = energies.front() + ( energies.back() - energies.front()) / (1. - al_filter_gradiant_factor);

	// Fill value vector
	for (auto energyIt = energies.begin(); energyIt < energies.end(); energyIt++) {
		size_t curIdx = energyIt - energies.begin();	// Current index

		double bremsGradient = -1;											// Gradient of brems spectrum
		double filterGradient = al_filter_gradiant_factor * bremsGradient;		// Gradient of filter spectrum

		// Filter dominates
		if ( *energyIt < changeEnergy ) {
			spectralPower.at(curIdx) = ( *energyIt - energies.front() ) * filterGradient;
			continue;
		}

		// Bremsspectrum dominates
		spectralPower.at(curIdx) = ( energies.back() - *energyIt ) * ( -bremsGradient );
	}


	// Calculate correction factor for spectrum for its values to sum up to totalPower
	double currentSum = Sum(spectralPower);
	double correctionFactor = totalPower_W / currentSum;

	// Correct values for sums to match
	Scale(spectralPower, correctionFactor);

	// Write frequency and power values to spectrum
	xRay_spectrum = spectrum( energies, spectralPower );

}

vector<ray> tube::getBeam( const vector<pixel> detectorPixel, const double detectorFocusDistance, size_t raysPerPixel, const double exposureTime ) const{

	// Force minimum of one
	raysPerPixel = ForceToMin1( raysPerPixel );

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

		const Point3D pMin = currentPixel.getPnt( currentPixel.AMin(), 0);		// Point on "minimum" edge
		const Point3D pMax = currentPixel.getPnt( currentPixel.AMax(), 0 );	// Point on "maximum" edge
		const line connectionLine{ pMax - pMin, pMin };						// Line connection the edge points

		const double edgeDistance = ( pMax - pMin ).length();								// Distance between edge points
		const double rayOriginDistanceDelta = edgeDistance / (double) ( raysPerPixel + 1 );	// Offset of ray origins on pixel

		// Iterate all rays hitting current pixel
		for( size_t currentRayIndex = 0; currentRayIndex < raysPerPixel; currentRayIndex++ ){
			
			// Offset of current ray origin_
			const double currentOffset = (double) ( currentRayIndex + 1 ) * rayOriginDistanceDelta;

			// Current ray origin_
			const Point3D currentOrigin = connectionLine.getPnt( currentOffset );

			// Tempory line pointing from pixel to tube
			const line tempLine{ currentPixel.Normal().ConvertTo( cSys ), currentOrigin.ConvertTo( cSys ) };

			// Origin of ray with specific distance to pixel
			const Point3D rayOrigin = tempLine.getPnt( detectorFocusDistance );

			// Add ray in tube's coordinate system to vector
			rays.emplace_back( -tempLine.R(), rayOrigin, beamProperties);

		}

	}

	return rays;
}

VectorPair tube::spectrumPoints( const bool integral ) const{

	VectorPair points;
	const vector<Tuple2D> spectrumPoints = xRay_spectrum.rawData();

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