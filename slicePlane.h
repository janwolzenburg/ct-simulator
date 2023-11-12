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
 * @brief Class for slicing a model
*/

class SlicePlane{

	public:

	/*!
	 * @brief Default constructor 
	*/
	SlicePlane( void );

	/*!
	 * @brief Constructor from serialized data
	 * @param binary_data Reference to vector with binary data
	 * @param current_byte Iterator to start of data in vector
	*/
	SlicePlane( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	/*!
	 * @brief Serialize this object
	 * @param binary_data Reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;


	CoordinateSystem* coordinate_system;	/*!< Coordinate system of slice plane*/
	Surface surface;						/*!< Surface for slicing*/
	double rotation_angle_x;	/*!< Current x-rotation angle*/
	double rotation_angle_y;	/*!< Current y-rotation angle*/
	double position_z;			/*!< Current z-position*/

};