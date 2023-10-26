#pragma once
/*********************************************************************
 * @file   slicePlan.h
 * @brief  
 *
 * @author Jan Wolzenburg
 * @date   April 2023
 *********************************************************************/

#include "cSysTree.h"
#include "surf.h"

/*!
 * @brief Class for slicing a model
*/
class slicePlane{


	public:
	
	static const string FILE_PREAMBLE;

	/*!
	 * @brief Default constructor 
	*/
	slicePlane( void );

	/*!
	 * @brief Constructor from serialized data
	 * @param binary_data Reference to vector with binary data
	 * @param it Iterator to start of data in vector
	*/
	slicePlane( const vector<char>& binary_data, vector<char>::const_iterator& it );

	/*!
	 * @brief Serialize this object
	 * @param binary_data Reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;

	public:

	cartCSys* cSys;			/*!<Coordinate system of slice plane*/
	surf surface;			/*!<Surface for slicing*/
	double rotationAngleX;	/*!<Current x-rotation angle*/
	double rotationAngleY;	/*!<Current y-rotation angle*/
	double positionZ;		/*!<Current z-position*/

};