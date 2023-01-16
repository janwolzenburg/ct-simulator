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

bool test_Tomography( void ){


	tubeParameter tubeParas{ .anodeVoltage_V = 53000,
							.anodeCurrent_A = 0.2,
							.anodeAtomicNumber = 74 };

	detectorParameterPhysical detectorParas{ .columns = 25,
										.rowSize = 10,
										.colSize = 20,
										.structured = false };



	gantry testGantry{ GLOBAL_CSYS()->createCopy( "Gantry system" ), 300., PI / 4., 60, tubeParas, detectorParas };
	model mod = getTestModel( GLOBAL_CSYS() );
	
	tomography testTomography{ testGantry, mod };

	radonTransformed sinogram = testTomography.recordSlice();

	ofstream ax1 = openAxis( path( "./test_Tomography.txt" ), true );

	addSingleObject( ax1, "Sinogram", sinogram.Data(), "Angle;Distance;Energy;Dots", false );

	closeAxis( ax1 );

	return true;
}