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
#include "plotting.h"


/*********************************************************************
   Implemnetations
*********************************************************************/

bool test_radonTransform( void ){

	range distanceRange{ -25., 25 };
	double distanceResolution = 4;

	detectorRadonParameter radonParameters{ PI / 26, distanceResolution, distanceRange, 2 };

	radonTransformed test_Sinogram{ radonParameters };

	ofstream ax1 = openAxis( path( "./test_radonTransform.txt" ), true );

	addSingleObject( ax1, "Sinogram", test_Sinogram.Data(), "Angle;Distance;Energy;Dots", false );

	closeAxis( ax1 );

	return true;
}