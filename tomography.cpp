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


/*********************************************************************
   Implemnetations
*********************************************************************/


const string TomographyProperties::FILE_PREAMBLE{ "TOMO_PARAMETER_FILE_PREAMBLE_Ver05" };

TomographyProperties::TomographyProperties( void ) :
	scattering_enabled( true ),
	max_scattering_occurrences( default_max_radiation_loops ),
	scatter_propability_correction( default_scatter_propability_correction ),
	use_simple_absorption( true ),
	scattered_ray_absorption_factor( 0.25 ),
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



optional<Projections> Tomography::RecordSlice( const ProjectionsProperties radon_properties, Gantry gantry, const Model& Model, const double zPosition, Fl_Progress_Window* progressWindow ){

	// Reset gantry to its initial position
	gantry.ResetGantry();

	// Translate Gantry
	if( zPosition != 0. )
		gantry.TranslateInZDirection( zPosition );

	// Assign gantry csys-data to radon coordinate system
	this->radon_coordinate_system_->CopyPrimitiveFrom( gantry.coordinate_system() );
	//this->radon_coordinate_system_->Rotate( this->radon_coordinate_system_->GetEz(), radon_properties.angles_resolution() / 2. );

	// Create sinogram 
	Projections sinogram{ radon_properties };

	// Radiate the model for each frame
	for( size_t currentFrame = 0; currentFrame < radon_properties.number_of_frames_to_fill(); currentFrame++ ){
		
		if( progressWindow != nullptr ) 
			progressWindow->ChangeLineText( 0, "Radiating frame " + ToString( currentFrame ) + " of " + ToString( radon_properties.number_of_frames_to_fill() ) );

		// Radiate
		gantry.RadiateModel( Model, properties_ );

		// Get the detection result
		const vector<DetectorPixel> detectionPixel = gantry.pixel_array();


		// Iterate all pixel
		for( const DetectorPixel& currentPixel : detectionPixel ){

			// Get Coordinates for pixel
			const RadonCoordinates newRadonCoordinates{ this->radon_coordinate_system_, currentPixel.NormalLine() };

			optional<double> line_integral = currentPixel.GetDetectedLineIntegral( properties_.use_simple_absorption, gantry.tube().number_of_rays_per_pixel(), gantry.tube().GetEmittedBeamPower() / ( static_cast<double>( detectionPixel.size() ) * static_cast<double>( gantry.tube().number_of_rays_per_pixel() ) ) );
			
			// If no value no ray was detected by pixel: line_integral would be infinite.
			// Set it to a high value
			if( !line_integral.has_value() ){
				line_integral = 25.; // Is like ray's energy is 1 / 100000000000 of its start energy 
			}
			// Get the radon point
			const RadonPoint newRadonPoint{ newRadonCoordinates, line_integral.value() };

			// Assign the data to sinogram
			sinogram.AssignData( newRadonPoint );
		}
		
		// Rotate gantry
		gantry.RotateCounterClockwise( radon_properties.angles_resolution() );

		Fl::check();

		if( !progressWindow->visible() ){
			return {};
		}
	}

	return sinogram;
}