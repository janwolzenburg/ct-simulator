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

const string XRayTubeProperties::FILE_PREAMBLE{ "TUBEPARAMETER_FILE_PREAMBLE" };


const std::map < XRayTubeProperties::Material, std::pair<string, size_t>> XRayTubeProperties::materials{
		{ Copper,		std::make_pair( "Copper", 29 ) },
		{ Molybdenum,	std::make_pair( "Molybdenum", 42 ) },
		{ Thungsten,	std::make_pair( "Thungsten", 74 ) }
};

XRayTubeProperties::XRayTubeProperties( const vector<char>& binary_data, vector<char>::const_iterator& it ) :
	anode_voltage_V( DeSerializeBuildIn( 53000., binary_data, it ) ),
	anode_current_A( DeSerializeBuildIn( .2, binary_data, it ) ),
	anode_material( (Material) DeSerializeBuildIn( ToUnderlying( Material::Thungsten ), binary_data, it ) )
{}


XRayTubeProperties::Material XRayTubeProperties::GetMaterialEnum( const string materialString ){
	for( auto& [matEnum, value] : XRayTubeProperties::materials ){
		if( materialString == value.first )
			return matEnum;
	}

	return Thungsten;
}

size_t XRayTubeProperties::Serialize( vector<char>& binary_data ) const{
	size_t num_bytes = 0;


	num_bytes += SerializeBuildIn( FILE_PREAMBLE, binary_data );
	num_bytes += SerializeBuildIn( anode_voltage_V, binary_data );
	num_bytes += SerializeBuildIn( anode_current_A, binary_data );
	num_bytes += SerializeBuildIn( ToUnderlying( anode_material ), binary_data );

	return num_bytes;
}


/*
	tube implementation
*/

XRayTube::XRayTube( CoordinateSystem* const coordinate_system, const XRayTubeProperties parameter_ ) :
	coordinate_system_( coordinate_system ),
	anode_voltage_V_( ForcePositive( parameter_.anode_voltage_V )),
	anode_current_A_( ForcePositive( parameter_.anode_current_A )),
	anode_material_atomic_number_( ForcePositive( XRayTubeProperties::materials.at( parameter_.anode_material ).second ) ),
	radiation_power_W_( k_1PerV * static_cast<double>( anode_material_atomic_number_ ) * anode_current_A_ * pow( anode_voltage_V_, 2 ) ),
	max_photon_energy_eV_( anode_voltage_V_ )
{

	// Frequencies
	vector<double> energies = CreateLinearSpace( al_filter_cut_off_energy_eV, max_photon_energy_eV_, number_of_points_in_spectrum_);

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
	double correctionFactor = radiation_power_W_ / currentSum;

	// Correct values for sums to match
	Scale(spectralPower, correctionFactor);

	// Write frequency and power values to spectrum
	emitted_spectrum_ = spectrum( energies, spectralPower );

}

vector<Ray> XRayTube::GetEmittedBeam( const vector<pixel> detectorPixel, const double detectorFocusDistance, size_t raysPerPixel, const double exposureTime ) const{

	// Force minimum of one
	raysPerPixel = ForceToMin1( raysPerPixel );

	const size_t numRays = raysPerPixel * detectorPixel.size();

	// Split spectrum into the Ray spectra. Multiply by exposure time in seconds to get energy spectra
	const spectrum raySpectrum = emitted_spectrum_.getScaled( exposureTime / (double) numRays );

	// properties of created rays
	const RayProperties beamProperties{ raySpectrum };


	// Vector with rays
	vector<Ray> rays;

	// Iterate all pixel
	for( const pixel currentPixel : detectorPixel ){
		
		// Get points on the edge of pixel

		const Point3D pMin = currentPixel.GetPoint( currentPixel.parameter_1_min(), 0);		// Point on "minimum" edge
		const Point3D pMax = currentPixel.GetPoint( currentPixel.parameter_1_max(), 0 );	// Point on "maximum" edge
		const Line connectionLine{ pMax - pMin, pMin };						// Line connection the edge points

		const double edgeDistance = ( pMax - pMin ).length();								// Distance between edge points
		const double rayOriginDistanceDelta = edgeDistance / (double) ( raysPerPixel + 1 );	// Offset of Ray origins on pixel

		// Iterate all rays hitting current pixel
		for( size_t currentRayIndex = 0; currentRayIndex < raysPerPixel; currentRayIndex++ ){
			
			// Offset of current Ray origin_
			const double currentOffset = (double) ( currentRayIndex + 1 ) * rayOriginDistanceDelta;

			// Current Ray origin_
			const Point3D currentOrigin = connectionLine.GetPoint( currentOffset );

			// Tempory Line pointing from pixel to tube
			const Line tempLine{ currentPixel.GetNormal().ConvertTo( coordinate_system_ ), currentOrigin.ConvertTo( coordinate_system_ ) };

			// Origin of Ray with specific distance to pixel
			const Point3D rayOrigin = tempLine.GetPoint( detectorFocusDistance );

			// Add Ray in tube's coordinate system to vector
			rays.emplace_back( -tempLine.direction(), rayOrigin, beamProperties);

		}

	}

	return rays;
}

VectorPair XRayTube::GetEnergySpectrumPoints( void ) const{

	VectorPair points;
	const vector<Tuple2D> spectrumPoints = emitted_spectrum_.rawData();

	for( auto& point : spectrumPoints ){
		points.first.push_back( point.x );
		points.second.push_back( point.y );
	}

	return points;
}