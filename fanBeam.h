#pragma once
/*********************************************************************
 * @file   fanBeam.h
 * @brief  Class for a fanned beam of rays
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


 /*********************************************************************
   Includes
*********************************************************************/
#include <vector>
using std::vector;

#include "cartesian.h"
#include "rays.h"



/*********************************************************************
   Definitions
*********************************************************************/


class fanBeam {

	fanBeam( cartCSys* const cSys_, const size_t numRays_, const spectrum spectrum_, const double angle_ );

	
	protected:
	cartCSys* cSys;				/*!<Coordinate system of beam*/
	vector<ray> rays;			/*!<All rays in beam in local coordinate system*/

};