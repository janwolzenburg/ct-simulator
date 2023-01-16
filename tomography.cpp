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

	radonTransformed sinogram{ gantry.getDetectorParameter() };

	gantry.reset();


	gantry.radiate( model );
	vector<pixel> detectionPixel = gantry.getPixel();

	// TODO: get sinogram from radiation resul
	

}