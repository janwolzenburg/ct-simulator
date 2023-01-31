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


	detectorRadonParameter radonParameter{
											idx2CR{ 125, 41 },
											500
	};

	radonTransformed test_Sinogram{ radonParameter };

	ofstream ax1 = openAxis( path( "./test_radonTransform.txt" ), true );

	addSingleObject( ax1, "Sinogram", test_Sinogram.Data(), "Angle;Distance;Energy;Dots", false );

	closeAxis( ax1 );

	return true;
}

bool test_detector_to_sinogram( void ){

	gantry testGantry = getTestGantry( idx2CR{ 21, 7 }, 1 );

	ofstream ax2 = openAxis( path( "test_detector_to_sinogram_gantry.txt" ), true );
	addObject( ax2, "Gantry", testGantry, "r", GANTRY_SPECIFIERS::ORIGIN | GANTRY_SPECIFIERS::DETECTOR_SURFACES | GANTRY_SPECIFIERS::DETECTOR_NORMALS );
	closeAxis( ax2 );

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
			radonPoint newRadonPoint{ newRadonCoordinates, 1. };
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

	gantry testGantry = getTestGantry( idx2CR{ 150, 50 }, 1 );
	model mod = getTestModel( GLOBAL_CSYS() );
	
	tomography testTomography{ testGantry, mod };

	ofstream ax2 = openAxis( path( "./test_Tomography_gantry_150x50_1.txt" ), true );

	addObject( ax2, "Gantry", testGantry, "r", GANTRY_SPECIFIERS::ORIGIN | GANTRY_SPECIFIERS::BEAMS | GANTRY_SPECIFIERS::DETECTOR_SURFACES );
	addObject( ax2, "TestModel", mod, "g", 0.015 );

	closeAxis( ax2 );

	radonTransformed sinogram = testTomography.recordSlice();

	vector<char> serializedData;
	sinogram.serialize( serializedData );
	exportSerialized( "test_Tomography_serialized_sinogram_150x50_1.txt", serializedData );

	ofstream ax1 = openAxis( path( "./test_Tomography_150x50_1.txt" ), true );

	addSingleObject( ax1, "Sinogram", sinogram.Data(), "Angle;Distance;Energy;Dots", false );

	closeAxis( ax1 );

	return true;
}


bool test_serialisation( void ){

	detectorRadonParameter radonParameter{
											idx2CR{ 8, 8 },
											500
	};

	radonTransformed testSinogram{ radonParameter };

	for( size_t col = 0; col < testSinogram.Data().Size().col; col++ ){
		for( size_t row = 0; row < testSinogram.Data().Size().row; row++ ){
			testSinogram.assignData( idx2CR{ col, row }, 1. * (double) (col * testSinogram.Data().Size().row + row ));
		}
	}

	vector<char> serializedData;
	testSinogram.serialize( serializedData );

	exportSerialized( "test_serialisation.txt", serializedData );

	vector<char> importedData = importSerialized( "test_serialisation.txt" );

	vector<char>::const_iterator readStart = importedData.cbegin();
	radonTransformed importedSinogram{ importedData, readStart };


	if( testSinogram.Data().Size().col != importedSinogram.Data().Size().col ) return false;
	if( testSinogram.Data().Size().row != importedSinogram.Data().Size().row ) return false;

	if( testSinogram.Data().Start().col != importedSinogram.Data().Start().col ) return false;
	if( testSinogram.Data().Start().row != importedSinogram.Data().Start().row ) return false;

	if( testSinogram.Data().Resolution().col != importedSinogram.Data().Resolution().col ) return false;
	if( testSinogram.Data().Resolution().row != importedSinogram.Data().Resolution().row ) return false;


	for( size_t col = 0; col < testSinogram.Data().Size().col; col++ ){
		for( size_t row = 0; row < testSinogram.Data().Size().row; row++ ){
			if( testSinogram.Data().operator()( idx2CR{ col, row }) != importedSinogram.Data().operator()( idx2CR{ col, row } ) ) return false;
		}
	}

	return true;

}