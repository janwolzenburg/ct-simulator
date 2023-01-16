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



radonTransformed tomography::recordSlice( void ){

	gantry.reset();
	// TODO: Translate in z direction

	this->radonCSys->setPrimitive( gantry.CSys()->getPrimitive() );

	detectorRadonParameter radonParameter = gantry.getDetectorParameter( this->radonCSys );

	radonTransformed sinogram{ radonParameter };



	for( size_t currentFrame = 0; currentFrame < radonParameter.framesToFillSinogram; currentFrame++ ){

		gantry.radiate( model );
		vector<pixel> detectionPixel = gantry.getPixel();

		// TODO: Rotate gantry


		// Iterate all pixel
		for( pixel currentPixel : detectionPixel ){

			radonCoords newRadonCoordinates{ this->radonCSys, currentPixel.NormalLine() };

			radonPoint newRadonPoint{ newRadonCoordinates, currentPixel.getSinogramValue() };
			sinogram.assignData( newRadonPoint );

		}

		gantry.rotateCounterClockwise( radonParameter.deltaTheta );
	}



	return sinogram;
}