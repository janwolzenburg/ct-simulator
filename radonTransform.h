#pragma once
/*********************************************************************
 * @file   radonTransform.h
 * @brief  Classes radon transformation
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/



 /*********************************************************************
  Includes
*********************************************************************/

#include "line.h"


/*********************************************************************
   Definitions
*********************************************************************/


class radonCoords{

	public:
	radonCoords( const double theta_, const double distance_ );

	radonCoords( const cartCSys* const cSys, const line l );

	radonCoords( void );

	double theta = 0;
	double distance = 0;
	
};

