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


Model getTestModel( const CoordinateSystem* const parent, const size_t res ){

	CoordinateSystem* modelSys = parent->AddCoordinateSystem( Tuple3D{ -200, -200, -200 }, Tuple3D{ 1, 0, 0 }, Tuple3D{ 0, 1, 0 }, Tuple3D{ 0, 0, 1 }, "Model system" );

	Model mod{ modelSys, Index3D {  20 * res, 20 * res, 20 * res}, Tuple3D {20. / (double) res, 20. / (double) res, 20 / (double) res }, "testModel" + to_string( res ) + "x"};

	//double kWater = 0.01611970000;

	VoxelData bgData = { 0.01, 120000. };

	Point3D sp1_center = { Tuple3D{ 120, 120, 200 }, mod.coordinate_system() };
	double sp1_radius = 60;
	VoxelData sp1_data = { 0.3, 120000. };

	Point3D sp2_center = { Tuple3D{ 280, 280, 200 }, mod.coordinate_system() };
	double sp2_radius = 70;
	VoxelData sp2_data = { 0.5, 120000. };


	Point3D artifact{ Tuple3D{ 250, 200, 200}, mod.coordinate_system() };
	double artRadius = 15;


	for( size_t x = 0; x < mod.number_of_voxel_3D().x; x++ ){
		for( size_t y = 0; y < mod.number_of_voxel_3D().y; y++ ){
			for( size_t z = 0; z < mod.number_of_voxel_3D().z; z++ ){
				Point3D p{ { (double) x * mod.voxel_size().x , (double) y * mod.voxel_size().y , (double) z * mod.voxel_size().z }, modelSys };


				if( ( sp1_center - p ).length() <= sp1_radius && ( true || ( sp1_center - p ).length() >= sp1_radius - 1.1 ) )  mod.SetVoxelData( sp1_data, { x, y, z } );
				else if( ( sp2_center - p ).length() <= sp2_radius && ( true || ( sp2_center - p ).length() >= sp2_radius - 1.1 ) ) mod.SetVoxelData( sp2_data, { x, y, z } );
				else mod.SetVoxelData( bgData, { x, y, z } );
			
				if( ( artifact - p ).length() <= artRadius && ( true || ( artifact - p ).length() >= artRadius - 1.1 ) )  mod.SetVoxelProperties( VoxelData::Metal, { x, y, z } );
			
			}
		}
	}


	return mod;
}


void save_testModel( void ){

	Model mod = getTestModel( GlobalSystem(), 10 );
	
	vector<char> binary_data;
	mod.Serialize( binary_data );

}

bool test_testModel( void ){

	
	Model mod = getTestModel( GlobalSystem() );

	ofstream ax1 = openAxis( path( "./test_testModel.txt" ), true );

	addObject( ax1, "TestModel", mod, "g", 0.015 );

	closeAxis( ax1 );

	return true;
}


bool test_modelTransmission( void ){

	Gantry testGantry = getTestGantry( GridIndex{ 70, 20 }, 1 );
	Model mod{ getTestModel( GlobalSystem() ) };

	ofstream ax1 = openAxis( path( "./test_modelTransmission.txt" ), true );

	addObject( ax1, "Gantry", testGantry, "r", GANTRY_SPECIFIERS::DETECTOR_SURFACES | GANTRY_SPECIFIERS::BEAMS );
	addObject( ax1, "TestModel", mod, "g", 0.015 );


	/*
	for( const Ray r : testGantry.GetEmittedBeam( 1. ) ){
		
		RayVoxelIntersection rayVoxIsect{ mod.GetModelVoxel(), r };

		addSingleObject( ax1, "Entrance", rayVoxIsect.entrance_.intersection_point_, string{ "g" } );
		addSingleObject( ax1, "Exit", rayVoxIsect.exit_.intersection_point_, string{ "r" } );
	}*/


	closeAxis( ax1 );


	testGantry.RadiateModel( mod, TomographyProperties{ 1., false, 16, .05, 5e-2 } );
	vector<DetectorPixel> detectorPixel = testGantry.pixel_array();

	std::sort( detectorPixel.begin(), detectorPixel.end(), [] ( const DetectorPixel& p1, const DetectorPixel& p2 ){ return p1.origin().Y() < p2.origin().Y(); });

	vector<Tuple2D> primitiveDetectionResult( detectorPixel.size(), Tuple2D{0, 0});

	for( size_t i = 0; i < detectorPixel.size(); i++ ){	
		primitiveDetectionResult.at( i ).x = (double) i;
		for( RayProperties currentProperties : detectorPixel.at( i ).detected_ray_properties() ){
			primitiveDetectionResult.at( i ).y += currentProperties.energy_spectrum().GetTotal();
		}
	}	

	ofstream ax2 = openAxis( path( "./test_modelTransmission_Result_newApproach.txt" ), true );
	addSingleObject( ax2, "Result", primitiveDetectionResult, string{ "PixelNum;Result;Dots" });
	closeAxis( ax2 );

	return true;
}