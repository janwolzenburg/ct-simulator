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



bool test_tube(void) {

	
	tube testTube{ 53000, 0.2, 74, GLOBAL_CSYS()->createCopy( "Tube system" )};

	vector<ray> beam = testTube.getBeam( PI/4, 20 );


	return true;
}