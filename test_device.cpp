/*********************************************************************
 * @file   tests_device.cpp
 * @brief  Implementation
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include <iostream>
using std::cerr; using std::cout; using std::endl;

#include <vector>
using std::vector;

#include "generelMath.h"
#include "cSysTree.h"
#include "test_device.h"
#include "tube.h"
#include "detector.h"
#include "plotting.h"
#include "gantry.h"

//TODO: fix tests
bool test_tube(void) {

	tubeParameter tubeParas{	.anodeVoltage_V = 53000,
								.anodeCurrent_A = 0.2,
								.anodeAtomicNumber = 74 };

	tube testTube{ GLOBAL_CSYS()->createCopy( "Tube system" ), tubeParas };

	vector<ray> beam = testTube.getBeam( PI/4, 20 );

	ofstream ax1 = openAxis( path( "./test_tube.txt" ), true );

	addObject( ax1, "Beam", beam, "r", 1. );

	closeAxis( ax1 );

	return true;
}

detector getTestDetector( void ){
	// 64 x 32 points in radon space
	// 500mm measure field
	detectorRadonParameter radonParameter{
											idx2CR{ 64, 32 },
											500
	};

	// 50 degree angle
	detector testDetector{ GLOBAL_CSYS()->createCopy( "Detector system" ), radonParameter, 1, 2*PI*( 50./360. ), 1, false };

	return testDetector;
}

bool test_detector(void) {


	detector test_detector = getTestDetector();

	vector<pixel> allPixel = test_detector.getPixel();

	ofstream ax1 = openAxis(path("./test_detector.txt"), true);

	addObject( ax1, "Detector", allPixel, "r", .2 );
	
	closeAxis(ax1);
	
	return true;

}


bool test_gantry( void ){

	tubeParameter tubeParas{	.anodeVoltage_V = 53000,
								.anodeCurrent_A = 0.2,
								.anodeAtomicNumber = 74 };

	


	gantry testGantry{ GLOBAL_CSYS()->createCopy("Gantry system"), 300., PI/4., 10, tubeParas, detectorParas };

	ofstream ax1 = openAxis( path( "./test_gantry.txt" ), true );

	addObject( ax1, "Gantry", testGantry, "r", 0 );

	testGantry.rotateCounterClockwise( 2.*PI/3 );

	addObject( ax1, "Gantry", testGantry, "r", 0 );

	closeAxis( ax1 );

	return true;
}