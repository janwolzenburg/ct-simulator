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

bool test_detector(void) {

	detectorParameter detectorParas{	.rows = 2,
										.columns = 11,
										.rowSize = 2,
										.colSize = 2,
										.structured = false };

	detector test_detector{ GLOBAL_CSYS()->createCopy("Detector system"), 50, detectorParas };

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