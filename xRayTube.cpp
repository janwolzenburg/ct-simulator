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
#include "xRayTube.h"
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
	anode_voltage_V( DeSerializeBuildIn<double>( 120000., binary_data, it ) ),
	anode_current_A( DeSerializeBuildIn<double>( .2, binary_data, it ) ),
	anode_material( (Material) DeSerializeBuildIn<>( ToUnderlying( Material::Thungsten ), binary_data, it ) ),
	number_of_rays_per_pixel_( DeSerializeBuildIn<size_t>( 1, binary_data, it ) ),
	has_filter_( DeSerializeBuildIn<bool>( true, binary_data, it ) )
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


	num_bytes += SerializeBuildIn<double>( anode_voltage_V, binary_data );
	num_bytes += SerializeBuildIn<double>( anode_current_A, binary_data );
	num_bytes += SerializeBuildIn<typename std::underlying_type_t<XRayTubeProperties::Material>>( ToUnderlying( anode_material ), binary_data );
	num_bytes += SerializeBuildIn<size_t>( number_of_rays_per_pixel_, binary_data );
	num_bytes += SerializeBuildIn<bool>( has_filter_, binary_data );

	return num_bytes;
}


/*
	tube implementation
*/

XRayTube::XRayTube( CoordinateSystem* const coordinate_system, const XRayTubeProperties tube_properties ) :
	coordinate_system_( coordinate_system ),
	properties_( tube_properties ),
	anode_material_atomic_number_( ForcePositive( XRayTubeProperties::materials.at( properties_.anode_material ).second ) ),
	radiation_power_W_( efficiancy_constant_PerV * static_cast<double>( anode_material_atomic_number_ ) * properties_.anode_current_A * pow( properties_.anode_voltage_V, 2 ) ),
	max_photon_energy_eV_( properties_.anode_voltage_V )
{

	// 
	VectorPair energy_spectrum{ CreateLinearSpace( 1000., max_photon_energy_eV_, number_of_points_in_spectrum_), 
								vector<double>( number_of_points_in_spectrum_, 0. ) };


	
	const double bremsGradient = -1;		

	// Fill value vector
	for (auto energyIt = energy_spectrum.first.begin(); energyIt < energy_spectrum.first.end(); energyIt++) {
		size_t curIdx = energyIt - energy_spectrum.first.begin();	// Current index
									// Gradient of brems spectrum
		
		// Bremsspectrum dominates
		energy_spectrum.second.at(curIdx) = ( energy_spectrum.first.back() - *energyIt ) * ( -bremsGradient );
	}


	// Calculate correction factor for spectrum for its values to sum up to totalPower
	double currentSum = Sum( energy_spectrum.second );
	double correctionFactor = radiation_power_W_ / currentSum;

	// Correct values for sums to match
	Scale( energy_spectrum.second, correctionFactor );

	if( properties_.has_filter_ ){
		// Frequency to which the filter dominates spectral behavious
		double changeEnergy = al_filter_cut_off_energy_eV +  ( energy_spectrum.first.back() - al_filter_cut_off_energy_eV ) / ( 1. -  al_filter_gradient_factor );

		for ( auto energyIt = energy_spectrum.first.begin(); energyIt < energy_spectrum.first.end(); energyIt++ ) {
			
			const size_t curIdx = energyIt - energy_spectrum.first.begin();	// Current index
			
			if( *energyIt <= al_filter_cut_off_energy_eV ){
				energy_spectrum.second.at( curIdx ) = 0.;
				continue;
			}

			const double filterGradient = al_filter_gradient_factor * bremsGradient * correctionFactor;		// Gradient of filter spectrum

			// Filter dominates
			if ( *energyIt < changeEnergy ) {
				energy_spectrum.second.at( curIdx ) = ( *energyIt - al_filter_cut_off_energy_eV ) * filterGradient;
			}
		}
	}

	// Write frequency and power values to spectrum
	emitted_spectrum_ = EnergySpectrum{ energy_spectrum };

}

void XRayTube::UpdateProperties( const XRayTubeProperties tube_properties ){
	*this = XRayTube{ coordinate_system_, tube_properties };
}

vector<Ray> XRayTube::GetEmittedBeam( const vector<DetectorPixel> detectorPixel, const double detector_focus_distance ) const{

	const size_t numRays = properties_.number_of_rays_per_pixel_ * detectorPixel.size();

	// Split spectrum into the Ray spectra
	const EnergySpectrum raySpectrum = emitted_spectrum_.GetScaled( 1. / static_cast<double>( numRays ) );

	// Vector with rays
	vector<Ray> rays;

	size_t pixel_index = 0;

	// Iterate all pixel
	for( const DetectorPixel currentPixel : detectorPixel ){
		
		// Properties of created rays for this pixel
		const RayProperties beamProperties{ raySpectrum, pixel_index++ };

		// Get points on the edge of pixel

		const Point3D pMin = currentPixel.GetPoint( currentPixel.parameter_1_min(), 0);		// Point on "minimum" edge
		const Point3D pMax = currentPixel.GetPoint( currentPixel.parameter_1_max(), 0 );	// Point on "maximum" edge
		const Line connectionLine{ pMax - pMin, pMin };						// Line connection the edge points

		const double edgeDistance = ( pMax - pMin ).length();								// Distance between edge points
		const double rayOriginDistanceDelta = edgeDistance / static_cast<double>( properties_.number_of_rays_per_pixel_ + 1 );	// Offset of Ray origins on pixel

		// Iterate all rays hitting current pixel
		for( size_t currentRayIndex = 0; currentRayIndex < properties_.number_of_rays_per_pixel_; currentRayIndex++ ){
			
			// Offset of current Ray origin_
			const double currentOffset = static_cast<double>( currentRayIndex + 1 ) * rayOriginDistanceDelta;

			// Current Ray origin_
			const Point3D currentOrigin = connectionLine.GetPoint( currentOffset );

			// Tempory Line pointing from pixel to tube
			const Line tempLine{ currentPixel.GetNormal().ConvertTo( coordinate_system_ ), currentOrigin.ConvertTo( coordinate_system_ ) };

			// Origin of Ray with specific distance to pixel
			const Point3D rayOrigin = tempLine.GetPoint( detector_focus_distance );

			// Add Ray in tube's coordinate system to vector
			rays.emplace_back( -tempLine.direction(), rayOrigin, beamProperties );

		}

	}

	return rays;
}

VectorPair XRayTube::GetEnergySpectrumPoints( void ) const{

	VectorPair points;
	const vector<Tuple2D> spectrumPoints = emitted_spectrum_.data();

	for( auto& point : spectrumPoints ){
		points.first.push_back( point.x );
		points.second.push_back( point.y );
	}

	return points;
}