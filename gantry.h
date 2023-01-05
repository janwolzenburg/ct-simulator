#pragma once
/*********************************************************************
 * @file   gantry.h
 * @brief  Gantry class
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/



 /*********************************************************************
	Includes
 *********************************************************************/
#include <vector>
using std::vector;

#include "generelMath.h"
#include "tube.h"
#include "detector.h"


/*********************************************************************
   Definitions
*********************************************************************/



class gantry {

	gantry( cartCSys* const cSys_, const double radius, const tubeParameters tubeParameters_, const detectorParameter detectorParameters_ );



	private:
	cartCSys* cSys;
	tube raySource;
	detector rayDetector;

};
