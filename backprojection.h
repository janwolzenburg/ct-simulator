#pragma once
/*********************************************************************
 * @file   backprojection.h
 * @brief  Classes for backprojections
 *
 * @author Jan Wolzenburg
 * @date   Febuary 2023
 *********************************************************************/


 /*********************************************************************
   Includes
*********************************************************************/

#include "generel.h"
#include "filter.h"
#include "radonTransform.h"



/*********************************************************************
  Definitions
*********************************************************************/

class filteredProjections{

	public:
	filteredProjections( const radonTransformed projections, const discreteFilter::TYPE filterType );

	grid Data( void ) const;

	private:
	grid data;

};