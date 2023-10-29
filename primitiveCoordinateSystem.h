#pragma once
/*********************************************************************
 * @file   PrimitiveCoordinateSystem.h
 * @brief  Classes for cartesian coordinate systems
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



 /*********************************************************************
	Includes
 *********************************************************************/
#include "generelMath.h"
#include "primitiveVector3.h"



 /*********************************************************************
	Definitions
 *********************************************************************/


/*!
 * @brief Class describing a right-handed cartesian coordinate system
*/
class PrimitiveCoordinateSystem : public MathematicalObject{
	
	public:
	
	/*!
	 * @brief Constructor
	 * @param origin Origin of coordinate system
	 * @param ex x-axis
	 * @param ey y-axis
	 * @param ez z-axis
	 * @details Axes must be orthogonal to each other
	*/
	PrimitiveCoordinateSystem( const PrimitiveVector3 origin, const PrimitiveVector3 ex, const PrimitiveVector3 ey, const PrimitiveVector3 ez );

	/*!
	 * @brief Constructor from serialized data_
	 * @param binary_data Reference to vector with binary data_
	 * @param current_byte Iterator to start of data_ in vector
	*/
	PrimitiveCoordinateSystem( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	/*!
	 * @brief Convert coordinate system's data_ to string
	 * @return String with coordinate system's data_
	*/
	string ToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief Serialize this object
	 * @param binary_data Reference to vector where data_ will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;

	/*!
	 * @brief Get primitive origin_
	 * @return Primitive represantation of the origin_
	*/
	PrimitiveVector3 origin( void ) const{ return origin_; };

	/*!
	 * @brief Get primitive x Axis
	 * @return Primitive represantation of the x Axis
	*/
	PrimitiveVector3 ex( void ) const{ return ex_; };

	/*!
	 * @brief Get primitive y Axis
	 * @return Primitive represantation of the y Axis
	*/
	PrimitiveVector3 ey( void ) const{ return ey_; };

	/*!
	 * @brief Get primitive z Axis
	 * @return Primitive represantation of the z Axis
	*/
	PrimitiveVector3 ez( void ) const{ return ez_; };


	protected:

	/*!
	 * @brief Translate coordinate system
	 * @param direction Translation in x, y and z direction
	*/
	void Translate( const PrimitiveVector3 direction ){ origin_ = origin_ + direction; };

	/*!
	 * @brief Rotate coordinate system
	 * @param axis Rotation axis
	 * @param GetAngle Rotation GetAngle
	*/
	MathError Rotate( const PrimitiveVector3 axis, const double arc_angle );


	protected:

	PrimitiveVector3 origin_;		/*!< Origin point */
	PrimitiveVector3 ex_;			/*!< x-axis unit vector */
	PrimitiveVector3 ey_;			/*!< y-axis unit vector */
	PrimitiveVector3 ez_;			/*!< z-axis unit vector */
};
