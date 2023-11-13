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
	scattering_enabled( true ),
	max_scattering_occurrences( default_max_radiation_loops ),
	scatter_propability_correction( default_scatter_propability_correction )

{}

TomographyProperties::TomographyProperties( const bool scattering_, const size_t maxRadiationLoops_, const double scatterPropability_ ) :
	scattering_enabled( scattering_ ),
	max_scattering_occurrences( maxRadiationLoops_ ),
	scatter_propability_correction( scatterPropability_ )
{}

TomographyProperties::TomographyProperties( const vector<char>& binary_data, vector<char>::const_iterator& it ) :
	scattering_enabled( DeSerializeBuildIn<bool>(true, binary_data, it) ),
	max_scattering_occurrences( DeSerializeBuildIn<size_t>( default_max_radiation_loops, binary_data, it ) ),
	scatter_propability_correction( DeSerializeBuildIn<double>( default_scatter_propability_correction, binary_data, it ) )
{
}

size_t TomographyProperties::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;
	num_bytes += SerializeBuildIn( scattering_enabled, binary_data );
	num_bytes += SerializeBuildIn( max_scattering_occurrences, binary_data );
	num_bytes += SerializeBuildIn( scatter_propability_correction, binary_data );

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
			progressWindow->ChangeLineText( 0, "Radiating frame " + ToString( currentFrame ) + " of " + ToString( radon_properties.number_of_frames_to_fill() ) );

		// Radiate
		gantry.RadiateModel( Model, properties_ );

		// Get the detection result
		const vector<DetectorPixel> detectionPixel = gantry.pixel_array();


		// Iterate all pixel
		for( const DetectorPixel& currentPixel : detectionPixel ){

			// Get Coordinates for pixel
			const RadonCoordinates newRadonCoordinates{ this->radon_coordinate_system_, currentPixel.NormalLine() };

			double line_integral = currentPixel.GetDetectedLineIntegral();
			
			// If negative no ray was detected by pixel: line_integral would be infinite.
			// Set it to a high value
			if( line_integral < 0 ){
				line_integral = 25.; // Is like ray's energy is 1 / 100000000000 of its start energy 
			}

			// Get the radon point
			const RadonPoint newRadonPoint{ newRadonCoordinates, line_integral };
			
			// Assign the data to sinogram
			sinogram.AssignData( newRadonPoint );
		}
		
		// Rotate gantry
		gantry.RotateCounterClockwise( radon_properties.angles_resolution() );

		Fl::check();

	}

	return sinogram;
}