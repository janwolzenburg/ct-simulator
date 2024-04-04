#pragma once
/*********************************************************************
 * @file   slicePlan.h
 * @brief  
 *
 * @author Jan Wolzenburg
 * @date   April 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include "coordinateSystemTree.h"
#include "surface.h"


 /*********************************************************************
	Definitions
 *********************************************************************/

/*!
 * @brief class for slicing a model
*/

class SlicePlane{

	public:

	/*!
	 * @brief default constructor 
	*/
	SlicePlane( void );

	/*!
	 * @brief constructor from serialized data
	 * @param binary_data reference to vector with binary data
	 * @param current_byte Iterator to start of data in vector
	*/
	SlicePlane( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	/*!
	 * @brief serialize this object
	 * @param binary_data reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;


	CoordinateSystem* coordinate_system;	/*!< coordinate system of slice plane*/
	Surface surface;						/*!< surface for slicing*/
	double rotation_angle_x;	/*!< current x-rotation angle*/
	double rotation_angle_y;	/*!< current y-rotation angle*/
	double position_z;			/*!< current z-position*/

};