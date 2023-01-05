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



//TODO: Add comments and descriptions
class gantry {

	public:
	gantry( cartCSys* const cSys_, const double radius, const tubeParameter tubeParameters_, const detectorParameter detectorParameters_ );



	private:
	cartCSys* cSys;
	tube raySource;
	detector rayDetector;

};
