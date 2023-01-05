/*********************************************************************
 * @file   gantry.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/



 /*********************************************************************
   Includes
*********************************************************************/

#include "gantry.h"



/*********************************************************************
  Implementations
*********************************************************************/


gantry::gantry( cartCSys* const cSys_, const double radius, const tubeParameter tubeParameters_, const detectorParameter detectorParameters_ ) : 

	cSys( cSys_ ),
	raySource{ cSys->addCSys( primitiveVec3{ 0, radius, 0 }, primitiveVec3{ 1, 0, 0 }, primitiveVec3{ 0, -1, 0 }, primitiveVec3{ 0, 0, 1 }, "xRay tube" ), tubeParameters_ },
	rayDetector{ cSys->addCSys( primitiveVec3{ 0, -radius, 0 }, primitiveVec3{ 1, 0, 0 }, primitiveVec3{ 0, 1, 0 }, primitiveVec3{ 0, 0, 1 }, "xRay detector" ), radius, detectorParameters_ }
{

	//cSys->addCSys( primitiveVec3{ 0, radius, 0 }, primitiveVec3{ 1, 0, 0 }, primitiveVec3{ 0, -1, 0 }, primitiveVec3{ 0, 0, 1 }, "xRay tube" );

}