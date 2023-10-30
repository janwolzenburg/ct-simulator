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
#include <chrono>

#include "test_processing.h"
#include "projections.h"
#include "plotting.h"
#include "tomography.h"
#include "test_model.h"
#include "model.h"
#include "coordinateSystemTree.h"
#include "test_device.h"
#include "filter.h"
#include "backprojection.h"
#include "serialization.h"


/*********************************************************************
   Implemnetations
*********************************************************************/

bool test_radonTransform( void ){


	RadonTransformationProperties radonParameter{
											125, 41,
											500
	};

	Projections test_Sinogram{ radonParameter };

	ofstream ax1 = openAxis( path( "./test_radonTransform.txt" ), true );

	addSingleObject( ax1, "Sinogram", test_Sinogram.data(), "Angle;Distance;Energy;Dots", false );

	closeAxis( ax1 );

	return true;
}

bool test_detector_to_sinogram( void ){

	Gantry testGantry = getTestGantry( GridIndex{ 21, 7 }, 1 );

	ofstream ax2 = openAxis( path( "test_detector_to_sinogram_gantry.txt" ), true );
	addObject( ax2, "Gantry", testGantry, "r", GANTRY_SPECIFIERS::ORIGIN | GANTRY_SPECIFIERS::DETECTOR_SURFACES | GANTRY_SPECIFIERS::DETECTOR_NORMALS );
	closeAxis( ax2 );

	/*
	radonProperties radonParameter = testGantry.getDetectorParameter();
	CoordinateSystem* radonCSys = testGantry.CSys()->CreateCopy( "Radon System" );

	// Create sinogram 
	radonTransformed sinogram{ radonParameter };

	for( size_t currentFrame = 0; currentFrame < radonParameter.framesToFillSinogram; currentFrame++ ){
		// Get the detection result
		vector<pixel> detectionPixel = testGantry.getPixel();

		// Iterate all pixel
		for( pixel currentPixel : detectionPixel ){
			// Get Coordinates for pixel
			radonCoords newRadonCoordinates{ radonCSys, currentPixel.NormalLine() };
			// Get the radon point
			radonPoint newRadonPoint{ newRadonCoordinates, 1. };
			// Assign the data_ to sinogram
			sinogram.assignData( newRadonPoint );
		}



		string filename = "./test_detector_to_sinogram_";
		filename += to_string(currentFrame) + ".txt";

		ofstream ax1 = openAxis( path( filename ), true );
		addSingleObject( ax1, "Sinogram", sinogram.Data(), "Angle;Distance;Energy;Dots", false );
		closeAxis( ax1 );


		// Rotate gantry
		testGantry.rotateCounterClockwise( radonParameter.resolution.c );
	}

	*/

	

	return true;
}


bool test_Tomography( void ){

	/*

	auto start = std::chrono::system_clock::now();
	gantry testGantry = getTestGantry( GridIndex{ 900, 300 }, 1 );
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> diff = end - start;
	cout << "Time for gantry construction: " << diff << endl;

	start = std::chrono::system_clock::now();
	Model mod = getTestModel( GlobalSystem(), 10 );
	end = std::chrono::system_clock::now();
	diff = end - start;
	cout << "Time for test model construction: " << diff << endl;

	start = std::chrono::system_clock::now();
	tomography testTomography( TomographyProperties{ 1., true, 16, 0.05, 5e-2 } );
	end = std::chrono::system_clock::now();
	diff = end - start;
	cout << "Time for test tomography construction: " << diff << endl;

	//ofstream ax2 = openAxis( path( "./test_Tomography_gantry_300x100_1_4xModelRes.txt" ), true );

	//addObject( ax2, "Gantry", testGantry, "direction_", GANTRY_SPECIFIERS::ORIGIN | GANTRY_SPECIFIERS::BEAMS | GANTRY_SPECIFIERS::DETECTOR_SURFACES );
	//addObject( ax2, "TestModel", mod, "g", 0.015 );

	//closeAxis( ax2 );

	start = std::chrono::system_clock::now();
	radonTransformed sinogram = testTomography.recordSlice( testGantry, mod, 0. );
	end = std::chrono::system_clock::now();
	diff = end - start;
	cout << "Time for slice: " << diff << endl;

	start = std::chrono::system_clock::now();
	vector<char> serializedData;
	sinogram.Serialize( serializedData );
	ExportSerialized( string{"test_Tomography_serialized_sinogram_900x300_1_10xModelRes.txt"}, serializedData );
	end = std::chrono::system_clock::now();
	diff = end - start;
	cout << "Time for test sinogram export: " << diff << endl;

	//ofstream ax1 = openAxis( path( "./test_Tomography_300x100_1_4xModelRes.txt" ), true );

	//addSingleObject( ax1, "Sinogram", sinogram.Data(), "Angle;Distance;Energy;Dots", false );

	//closeAxis( ax1 );

	//ofstream ax3 = openAxis( path( "./test_Tomography_300x100_1_4xModelRes_image.txt" ), true );

	//addSingleObject( ax3, "Sinogram", sinogram.Data(), "Angle;Distance;Energy;Dots", true );

	//closeAxis( ax3 );
	*/

	return true;
}



void serialisedToImage( void ){

	
	vector<char> importedData = ImportSerialized( string{ "./test_Tomography_serialized_sinogram_900x300_1_10xModelRes.txt" } );

	vector<char>::const_iterator readStart = importedData.cbegin();


	Projections importedSinogram{ importedData, readStart };
	filteredProjections Q{ importedSinogram, BackprojectionFilter::ramLak };
	reconstrucedImage image{ Q };

	ofstream ax1 = openAxis( path( "./test_Tomography_900_300_1_10xModelRes.txt" ), true );
	addSingleObject( ax1, "Sinogram", importedSinogram.data(), "Angle;Distance;Energy;Dots", true );
	closeAxis( ax1 );

	ofstream ax2 = openAxis( path( "./test_Reconstruction_900_300_1_10xModelRes.txt" ), true );
	addSingleObject( ax2, "ReconstrucedImage", image.getGrid(), "X;Y;Absorbtion;Dots", true );
	closeAxis( ax2 );

}

bool test_serialisation( void ){

	RadonTransformationProperties radonParameter{
											8, 8,
											500
	};

	Projections testSinogram{ radonParameter };

	for( size_t col = 0; col < testSinogram.data().size().c; col++ ){
		for( size_t row = 0; row < testSinogram.data().size().r; row++ ){
			testSinogram.AssignData( GridIndex{ col, row }, 1. * (double) (col * testSinogram.data().size().r + row ));
		}
	}

	vector<char> serializedData;
	testSinogram.Serialize( serializedData );

	ExportSerialized( string{"test_serialisation.txt"}, serializedData );

	vector<char> importedData = ImportSerialized( string{ "test_serialisation.txt" } );

	vector<char>::const_iterator readStart = importedData.cbegin();
	Projections importedSinogram{ importedData, readStart };


	if( testSinogram.data().size().c != importedSinogram.data().size().c ) return false;
	if( testSinogram.data().size().r != importedSinogram.data().size().r ) return false;

	if( testSinogram.data().start().c != importedSinogram.data().start().c ) return false;
	if( testSinogram.data().start().r != importedSinogram.data().start().r ) return false;

	if( testSinogram.data().resolution().c != importedSinogram.data().resolution().c ) return false;
	if( testSinogram.data().resolution().r != importedSinogram.data().resolution().r ) return false;


	for( size_t col = 0; col < testSinogram.data().size().c; col++ ){
		for( size_t row = 0; row < testSinogram.data().size().r; row++ ){

			

			if( testSinogram.data().GetData( GridIndex{ col, row }) != importedSinogram.data().GetData( GridIndex{ col, row } ) ) return false;
		}
	}

	return true;

}


bool test_filter( void ){

	signed long long N = 101;
	NaturalNumberRange NumberRange{ -N + 1, N - 1 };
	double samplingInterval = 5;

	BackprojectionFilter h{ NumberRange, samplingInterval, BackprojectionFilter::ramLak };

	vector<Tuple2D> plot;

	for( signed long long int n = h.points_range().start(); n <= h.points_range().end(); n++ ) plot.emplace_back( (double) n, h( n ) );

	ofstream ax = openAxis( path( "./test_filter_ramLak.txt" ), true );

	addSingleObject( ax, "RamLakFilter", plot, "n;h(n);line" );

	closeAxis( ax );

	return true;
}

bool test_filteredProjection( void ){

	vector<char> importedData = ImportSerialized( string{ "test_Tomography_serialized_sinogram_300x100_1.txt" } );

	vector<char>::const_iterator readStart = importedData.cbegin();
	Projections importedSinogram{ importedData, readStart };

	filteredProjections Q{ importedSinogram, BackprojectionFilter::ramLak };

	ofstream ax1 = openAxis( path( "./test_filteredProjection.txt" ), true );

	addSingleObject( ax1, "filteredProjections", Q.getGrid(), "Angle;Distance;Energy;Dots", true);

	closeAxis( ax1 );

	return true;
}

bool test_reconstruction( void ){

	vector<char> importedData = ImportSerialized( string{ "test_Tomography_serialized_sinogram_900_300_1_4xModelRes.txt"} );

	vector<char>::const_iterator readStart = importedData.cbegin();
	Projections importedSinogram{ importedData, readStart };

	filteredProjections Q{ importedSinogram, BackprojectionFilter::ramLak };

	reconstrucedImage image{ Q };

	ofstream ax1 = openAxis( path( "./test_reconstruction900_300_1_4xModelRes.txt" ), true );

	addSingleObject( ax1, "filteredProjections", image.getGrid(), "X;Y;Absorbtion;Dots", true );

	closeAxis( ax1 );

	return true;
}