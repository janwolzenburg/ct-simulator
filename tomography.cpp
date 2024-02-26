#pragma once
/*********************************************************************
 * @file   tomography.cpp
 * @brief  Implementation
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/



 /*********************************************************************
  Includes
*********************************************************************/

#include <FL/Fl.H>

#include "tomography.h"
#include "coordinateSystemTree.h"
#include "simulation.h"
#include "serialization.h"
#include "projections.h"


/*********************************************************************
   Implemnetations
*********************************************************************/


const string TomographyProperties::FILE_PREAMBLE{ "TOMO_PARAMETER_FILE_PREAMBLE_Ver05" };

TomographyProperties::TomographyProperties( void ) :
	scattering_enabled( true ),
	max_scattering_occurrences( default_max_radiation_loops ),
	scatter_propability_correction( default_scatter_propability_correction ),
	use_simple_absorption( true ),
	scattered_ray_absorption_factor( 1. ),
	mean_energy_of_tube( reference_energy_for_mu_eV ),
	name( "Unnamed" ),
	filter_active( false )

{}

TomographyProperties::TomographyProperties( const bool scattering_, const size_t maxRadiationLoops_, const double scatterPropability_, 
											const bool use_simple_absorption, const double scattered_ray_absorption_factor,
											const string identifiaction_name, const bool tube_filter_active ) :
	scattering_enabled( scattering_ ),
	max_scattering_occurrences( maxRadiationLoops_ ),
	scatter_propability_correction( scatterPropability_ ),
	use_simple_absorption( use_simple_absorption ),
	scattered_ray_absorption_factor( scattered_ray_absorption_factor ),
	mean_energy_of_tube( reference_energy_for_mu_eV ),
	name( identifiaction_name ),
	filter_active( tube_filter_active )
{}

TomographyProperties::TomographyProperties( const vector<char>& binary_data, vector<char>::const_iterator& it ) :
	scattering_enabled( DeSerializeBuildIn<bool>(true, binary_data, it) ),
	max_scattering_occurrences( DeSerializeBuildIn<size_t>( default_max_radiation_loops, binary_data, it ) ),
	scatter_propability_correction( DeSerializeBuildIn<double>( default_scatter_propability_correction, binary_data, it ) ),
	use_simple_absorption( DeSerializeBuildIn<bool>(true, binary_data, it) ),
	scattered_ray_absorption_factor( DeSerializeBuildIn<double>( default_scatter_propability_correction, binary_data, it ) ),
	mean_energy_of_tube( DeSerializeBuildIn<double>( reference_energy_for_mu_eV, binary_data, it ) ),
	name( DeSerializeBuildIn<string>( "Unnamed", binary_data, it ) ),
	filter_active( DeSerializeBuildIn<bool>(false, binary_data, it) )

{
}

size_t TomographyProperties::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;
	num_bytes += SerializeBuildIn<bool>( scattering_enabled, binary_data );
	num_bytes += SerializeBuildIn<size_t>( max_scattering_occurrences, binary_data );
	num_bytes += SerializeBuildIn<double>( scatter_propability_correction, binary_data );
	num_bytes += SerializeBuildIn<bool>( use_simple_absorption, binary_data );
	num_bytes += SerializeBuildIn<double>( scattered_ray_absorption_factor, binary_data );
	num_bytes += SerializeBuildIn<double>( mean_energy_of_tube, binary_data );
	num_bytes += SerializeBuildIn<string>( name, binary_data );
	num_bytes += SerializeBuildIn<bool>( filter_active, binary_data );


	return num_bytes;

}


optional<Projections> Tomography::RecordSlice( 
	const ProjectionsProperties projection_properties, Gantry gantry, 
	const Model& model, const double z_position, Fl_Progress_Window* progress_window ){

	// Reset gantry to its initial position
	gantry.ResetGantry();

	// Translate Gantry
	if( z_position != 0. )
		gantry.TranslateInZDirection( z_position );

	// Assign gantry coordinate-system's unit-vectors to radon coordinate system
	this->radon_coordinate_system_->CopyPrimitiveFrom( gantry.coordinate_system() );

	// Create projections 
	Projections projections{ projection_properties, properties_ };

	// Radiate the model for each frame
	for( size_t frame_index = 0; frame_index < projection_properties.number_of_frames_to_fill(); frame_index++ ){
		
		if( progress_window != nullptr ) 
			progress_window->ChangeLineText( 0, "Radiating frame " + 
				ToString( frame_index ) + " of " + 
				ToString( projection_properties.number_of_frames_to_fill() ) );

		// Radiate
		gantry.RadiateModel( model, properties_ );

		// Get the detection result
		const vector<DetectorPixel> pixel_array = std::move( gantry.pixel_array() );

		// Iterate all pixel
		for( const DetectorPixel& pixel : pixel_array ){

			// Get Coordinates for pixel
			const RadonCoordinates radon_coordinates{ this->radon_coordinate_system_, pixel.NormalLine() };

			optional<double> line_integral = pixel.GetDetectedLineIntegral( 
												properties_.use_simple_absorption, 
												gantry.tube().number_of_rays_per_pixel(), 
												gantry.tube().GetEmittedBeamPower() / 
												( static_cast<double>( pixel_array.size() ) * 
													static_cast<double>( gantry.tube().number_of_rays_per_pixel() ) 
												) );
			
			// If no value no ray was detected by pixel: line_integral would be infinite.
			// Set it to a high value
			if( !line_integral.has_value() ){
				line_integral = 25.; // Is like ray's energy is 1 / 100000000000 of its start energy 
			}

			// Get the radon point
			const RadonPoint radon_point{ radon_coordinates, line_integral.value() };

			// Assign the data to sinogram
			projections.AssignData( radon_point );
		}
		
		// Rotate gantry
		gantry.RotateCounterClockwise( projection_properties.angles_resolution() );

		Fl::check();

		if( progress_window != nullptr ){
			if( !progress_window->visible() )
				return {};
		}
	}

	return projections;
}