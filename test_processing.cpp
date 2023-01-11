#pragma once
/*********************************************************************
 * @file   test_processing.cpp
 * @brief  Tests Implementation
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/



 /*********************************************************************
  Includes
*********************************************************************/
#include "test_processing.h"
#include "radonTransform.h"


/*********************************************************************
   Implemnetations
*********************************************************************/

bool test_radonTransform( void ){

	range distanceRange{ -25., 25 };
	double distanceResolution = 4;

	radonTransformed test_Sinogram{ distanceRange, v2RC{ PI / 26, distanceResolution }};


}