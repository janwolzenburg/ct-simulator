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
#include "grid.h"


/*********************************************************************
  Definitions
*********************************************************************/

class filteredProjections : public grid {

	public:
	filteredProjections( const radonTransformed projections, const discreteFilter::TYPE filterType );
	
	double getValue( const size_t angleIdx, const double distance ) const;

};


class reconstrucedImage : public grid{

	public:
	reconstrucedImage( const filteredProjections projections );

};
