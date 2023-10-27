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
#include "coordinateSystemTree.h"
#include "gantry.h"
#include "test_device.h"
#include "tomography.h"


  /*********************************************************************
	 Implementations
  *********************************************************************/


model getTestModel( const CoordinateSystem* const parent, const size_t res ){

	CoordinateSystem* modelSys = parent->AddCoordinateSystem( Tuple3D{ -200, -200, -200 }, Tuple3D{ 1, 0, 0 }, Tuple3D{ 0, 1, 0 }, Tuple3D{ 0, 0, 1 }, "Model system" );

	model mod{ modelSys, Index3D {  20 * res, 20 * res, 20 * res}, Tuple3D {20. / (double) res, 20. / (double) res, 20 / (double) res }, "testModel" + to_string( res ) + "x"};

	//double kWater = 0.01611970000;

	voxData bgData = { 0.01, 120000. };

	Point3D sp1_center = { Tuple3D{ 120, 120, 200 }, mod.CSys() };
	double sp1_radius = 60;
	voxData sp1_data = { 0.3, 120000. };

	Point3D sp2_center = { Tuple3D{ 280, 280, 200 }, mod.CSys() };
	double sp2_radius = 70;
	voxData sp2_data = { 0.5, 120000. };


	Point3D artifact{ Tuple3D{ 250, 200, 200}, mod.CSys() };
	double artRadius = 15;


	for( size_t x = 0; x < mod.NumVox().x; x++ ){
		for( size_t y = 0; y < mod.NumVox().y; y++ ){
			for( size_t z = 0; z < mod.NumVox().z; z++ ){
				Point3D p{ { (double) x * mod.VoxSize().x , (double) y * mod.VoxSize().y , (double) z * mod.VoxSize().z }, modelSys };


				if( ( sp1_center - p ).length() <= sp1_radius && ( true || ( sp1_center - p ).length() >= sp1_radius - 1.1 ) )  mod.setVoxelData( sp1_data, { x, y, z } );
				else if( ( sp2_center - p ).length() <= sp2_radius && ( true || ( sp2_center - p ).length() >= sp2_radius - 1.1 ) ) mod.setVoxelData( sp2_data, { x, y, z } );
				else mod.setVoxelData( bgData, { x, y, z } );
			
				if( ( artifact - p ).length() <= artRadius && ( true || ( artifact - p ).length() >= artRadius - 1.1 ) )  mod.setVoxelProperty( voxData::METAL, { x, y, z } );
			
			}
		}
	}


	return mod;
}


void save_testModel( void ){

	model mod = getTestModel( GlobalSystem(), 10 );
	
	vector<char> binary_data;
	mod.Serialize( binary_data );

}

bool test_testModel( void ){

	
	model mod = getTestModel( GlobalSystem() );

	ofstream ax1 = openAxis( path( "./test_testModel.txt" ), true );

	addObject( ax1, "TestModel", mod, "g", 0.015 );

	closeAxis( ax1 );

	return true;
}


bool test_modelTransmission( void ){

	gantry testGantry = getTestGantry( GridIndex{ 70, 20 }, 1 );
	model mod{ getTestModel( GlobalSystem() ) };

	ofstream ax1 = openAxis( path( "./test_modelTransmission.txt" ), true );

	addObject( ax1, "Gantry", testGantry, "r", GANTRY_SPECIFIERS::DETECTOR_SURFACES | GANTRY_SPECIFIERS::BEAMS );
	addObject( ax1, "TestModel", mod, "g", 0.015 );



	for( const ray r : testGantry.getBeam( 1. ) ){
		
		rayVoxelIntersection rayVoxIsect{ mod.Vox(), r };
		rayVox_Intersection_Result entrance = rayVoxIsect.entrance;
		rayVox_Intersection_Result exit = rayVoxIsect.exit;

		addSingleObject( ax1, "Entrance", entrance.intersectionPoint, string{ "g" } );
		addSingleObject( ax1, "Exit", exit.intersectionPoint, string{ "r" } );
	}


	closeAxis( ax1 );


	testGantry.radiate( mod, tomographyParameter{ 1., false, 16, .05, 5e-2 } );
	vector<pixel> detectorPixel = testGantry.getPixel();

	std::sort( detectorPixel.begin(), detectorPixel.end(), [] ( const pixel& p1, const pixel& p2 ){ return p1.O().Y() < p2.O().Y(); });

	vector<Tuple2D> primitiveDetectionResult( detectorPixel.size(), Tuple2D{0, 0});

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