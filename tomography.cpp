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


const string tomographyParameter::FILE_PREAMBLE{ "TOMO_PARAMETER_FILE_PREAMBLE_Ver01" };

tomographyParameter::tomographyParameter( void ) :
	exposureTime( 1. ),
	scattering( true ),
	maxRadiationLoops( default_max_radiation_loops ),
	scatterPropability( default_complete_model_scatter_propability ),
	rayStepSize( default_ray_step_size_mm )

{}

tomographyParameter::tomographyParameter( const double exposureTime_, const bool scattering_, const size_t maxRadiationLoops_, const double scatterPropability_, const double rayStepSize_ ) :
	exposureTime( exposureTime_ ),
	scattering( scattering_ ),
	maxRadiationLoops( maxRadiationLoops_ ),
	scatterPropability( scatterPropability_ ),
	rayStepSize( rayStepSize_ )
{}

tomographyParameter::tomographyParameter( const vector<char>& binary_data, vector<char>::const_iterator& it ) :
	exposureTime( DeSerializeBuildIn<double>( 1., binary_data, it ) ),
	scattering( DeSerializeBuildIn<bool>(true, binary_data, it) ),
	maxRadiationLoops( DeSerializeBuildIn<size_t>( default_max_radiation_loops, binary_data, it ) ),
	scatterPropability( DeSerializeBuildIn<double>( default_complete_model_scatter_propability, binary_data, it ) ),
	rayStepSize( DeSerializeBuildIn<double>( default_ray_step_size_mm, binary_data, it ) )
{
}

size_t tomographyParameter::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;
	num_bytes += SerializeBuildIn( FILE_PREAMBLE, binary_data );
	num_bytes += SerializeBuildIn( exposureTime, binary_data );
	num_bytes += SerializeBuildIn( scattering, binary_data );
	num_bytes += SerializeBuildIn( maxRadiationLoops, binary_data );
	num_bytes += SerializeBuildIn( scatterPropability, binary_data );
	num_bytes += SerializeBuildIn( rayStepSize, binary_data );

	return num_bytes;

}



radonTransformed tomography::recordSlice( const radonProperties radon_properties, gantry Gantry, const Model& Model, const double zPosition, Fl_Progress_Window* progressWindow ){

	// Reset gantry to its initial position
	Gantry.ResetDetected();
	

	// Translate Gantry
	if( zPosition != 0. )
		Gantry.CSys()->Translate( Gantry.CSys()->GetEz() * zPosition );

	// Assign gantry csys-data_ to radon coordinate system
	this->radonCSys->CopyPrimitiveFrom( Gantry.CSys() );


	// Create sinogram 
	radonTransformed sinogram{ radon_properties };

	

	// Radiate the model for each frame
	for( size_t currentFrame = 0; currentFrame < radon_properties.framesToFillSinogram; currentFrame++ ){
		
		if( progressWindow != nullptr ) 
			progressWindow->changeLineText( 0, "Radiating frame " + ToString( currentFrame ) + " of " + ToString( radon_properties.framesToFillSinogram ) );

		// Radiate
		Gantry.radiate( Model, voxel_data_ );
		
		// Get the detection result
		const vector<DetectorPixel> detectionPixel = Gantry.getPixel();


		// Iterate all pixel
		for( const DetectorPixel& currentPixel : detectionPixel ){

			// Get Coordinates for pixel
			const radonCoords newRadonCoordinates{ this->radonCSys, currentPixel.NormalLine() };

			// Get the radon point
			const radonPoint newRadonPoint{ newRadonCoordinates, currentPixel.GetRadonValue() };
			
			// Assign the data_ to sinogram
			sinogram.assignData( newRadonPoint );
		}
		
		// Rotate gantry
		Gantry.rotateCounterClockwise( radon_properties.resolution.c );

		Fl::check();

	}

	return sinogram;
}