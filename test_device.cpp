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


bool test_tube(void) {

	
	tube testTube{ 53000, 0.2, 74, GLOBAL_CSYS()->createCopy( "Tube system" )};

	vector<ray> beam = testTube.getBeam( PI/4, 20 );

	ofstream ax1 = openAxis( path( "./test_tube.txt" ), true );

	addObject( ax1, "Beam", beam, "r", 1. );

	closeAxis( ax1 );

	return true;
}

bool test_detector(void) {

	detector test_detector{ GLOBAL_CSYS()->createCopy("Detector system"),2, 11, t2{ 2, 2}, 500, false };

	vector<vector<pixel>> allPixel = test_detector.getPixel();

	ofstream ax1 = openAxis(path("./test_detector.txt"), true);

	for (auto rowIt = allPixel.begin(); rowIt < allPixel.end(); rowIt++) {
		
		vector<surfLim> rowPixel;
		for (auto pxIt = rowIt->begin(); pxIt < rowIt->end(); pxIt++) {
			rowPixel.push_back( *pxIt );
		}

		addObject(ax1, "Detector", rowPixel, "r", .2);

	}
	
	closeAxis(ax1);
	
	return true;

}