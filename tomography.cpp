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
#include "cSysTree.h"
#include "simulation.h"
#include "serialization.h"


/*********************************************************************
   Implemnetations
*********************************************************************/


const string tomographyParameter::FILE_PREAMBLE{ "TOMO_PARAMETER_FILE_PREAMBLE_Ver01" };

tomographyParameter::tomographyParameter( void ) :
	exposureTime( 1. ),
	scattering( true ),
	maxRadiationLoops( maxRadiationLoops_Def ),
	scatterPropability( completeModelScatterPropability_Def ),
	rayStepSize( rayStepSize_Def )

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
	maxRadiationLoops( DeSerializeBuildIn<size_t>( maxRadiationLoops_Def, binary_data, it ) ),
	scatterPropability( DeSerializeBuildIn<double>( completeModelScatterPropability_Def, binary_data, it ) ),
	rayStepSize( DeSerializeBuildIn<double>( rayStepSize_Def, binary_data, it ) )
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



radonTransformed tomography::recordSlice( gantry Gantry, const model& Model, const double zPosition, Fl_Progress_Window* progressWindow ){

	// Reset gantry to its initial position
	Gantry.reset();
	

	// Translate Gantry
	if( zPosition != 0. )
		Gantry.CSys()->translateM( Gantry.CSys()->EzVec() * zPosition );

	// Assign gantry csys-data to radon coordinate system
	this->radonCSys->copyFrom( Gantry.CSys() );


	// Get the radon paramters for the detector
	const detectorRadonParameter radonParameter = Gantry.getDetectorParameter( );

	// Create sinogram 
	radonTransformed sinogram{ radonParameter };

	

	// Radiate the model for each frame
	for( size_t currentFrame = 0; currentFrame < radonParameter.framesToFillSinogram; currentFrame++ ){
		
		if( progressWindow != nullptr ) 
			progressWindow->changeLineText( 0, "Radiating frame " + ToString( currentFrame ) + " of " + ToString( radonParameter.framesToFillSinogram ) );

		// Radiate
		Gantry.radiate( Model, parameter );
		
		// Get the detection result
		const vector<pixel> detectionPixel = Gantry.getPixel();


		// Iterate all pixel
		for( const pixel& currentPixel : detectionPixel ){

			// Get coordinates for pixel
			const radonCoords newRadonCoordinates{ this->radonCSys, currentPixel.NormalLine() };

			// Get the radon point
			const radonPoint newRadonPoint{ newRadonCoordinates, currentPixel.getRadonValue() };
			
			// Assign the data to sinogram
			sinogram.assignData( newRadonPoint );
		}
		
		// Rotate gantry
		Gantry.rotateCounterClockwise( radonParameter.resolution.c );

		Fl::check();

	}

	return sinogram;
}