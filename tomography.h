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

struct tomographyParameter{

	double exposureTime;

};

/*!
 * @brief Class for computed tomography
 * 
*/
class tomography{

	public:
	/*!
	 * @brief Constructor
	 * @param gantry_ 
	 * @param model_ 
	*/
	tomography( const gantry gantry_, model& model_, const tomographyParameter parameter_ );

	/*!
	 * @brief Record a slice
	 * @return Radon transformed of model slice
	*/
	radonTransformed recordSlice( void );

	private:

	gantry Gantry;				/*!<Gantry*/
	model& Model;				/*!<Model*/
	tomographyParameter parameter;
	cartCSys* radonCSys;		/*!<Coordinate system to use as reference for radon coordinates calculation*/
};

