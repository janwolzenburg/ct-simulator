#pragma once
/*********************************************************************
 * @file   slicePlan.h
 * @brief  
 *
 * @author Jan Wolzenburg
 * @date   April 2023
 *********************************************************************/

#include "coordinateSystemTree.h"
#include "surface.h"

/*!
 * @brief Class for slicing a model
*/
class slicePlane{


	public:

	/*!
	 * @brief Default constructor 
	*/
	slicePlane( void );

	/*!
	 * @brief Constructor from serialized data_
	 * @param binary_data Reference to vector with binary data_
	 * @param it Iterator to start of data_ in vector
	*/
	slicePlane( const vector<char>& binary_data, vector<char>::const_iterator& it );

	/*!
	 * @brief Serialize this object
	 * @param binary_data Reference to vector where data_ will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;

	public:

	CoordinateSystem* cSys;			/*!<Coordinate system of slice plane*/
	Surface surface;			/*!<Surface for slicing*/
	double rotationAngleX;	/*!<Current x-rotation GetAngle*/
	double rotationAngleY;	/*!<Current y-rotation GetAngle*/
	double positionZ;		/*!<Current z-position*/

};