#pragma once
/*********************************************************************
 * @file   primitiveCoordinateSystem.h
 * @brief  classes for cartesian coordinate systems
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



 /*********************************************************************
	Includes
 *********************************************************************/
#include "generelMath.h"
#include "primitivevector3.h"



 /*********************************************************************
	Definitions
 *********************************************************************/


/*!
 * @brief class describing a right-handed cartesian coordinate system
*/
class PrimitiveCoordinateSystem : public MathematicalObject{
	
	public:
	
	/*!
	 * @brief constructor
	 * @param origin origin of coordinate system
	 * @param ex x-axis
	 * @param ey y-axis
	 * @param ez z-axis
	 * @details axes must be orthogonal to each other
	*/
	PrimitiveCoordinateSystem( const Primitivevector3 origin, const Primitivevector3 ex, const Primitivevector3 ey, const Primitivevector3 ez );

	/*!
	 * @brief constructor from serialized data
	 * @param binary_data reference to vector with binary data
	 * @param current_byte iterator to start of data in vector
	*/
	PrimitiveCoordinateSystem( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	/*!
	 * @brief convert coordinate system's data to string
	 * @return string with coordinate system's data
	*/
	string ConvertToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief serialize this object
	 * @param binary_data reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;

	/*!
	 * @brief get primitive origin
	 * @return primitive represantation of the origin
	*/
	Primitivevector3 origin( void ) const{ return origin_; };

	/*!
	 * @brief get primitive x Axis
	 * @return primitive represantation of the x Axis
	*/
	Primitivevector3 ex( void ) const{ return ex_; };

	/*!
	 * @brief get primitive y Axis
	 * @return primitive represantation of the y Axis
	*/
	Primitivevector3 ey( void ) const{ return ey_; };

	/*!
	 * @brief get primitive z Axis
	 * @return primitive represantation of the z Axis
	*/
	Primitivevector3 ez( void ) const{ return ez_; };


	protected:

	/*!
	 * @brief translate coordinate system
	 * @param direction Translation in x, y and z direction
	*/
	void Translate( const Primitivevector3 direction ){ origin_ = origin_ + direction; };

	/*!
	 * @brief rotate coordinate system
	 * @param axis rotation axis
	 * @param angle rotation angle
	*/
	MathError Rotate( const Primitivevector3 axis, const double angle );


	protected:

	Primitivevector3 origin_;		/*!< origin point */
	Primitivevector3 ex_;				/*!< x-axis unit vector */
	Primitivevector3 ey_;				/*!< y-axis unit vector */
	Primitivevector3 ez_;				/*!< z-axis unit vector */
};
