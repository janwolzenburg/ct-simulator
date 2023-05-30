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
#include "tomography.h"
#include "generel.h"
#include "cSysTree.h"
#include "plotting.h"

/*********************************************************************
   Implemnetations
*********************************************************************/


const string tomographyParameter::FILE_PREAMBLE{ "TOMO_PARAMETER_FILE_PREAMBLE" };

tomographyParameter::tomographyParameter( void ) :
	exposureTime( 1. ),
	scattering( true )
{}

tomographyParameter::tomographyParameter( const double exposureTime_, const bool scattering_ ) :
	exposureTime( exposureTime_ ),
	scattering( scattering_ )
{

}

tomographyParameter::tomographyParameter( const vector<char>& binData, vector<char>::const_iterator& it ) :
	exposureTime( deSerializeBuildIn<double>( 1., binData, it ) ),
	scattering( deSerializeBuildIn<bool>(true, binData, it) )
{
}

size_t tomographyParameter::serialize( vector<char>& binData ) const{

	size_t numBytes = 0;
	numBytes += serializeBuildIn( FILE_PREAMBLE, binData );
	numBytes += serializeBuildIn( exposureTime, binData );
	numBytes += serializeBuildIn( scattering, binData );

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



radonTransformed tomography::recordSlice( gantry Gantry, const model& Model, const double zPosition ){



	// Reset gantry to its initial position
	Gantry.reset();

	//int curRot = 0;
	//const int numRot = 3;
	//ofstream ax1 = openAxis( path( "./test_gantry.txt" ), true );
	

	

	// Translate Gantry
	if( zPosition != 0. )
		Gantry.CSys()->translateM( Gantry.CSys()->EzVec() * zPosition );

	// Assign gantry csys-data to radon coordinate system
	this->radonCSys->copyFrom( Gantry.CSys() );


	// Get the radon paramters for the detector
	detectorRadonParameter radonParameter = Gantry.getDetectorParameter( );

	// Create sinogram 
	radonTransformed sinogram{ radonParameter };


	// Radiate the model for each frame
	for( size_t currentFrame = 0; currentFrame < radonParameter.framesToFillSinogram; currentFrame++ ){
		
		// Radiate
		Gantry.radiate( Model, parameter.ExposureTime(), parameter.Scattering() );
		
		// Get the detection result
		vector<pixel> detectionPixel = Gantry.getPixel();


		// Iterate all pixel
		for( pixel currentPixel : detectionPixel ){

			// Get coordinates for pixel
			radonCoords newRadonCoordinates{ this->radonCSys, currentPixel.NormalLine() };

			// Get the radon point
			radonPoint newRadonPoint{ newRadonCoordinates, currentPixel.getRadonValue() };
			
			// Assign the data to sinogram
			sinogram.assignData( newRadonPoint );
		}

		//if( curRot++ < numRot ){
		//	
		//	string col = "r";

		//	switch( curRot ){
		//		case 1:
		//			col = "r"; break;
		//		case 2:
		//			col = "g"; break;
		//		case 3:
		//			col = "b"; break;
		//		default:
		//			col = "y"; break;
		//	}

		//	addObject( ax1, "Gantry", Gantry, col, GANTRY_SPECIFIERS::ORIGIN | GANTRY_SPECIFIERS::BEAMS | GANTRY_SPECIFIERS::DETECTOR_SURFACES );
		//}
		
		// Rotate gantry
		Gantry.rotateCounterClockwise( radonParameter.resolution.col );
	}

	//closeAxis( ax1 );

	return sinogram;
}