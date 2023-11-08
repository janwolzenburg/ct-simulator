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


const string TomographyProperties::FILE_PREAMBLE{ "TOMO_PARAMETER_FILE_PREAMBLE_Ver01" };

TomographyProperties::TomographyProperties( void ) :
	exposure_time( 1. ),
	scattering_enabled( true ),
	max_scattering_occurrences( default_max_radiation_loops ),
	scatter_propability_correction( default_scatter_propability_correction ),
	ray_step_length( default_ray_step_size_mm )

{}

TomographyProperties::TomographyProperties( const double exposureTime_, const bool scattering_, const size_t maxRadiationLoops_, const double scatterPropability_, const double rayStepSize_ ) :
	exposure_time( exposureTime_ ),
	scattering_enabled( scattering_ ),
	max_scattering_occurrences( maxRadiationLoops_ ),
	scatter_propability_correction( scatterPropability_ ),
	ray_step_length( rayStepSize_ )
{}

TomographyProperties::TomographyProperties( const vector<char>& binary_data, vector<char>::const_iterator& it ) :
	exposure_time( DeSerializeBuildIn<double>( 1., binary_data, it ) ),
	scattering_enabled( DeSerializeBuildIn<bool>(true, binary_data, it) ),
	max_scattering_occurrences( DeSerializeBuildIn<size_t>( default_max_radiation_loops, binary_data, it ) ),
	scatter_propability_correction( DeSerializeBuildIn<double>( default_scatter_propability_correction, binary_data, it ) ),
	ray_step_length( DeSerializeBuildIn<double>( default_ray_step_size_mm, binary_data, it ) )
{
}

size_t TomographyProperties::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;
	num_bytes += SerializeBuildIn( FILE_PREAMBLE, binary_data );
	num_bytes += SerializeBuildIn( exposure_time, binary_data );
	num_bytes += SerializeBuildIn( scattering_enabled, binary_data );
	num_bytes += SerializeBuildIn( max_scattering_occurrences, binary_data );
	num_bytes += SerializeBuildIn( scatter_propability_correction, binary_data );
	num_bytes += SerializeBuildIn( ray_step_length, binary_data );

	return num_bytes;

}



Projections Tomography::RecordSlice( const ProjectionsProperties radon_properties, Gantry gantry, const Model& Model, const double zPosition, Fl_Progress_Window* progressWindow ){

	// Reset gantry to its initial position
	gantry.ResetGantry();
	
	// Translate Gantry
	if( zPosition != 0. )
		gantry.TranslateInZDirection( zPosition );

	// Assign gantry csys-data to radon coordinate system
	this->radon_coordinate_system_->CopyPrimitiveFrom( gantry.coordinate_system() );

	// Create sinogram 
	Projections sinogram{ radon_properties };

	// Radiate the model for each frame
	for( size_t currentFrame = 0; currentFrame < radon_properties.number_of_frames_to_fill(); currentFrame++ ){
		
		if( progressWindow != nullptr ) 
			progressWindow->changeLineText( 0, "Radiating frame " + ToString( currentFrame ) + " of " + ToString( radon_properties.number_of_frames_to_fill() ) );

		// Radiate
		gantry.RadiateModel( Model, properties_ );

		// Get the detection result
		const vector<DetectorPixel> detectionPixel = gantry.pixel_array();


		size_t current_ray_index = 0;

		// Iterate all pixel
		for( size_t pixel_index = 0; pixel_index < gantry.pixel_array().size(); pixel_index++ ){

			// Get Coordinates for pixel
			const RadonCoordinates newRadonCoordinates{ this->radon_coordinate_system_, detectionPixel.at( pixel_index ).NormalLine() };

			// Get line integral from detected rays
			double line_integral = detectionPixel.at( pixel_index ).GetDetectedLineIntegral();

			// Negative return value means no detected rays
			if( line_integral < 0 ){
				// Radiate only one ray to fill radon space with data
				gantry.RadiateSingleRayWithoutScattering( Model, properties_, current_ray_index );
				line_integral = gantry.pixel_array().at( pixel_index ).GetDetectedLineIntegral(); // Call to this method necessary becaus pixel changed
			}

			// Get the radon point
			const RadonPoint newRadonPoint{ newRadonCoordinates, line_integral };
			
			// Assign the data to sinogram
			sinogram.AssignData( newRadonPoint );

			current_ray_index += gantry.tube().number_of_rays_per_pixel();
		}
		
		// Rotate gantry
		gantry.RotateCounterClockwise( radon_properties.angles_resolution() );

		Fl::check();

	}

	return sinogram;
}