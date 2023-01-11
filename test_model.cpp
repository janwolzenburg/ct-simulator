/*********************************************************************
 * @file   test_model.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/


 /*********************************************************************
	 Includes
  *********************************************************************/

#include "plotting.h"
#include "test_model.h"
#include "cSysTree.h"
#include "gantry.h"


  /*********************************************************************
	 Implementations
  *********************************************************************/


model getTestModel( const cartCSys* const parent ){

	cartCSys* modelSys = parent->addCSys( v3{ -200, -200, -200 }, v3{ 1, 0, 0 }, v3{ 0, 1, 0 }, v3{ 0, 0, 1 }, "Model system" );

	model mod{ modelSys, idx3 {40, 40, 40}, v3 {10., 10., 10.} };

	voxData bgData = { 0.0 };

	pnt3 sp1_center = { v3{ 150, 150, 190 }, mod.CSys() };
	double sp1_radius = 50;
	voxData sp1_data = { 0.02 };

	pnt3 sp2_center = { v3{ 200,220, 200 }, mod.CSys() };
	double sp2_radius = 20;
	voxData sp2_data = { 0.04 };

	for( size_t x = 0; x < mod.NumVox().x; x++ ){
		for( size_t y = 0; y < mod.NumVox().y; y++ ){
			for( size_t z = 0; z < mod.NumVox().z; z++ ){
				pnt3 p{ { (double) x * mod.VoxSize().x , (double) y * mod.VoxSize().y , (double) z * mod.VoxSize().z }, modelSys };
				if( ( sp1_center - p ).Length() <= sp1_radius && ( true || ( sp1_center - p ).Length() >= sp1_radius - 1.1 ) ) mod( x, y, z ) = sp1_data;
				else if( ( sp2_center - p ).Length() <= sp2_radius && ( true || ( sp2_center - p ).Length() >= sp2_radius - 1.1 ) ) mod( x, y, z ) = sp2_data;
				else mod( x, y, z ) = bgData;
			}
		}
	}

	return mod;
}



bool test_testModel( void ){

	
	model mod = getTestModel( GLOBAL_CSYS() );

	ofstream ax1 = openAxis( path( "./test_testModel.txt" ), true );

	addObject( ax1, "TestModel", mod, "g", 0.015 );

	closeAxis( ax1 );

	return true;
}


bool test_modelTransmission( void ){

	tubeParameter tubeParas{ .anodeVoltage_V = 53000,
							.anodeCurrent_A = 0.2,
							.anodeAtomicNumber = 74 };

	detectorParameter detectorParas{ .columns = 25,
										.rowSize = 10,
										.colSize = 20,
										.structured = false };



	gantry testGantry{ GLOBAL_CSYS()->createCopy( "Gantry system" ), 300., PI / 4., 60, tubeParas, detectorParas };
	model mod = getTestModel( GLOBAL_CSYS() );

	ofstream ax1 = openAxis( path( "./test_modelTransmission.txt" ), true );

	addObject( ax1, "Gantry", testGantry, "r", 0 );
	addObject( ax1, "TestModel", mod, "g", 0.015 );


	for( const ray r : testGantry.getBeam() ){
		
		rayVoxelIntersection rayVoxIsect{ mod.Vox(), r };
		rayVox_Intersection_Result entrance = rayVoxIsect.Entrance();
		rayVox_Intersection_Result exit = rayVoxIsect.Exit();

		addSingleObject( ax1, "Entrance", entrance.isectPnt, string{ "g" } );
		addSingleObject( ax1, "Exit", exit.isectPnt, string{ "r" } );
	}


	closeAxis( ax1 );


	testGantry.radiate( mod );
	vector<pixel> detectorPixel = testGantry.getPixel();
	vector<v2> primitiveDetectionResult( detectorPixel.size(), v2{0, 0});

	for( size_t i = 0; i < detectorPixel.size(); i++ ){	
		primitiveDetectionResult.at( i ).x = (double) i;
		for( rayProperties currentProperties : detectorPixel.at( i ).detectedRayProperties ){
			primitiveDetectionResult.at( i ).y += currentProperties.powerSpectrum.getSum();
		}
	}	

	ofstream ax2 = openAxis( path( "./test_modelTransmission_Result.txt" ), true );
	addSingleObject( ax2, "Result", primitiveDetectionResult, string{ "PixelNum;Result;Dots" });
	closeAxis( ax2 );

	return true;
}