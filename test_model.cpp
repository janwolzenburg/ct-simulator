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