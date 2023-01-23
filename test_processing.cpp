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

bool test_detector_to_sinogram( void ){

	gantry testGantry = getTestGantry( idx2CR{ 39, 17 } );
	detectorRadonParameter radonParameter = testGantry.getDetectorParameter();
	cartCSys* radonCSys = testGantry.CSys()->createCopy( "Radon System" );

	// Create sinogram 
	radonTransformed sinogram{ radonParameter };

	for( size_t currentFrame = 0; currentFrame < radonParameter.framesToFillSinogram; currentFrame++ ){
		// Get the detection result
		vector<pixel> detectionPixel = testGantry.getPixel();

		// Iterate all pixel
		for( pixel currentPixel : detectionPixel ){
			// Get coordinates for pixel
			radonCoords newRadonCoordinates{ radonCSys, currentPixel.NormalLine() };
			// Get the radon point
			radonPoint newRadonPoint{ newRadonCoordinates, 1 };
			// Assign the data to sinogram
			sinogram.assignData( newRadonPoint );
		}

		string filename = "./test_detector_to_sinogram_";
		filename += to_string(currentFrame) + ".txt";

		ofstream ax1 = openAxis( path( filename ), true );
		addSingleObject( ax1, "Sinogram", sinogram.Data(), "Angle;Distance;Energy;Dots", false );
		closeAxis( ax1 );


		// Rotate gantry
		testGantry.rotateCounterClockwise( radonParameter.resolution.col );
	}



	

	return true;
}


bool test_Tomography( void ){

	gantry testGantry = getTestGantry( idx2CR{ 63, 31 } );
	model mod = getTestModel( GLOBAL_CSYS() );
	
	tomography testTomography{ testGantry, mod };

	ofstream ax2 = openAxis( path( "./test_Tomography_gantry.txt" ), true );

	addObject( ax2, "Gantry", testGantry, "r", GANTRY_SPECIFIERS::ORIGIN | GANTRY_SPECIFIERS::BEAMS | GANTRY_SPECIFIERS::DETECTOR_SURFACES );
	addObject( ax2, "TestModel", mod, "g", 0.015 );

	closeAxis( ax2 );

	//radonTransformed sinogram = testTomography.recordSlice();

	//ofstream ax1 = openAxis( path( "./test_Tomography.txt" ), true );

	//addSingleObject( ax1, "Sinogram", sinogram.Data(), "Angle;Distance;Energy;Dots", false );

	//closeAxis( ax1 );

	return true;
}