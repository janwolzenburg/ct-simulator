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


#include "generel.h"
#include "gantry.h"
#include "model.h"
#include "radonTransform.h"



/*********************************************************************
   Definitions
*********************************************************************/

class tomographyParameter{

	public:

	static const string FILE_PREAMBLE;

	tomographyParameter( void );

	tomographyParameter( const double exposureTime_ );

	tomographyParameter( const vector<char>& binData, vector<char>::const_iterator& it );

	inline double ExposureTime( void ) const { return exposureTime; };


	size_t serialize( vector<char>& binData ) const;


	private:

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
	tomography( const tomographyParameter parameter_ );

	tomography( void );

	/*!
	 * @brief Record a slice
	 * @return Radon transformed of model slice
	*/
	radonTransformed recordSlice( gantry& gantry_, const model& model_, const double zPosition = 0. );


	private:

	//gantry& Gantry;				/*!<Gantry*/
	//model& Model;				/*!<Model*/
	tomographyParameter parameter;
	cartCSys* radonCSys;		/*!<Coordinate system to use as reference for radon coordinates calculation*/
};

