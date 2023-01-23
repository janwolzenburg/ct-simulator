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



/*********************************************************************
   Implemnetations
*********************************************************************/


tomography::tomography( const gantry gantry_, model& model_ ) :
	Gantry( gantry_ ),
	Model( model_ ),
	radonCSys( Gantry.CSys()->createCopy( "Radon System" ) )
{}


radonTransformed tomography::recordSlice( void ){

	// Reset gantry to its initial position
	Gantry.reset();
	// TODO: Translate in z direction

	// Set the radon coordinate system to current gantry postion
	this->radonCSys->setPrimitive( Gantry.CSys()->getPrimitive() );

	// Get the radon paramters for the detector
	detectorRadonParameter radonParameter = Gantry.getDetectorParameter( );

	// Create sinogram 
	radonTransformed sinogram{ radonParameter };

	//TODO: Add status output

	// Radiate the model for each frame
	for( size_t currentFrame = 0; currentFrame < radonParameter.framesToFillSinogram; currentFrame++ ){

		// Radiate
		Gantry.radiate( Model );
		
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

		// Rotate gantry
		Gantry.rotateCounterClockwise( radonParameter.resolution.col );
	}

	return sinogram;
}