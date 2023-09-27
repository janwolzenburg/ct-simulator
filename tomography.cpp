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
#include "generel.h"
#include "cSysTree.h"
#include "plotting.h"



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
{

}

tomographyParameter::tomographyParameter( const vector<char>& binData, vector<char>::const_iterator& it ) :
	exposureTime( deSerializeBuildIn<double>( 1., binData, it ) ),
	scattering( deSerializeBuildIn<bool>(true, binData, it) ),
	maxRadiationLoops( deSerializeBuildIn<size_t>( maxRadiationLoops_Def, binData, it ) ),
	scatterPropability( deSerializeBuildIn<double>( completeModelScatterPropability_Def, binData, it ) ),
	rayStepSize( deSerializeBuildIn<double>( rayStepSize_Def, binData, it ) )
{
}

size_t tomographyParameter::serialize( vector<char>& binData ) const{

	size_t numBytes = 0;
	numBytes += serializeBuildIn( FILE_PREAMBLE, binData );
	numBytes += serializeBuildIn( exposureTime, binData );
	numBytes += serializeBuildIn( scattering, binData );
	numBytes += serializeBuildIn( maxRadiationLoops, binData );
	numBytes += serializeBuildIn( scatterPropability, binData );
	numBytes += serializeBuildIn( rayStepSize, binData );

	return numBytes;

}


tomography::tomography( void ) :
	parameter(),
	radonCSys( DUMMY_CSYS() )
{}


tomography::tomography( const tomographyParameter parameter_ ) :
	parameter( parameter_ ),
	radonCSys( DUMMY_CSYS() )
{}



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
			progressWindow->changeLineText( 0, "Radiating frame " + toString( currentFrame ) + " of " + toString( radonParameter.framesToFillSinogram ) );

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
		Gantry.rotateCounterClockwise( radonParameter.resolution.col );

		Fl::check();

	}

	return sinogram;
}