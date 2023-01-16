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

	radonTransformed sinogram{ gantry.getDetectorParameter( this->radonCSys ) };

	gantry.radiate( model );
	vector<pixel> detectionPixel = gantry.getPixel();

	// TODO: get sinogram from radiation result
	

}