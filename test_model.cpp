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
#include "test_device.h"



  /*********************************************************************
	 Implementations
  *********************************************************************/


model getTestModel( const cartCSys* const parent, const size_t res ){

	cartCSys* modelSys = parent->addCSys( v3{ -200, -200, -200 }, v3{ 1, 0, 0 }, v3{ 0, 1, 0 }, v3{ 0, 0, 1 }, "Model system" );

	model mod{ modelSys, idx3 {  20 * res, 20 * res, 20 * res}, v3 {20. / (double) res, 20. / (double) res, 20 / (double) res }, "testModel" + to_string( res ) + "x"};

	double kWater = 0.01611970000;

	voxData bgData = { 0.01, 120000. };

	pnt3 sp1_center = { v3{ 120, 120, 200 }, mod.CSys() };
	double sp1_radius = 60;
	voxData sp1_data = { 0.3, 120000. };

	pnt3 sp2_center = { v3{ 280, 280, 200 }, mod.CSys() };
	double sp2_radius = 70;
	voxData sp2_data = { 0.5, 120000. };


	pnt3 artifact{ v3{ 250, 200, 200}, mod.CSys() };
	double artRadius = 15;


	for( size_t x = 0; x < mod.NumVox().x; x++ ){
		for( size_t y = 0; y < mod.NumVox().y; y++ ){
			for( size_t z = 0; z < mod.NumVox().z; z++ ){
				pnt3 p{ { (double) x * mod.VoxSize().x , (double) y * mod.VoxSize().y , (double) z * mod.VoxSize().z }, modelSys };


				if( ( sp1_center - p ).Length() <= sp1_radius && ( true || ( sp1_center - p ).Length() >= sp1_radius - 1.1 ) )  mod.setVoxelData( sp1_data, { x, y, z } );
				else if( ( sp2_center - p ).Length() <= sp2_radius && ( true || ( sp2_center - p ).Length() >= sp2_radius - 1.1 ) ) mod.setVoxelData( sp2_data, { x, y, z } );
				else mod.setVoxelData( bgData, { x, y, z } );
			
				if( ( artifact - p ).Length() <= artRadius && ( true || ( artifact - p ).Length() >= artRadius - 1.1 ) )  mod.setVoxelProperty( voxData::METAL, { x, y, z } );
			
			}
		}
	}


	return mod;
}


void save_testModel( void ){

	model mod = getTestModel( GLOBAL_CSYS(), 10 );
	
	vector<char> binData;
	mod.serialize( binData );

}

bool test_testModel( void ){

	
	model mod = getTestModel( GLOBAL_CSYS() );

	ofstream ax1 = openAxis( path( "./test_testModel.txt" ), true );

	addObject( ax1, "TestModel", mod, "g", 0.015 );

	closeAxis( ax1 );

	return true;
}


bool test_modelTransmission( void ){

	gantry testGantry = getTestGantry( idx2CR{ 70, 20 }, 1 );
	model mod = getTestModel( GLOBAL_CSYS() );

	ofstream ax1 = openAxis( path( "./test_modelTransmission.txt" ), true );

	addObject( ax1, "Gantry", testGantry, "r", GANTRY_SPECIFIERS::DETECTOR_SURFACES | GANTRY_SPECIFIERS::BEAMS );
	addObject( ax1, "TestModel", mod, "g", 0.015 );



	for( const ray r : testGantry.getBeam( 1. ) ){
		
		rayVoxelIntersection rayVoxIsect{ mod.Vox(), r };
		rayVox_Intersection_Result entrance = rayVoxIsect.Entrance();
		rayVox_Intersection_Result exit = rayVoxIsect.Exit();

		addSingleObject( ax1, "Entrance", entrance.intersectionPoint, string{ "g" } );
		addSingleObject( ax1, "Exit", exit.intersectionPoint, string{ "r" } );
	}


	closeAxis( ax1 );


	testGantry.radiate( mod, 1. );
	vector<pixel> detectorPixel = testGantry.getPixel();

	std::sort( detectorPixel.begin(), detectorPixel.end(), [] ( const pixel& p1, const pixel& p2 ){ return p1.O().Y() < p2.O().Y(); });

	vector<v2> primitiveDetectionResult( detectorPixel.size(), v2{0, 0});

	for( size_t i = 0; i < detectorPixel.size(); i++ ){	
		primitiveDetectionResult.at( i ).x = (double) i;
		for( rayProperties currentProperties : detectorPixel.at( i ).getProperties() ){
			primitiveDetectionResult.at( i ).y += currentProperties.EnergySpectrum().getSum();
		}
	}	

	ofstream ax2 = openAxis( path( "./test_modelTransmission_Result_newApproach.txt" ), true );
	addSingleObject( ax2, "Result", primitiveDetectionResult, string{ "PixelNum;Result;Dots" });
	closeAxis( ax2 );

	return true;
}