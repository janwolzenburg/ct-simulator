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

	detectorParameterPhysical detectorParas{ .columns = 31,
										.rowSize = 25,
										.colSize = 25,
										.structured = false };



	double measureField = 500;
	double detectorAngle = 2.*PI*( 50./360. );
	double gantryRadius = measureField/2/sin(  detectorAngle/2 );

	gantry testGantry{ GLOBAL_CSYS()->createCopy( "Gantry system" ),gantryRadius , 1.1 * detectorAngle, 40, tubeParas, detectorParas};
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