#pragma once
/*********************************************************************
 * @file   tomography.h
 * @brief  Classes 
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/



 /*********************************************************************
  Includes
*********************************************************************/
#include "gantry.h"
#include "model.h"
#include "radonTransform.h"


/*********************************************************************
   Definitions
*********************************************************************/

// TODO: add comments
class tomography{

	public:
	tomography( const gantry gantry_, model& model_ ) : 
	gantry(gantry_),
	model( model_ ),
	radonCSys( gantry.CSys()->createCopy( "Radon System" ))
	{};

	radonTransformed recordSlice( void );

	private:
	gantry gantry;
	model& model;
	cartCSys* radonCSys;
};

