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
#include "tomography.h"
#include "test_model.h"
#include "model.h"
#include "cSysTree.h"
#include "test_device.h"


/*********************************************************************
   Implemnetations
*********************************************************************/

bool test_radonTransform( void ){

	range distanceRange{ -25., 25 };
	double distanceResolution = 4;

	detectorRadonParameter radonParameter{
											idx2CR{ 64, 32 },
											500
	};

	radonTransformed test_Sinogram{ radonParameter };

	ofstream ax1 = openAxis( path( "./test_radonTransform.txt" ), true );

	addSingleObject( ax1, "Sinogram", test_Sinogram.Data(), "Angle;Distance;Energy;Dots", false );

	closeAxis( ax1 );

	return true;
}

bool test_Tomography( void ){

	gantry testGantry = getTestGantry();
	model mod = getTestModel( GLOBAL_CSYS() );
	
	tomography testTomography{ testGantry, mod };

	ofstream ax2 = openAxis( path( "./test_Tomography_gantry.txt" ), true );

	addObject( ax2, "Gantry", testGantry, "r", 0 );
	addObject( ax2, "TestModel", mod, "g", 0.015 );

	closeAxis( ax2 );

	//radonTransformed sinogram = testTomography.recordSlice();





	ofstream ax1 = openAxis( path( "./test_Tomography.txt" ), true );

	//addSingleObject( ax1, "Sinogram", sinogram.Data(), "Angle;Distance;Energy;Dots", false );

	closeAxis( ax1 );

	return true;
}