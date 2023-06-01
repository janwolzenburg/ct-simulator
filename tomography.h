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

/*!
 * @brief Class for tomography parameter
*/
class tomographyParameter{

	public:

	static const string FILE_PREAMBLE;

	tomographyParameter( void );

	tomographyParameter( const double exposureTime_, const bool scattering_ );

	tomographyParameter( const vector<char>& binData, vector<char>::const_iterator& it );

	inline double ExposureTime( void ) const { return exposureTime; };

	inline bool Scattering( void ) const{ return scattering; };

	size_t serialize( vector<char>& binData ) const;


	private:

	double exposureTime;
	bool scattering;

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
	radonTransformed recordSlice( gantry gantry_, const model& model_, const double zPosition );


	private:

	tomographyParameter parameter;	/*!<Parameter used for tomography*/
	cartCSys* radonCSys;			/*!<Coordinate system to use as reference for radon coordinates calculation*/
};

