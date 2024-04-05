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
	XRayTubeProperties
*/

const string XRayTubeProperties::FILE_PREAMBLE{ "Ver01TUBEPARAMETER_FILE_PREAMBLE" };


const std::map < XRayTubeProperties::Material, std::pair<string, size_t>> XRayTubeProperties::materials{
		{ Copper,		std::make_pair( "Copper", 29 ) },
		{ Molybdenum,	std::make_pair( "Molybdenum", 42 ) },
		{ Thungsten,	std::make_pair( "Thungsten", 74 ) }
};

XRayTubeProperties::XRayTubeProperties( const vector<char>& binary_data, vector<char>::const_iterator& current_byte ) :
	anode_voltage_V( DeSerializeBuildIn<double>( 120000., binary_data, current_byte ) ),
	anode_current_A( DeSerializeBuildIn<double>( .2, binary_data, current_byte ) ),
	anode_material( (Material) DeSerializeBuildIn<>( ConvertToUnderlying( Material::Thungsten ), binary_data, current_byte ) ),
	number_of_rays_per_pixel_( DeSerializeBuildIn<size_t>( 1, binary_data, current_byte ) ),
	has_filter_( DeSerializeBuildIn<bool>( true, binary_data, current_byte ) ),
	filter_cut_of_energy( DeSerializeBuildIn<double>( al_filter_cut_off_energy_eV, binary_data, current_byte ) ),
	filter_gradient( DeSerializeBuildIn<double>( 10., binary_data, current_byte ) ),
	spectral_energy_resolution( ( anode_voltage_V - minimum_energy_in_tube_spectrum ) / static_cast<double>( simulation_properties.number_of_points_in_spectrum - 1 ) )
{}


XRayTubeProperties::Material XRayTubeProperties::GetMaterialEnum( const string materialString ){
	for( auto& [matEnum, value] : XRayTubeProperties::materials ){
		if( materialString == value.first )
			return matEnum;
	}

	return Thungsten;
}

size_t XRayTubeProperties::Serialize( vector<char>& binary_data ) const{
	size_t number_of_bytes = 0;


	number_of_bytes += SerializeBuildIn<double>( anode_voltage_V, binary_data );
	number_of_bytes += SerializeBuildIn<double>( anode_current_A, binary_data );
	number_of_bytes += SerializeBuildIn<typename std::underlying_type_t<XRayTubeProperties::Material>>( ConvertToUnderlying( anode_material ), binary_data );
	number_of_bytes += SerializeBuildIn<size_t>( number_of_rays_per_pixel_, binary_data );
	number_of_bytes += SerializeBuildIn<bool>( has_filter_, binary_data );
	number_of_bytes += SerializeBuildIn<double>( filter_cut_of_energy, binary_data );
	number_of_bytes += SerializeBuildIn<double>( filter_gradient, binary_data );

	return number_of_bytes;
}


/*
	XRayTube implementation
*/

XRayTube::XRayTube( CoordinateSystem* const coordinate_system, 
										const XRayTubeProperties tube_properties ) :
	coordinate_system_( coordinate_system ),
	properties_( tube_properties ),
	anode_material_atomic_number_( ForcePositive( 
		XRayTubeProperties::materials.at( properties_.anode_material ).second ) ),
	radiation_power_W_( 
		efficiancy_constant_PerV * static_cast<double>( anode_material_atomic_number_ ) * 
		properties_.anode_current_A * pow( properties_.anode_voltage_V, 2 ) ),
	max_photon_energy_eV_( ForceToMax( 
		properties_.anode_voltage_V, maximum_energy_in_tube_spectrum ) )
{

	// prepare empty spectrum. first vector holds the energies, 
	// second holds the photonflow in an arbitrary unit
	VectorPair energy_spectrum{ CreateLinearSpace( minimum_energy_in_tube_spectrum, 
							max_photon_energy_eV_, 
							simulation_properties.number_of_points_in_spectrum ), 
							vector<double>( simulation_properties.number_of_points_in_spectrum, 0. ) };
	
	const double energy_resolution = energy_spectrum.first.at( 1 ) - 
																	 energy_spectrum.first.at( 0 );

	// fill value vector with values
	for( auto energy_it = energy_spectrum.first.begin(); 
						energy_it < energy_spectrum.first.end(); energy_it++) {
		
		// current index
		size_t energy_index = energy_it - energy_spectrum.first.begin();
		energy_spectrum.second.at(energy_index) =
			(energy_spectrum.first.back() - *energy_it + 2 * energy_resolution);
	}

	double complete_power = 0.;
	for( size_t energy_index = 0; energy_index < energy_spectrum.first.size(); 
							energy_index++ ){
		// cumulate power
		complete_power += energy_spectrum.first.at( energy_index ) * 
											energy_spectrum.second.at( energy_index );
	}
	
	// only the shape of the spectrum is correct
	// this is the factor to scale the spectrum by
	const double correction_factor = radiation_power_W_ / complete_power / J_Per_eV;
	Scale( energy_spectrum.second, correction_factor );		// scale by factor

	// apply hardening filter
	if( properties_.has_filter_ ){
		// energy to which the filter dominates spectral behavious
		const double change_energy = 
			properties_.filter_cut_of_energy + 
			 ( energy_spectrum.first.back() - properties_.filter_cut_of_energy ) / 
			 ( 1. +  properties_.filter_gradient );
			 
		// gradient of filter		 
		const double filter_gradient = properties_.filter_gradient * correction_factor;		

		// iterate energies and attentuate according to filter properties
		for( auto energy_it = energy_spectrum.first.begin(); 
							energy_it < energy_spectrum.first.end(); energy_it++ ) {
				 
			// current energy index
			const size_t energy_index = energy_it - energy_spectrum.first.begin();	
			
			// if filter dominates weaken the photonflow
			if ( *energy_it < change_energy ) {
				if( *energy_it < properties_.filter_cut_of_energy ){
					energy_spectrum.second.at( energy_index ) = 
						0.5 * energy_resolution * 
						( *energy_it + properties_.filter_cut_of_energy ) *
						( 1 / properties_.filter_cut_of_energy ) * filter_gradient;
				}
				else{
					energy_spectrum.second.at( energy_index ) = 
						( *energy_it - properties_.filter_cut_of_energy + energy_resolution) * 
						filter_gradient;
				}
			}
		}
	}

	// write energy and power values to spectrum
	emitted_spectrum_ = EnergySpectrum{ energy_spectrum };
	radiation_power_W_ = emitted_spectrum_.GetTotalPower();
}

void XRayTube::UpdateProperties( const XRayTubeProperties tube_properties ){
	*this = XRayTube{ coordinate_system_, tube_properties };
}

vector<Ray> XRayTube::GetEmittedBeam( const vector<DetectorPixel> detector_pixel, 
																			const double detector_focus_distance ) const{

	const size_t number_of_rays = properties_.number_of_rays_per_pixel_ * 
																detector_pixel.size();

	// split spectrum into the ray spectra
	const EnergySpectrum single_ray_spectrum = 
		emitted_spectrum_.GetEvenlyScaled( 1. / static_cast<double>( number_of_rays ) );

	// vector with rays
	vector<Ray> rays;

	size_t pixel_index = 0;

	// iterate all pixel
	for( const DetectorPixel& current_pixel : detector_pixel ){
		
		// properties of created rays for this pixel
		const RayProperties ray_properties{ single_ray_spectrum, pixel_index++, true };

		// get points on the edge of pixel, the line between them and their distance
		const Point3D edge_point_1 = 
			current_pixel.GetPoint( current_pixel.parameter_1_min(), 0);
		const Point3D edge_point_2 = 
			current_pixel.GetPoint( current_pixel.parameter_1_max(), 0 );
		const Line connection_line{ edge_point_2 - edge_point_1, edge_point_1 };
		const double edge_distance = ( edge_point_2 - edge_point_1 ).length();

		// distance on the pixel between rays which hit the pixel
		const double distance_delta = 
			edge_distance / static_cast<double>( properties_.number_of_rays_per_pixel_ + 1 );

		// iterate all rays hitting current pixel
		for( size_t ray_index = 0; ray_index < properties_.number_of_rays_per_pixel_; 
								ray_index++ ){
			
			// offset of current ray origin
			const double offset = static_cast<double>( ray_index + 1 ) * distance_delta;

			// current ray origin
			const Point3D ray_origin_on_pixel = connection_line.GetPoint( offset );

			// tempory Line pointing from pixel to tube
			const Line line_to_tube{ 
				current_pixel.GetNormal().ConvertTo( coordinate_system_ ), 
				ray_origin_on_pixel.ConvertTo( coordinate_system_ ) };

			// origin of ray with specific distance to pixel
			const Point3D ray_origin = line_to_tube.GetPoint( detector_focus_distance );

			// add ray in tube's coordinate system to vector
			rays.emplace_back( -line_to_tube.direction(), ray_origin, ray_properties );

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