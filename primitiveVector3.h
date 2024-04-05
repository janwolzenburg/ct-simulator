#pragma once
/*********************************************************************
 * @file   primitiveVector3.h
 * @brief  class for a primitive 3D vector
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


  /*********************************************************************
	Includes
 *********************************************************************/
 #include "generel.h"


  /*********************************************************************
	Definitions
 *********************************************************************/



/*!
 * @brief class for a primitive 3D vector with some operations
*/
class PrimitiveVector3 : public Tuple3D, public MathematicalObject{

	public:

	/*!
	 * @brief constructor
	 * @param x x value
	 * @param y y value
	 * @param z z value
	*/
	PrimitiveVector3( const double x, const double y, const double z ) : Tuple3D{ x, y, z } {};

	/*!
	* @brief constructor
	* @param xyz x, y and z values
	*/
	PrimitiveVector3( const Tuple3D xyz ) : Tuple3D{ xyz } {};

	/*!
	 * @brief default constructor
	*/
	PrimitiveVector3( void ) : PrimitiveVector3{ 0., 0., 0. } {};

	/*!
	 * @brief constructor from serialized data
	 * @param binary_data reference to vector with binary data
	 * @param current_byte iterator to start of data in vector
	*/
	PrimitiveVector3( const vector<char>& binary_data, vector<char>::const_iterator& current_byte ) : Tuple3D{ binary_data, current_byte }{};

	/*!
	 * @brief convert data to string
	 * @param newline_tabulators tabulator to insert 
	 * @return string with data
	*/
	string ConvertToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief comparison operator
	 * @param vector_to_compare vector to compare with
	 * @return true when all components between this and second vector are almost equal
	*/
	bool operator== ( const PrimitiveVector3 vector_to_compare ) const;

	/*!
	 * @brief add second vector to this
	 * @param summand vector to add
	 * @return sum of this and summand
	*/
	PrimitiveVector3 operator+ ( const PrimitiveVector3 summand ) const{ return PrimitiveVector3{ x + summand.x, y + summand.y, z + summand.z }; };

	/*!
	 * @brief substract second vector to this
	 * @param subtrahend vector to substract
	 * @return difference of this and v
	*/
	PrimitiveVector3 operator- ( const PrimitiveVector3 subtrahend ) const{ return PrimitiveVector3{ x - subtrahend.x, y - subtrahend.y, z - subtrahend.z }; };

	/*!
	 * @brief negation operator
	 * @return negated vector
	*/
	PrimitiveVector3 operator- ( void ) const{ return PrimitiveVector3{ -x, -y, -z }; };

	/*!
	 * @brief scale this vector
	 * @param scalar factor to scale with
	 * @return scaled vector
	*/
	PrimitiveVector3 operator* ( const double scalar ) const{ return PrimitiveVector3{ scalar * x, scalar * y, scalar * z }; };

	/*!
	 * @brief scalar product
	 * @param second_vector second vector
	 * @return scalar product of vectors
	*/
	double operator* ( const PrimitiveVector3 second_vector ) const{ return this->x * second_vector.x + this->y * second_vector.y + this->z * second_vector.z; };

	/*!
	 * @brief divide this vector
	 * @param divisor value to divide by
	 * @return divided vector
	*/
	PrimitiveVector3 operator/ ( const double divisor ) const;

	/*!
	 * @brief get length of primitive vector
	 * @return length
	*/
	double GetLength( void ) const;

	/*!
	 * @brief scale this vector
	 * @param scalar factor to scale by
	*/
	void Scale( const double scalar );

	/*!
	 * @brief normalise values of components
	 * @return error code
	*/
	MathError Normalise( void );

	/*!
	 * @brief add value to x component
	 * @param x_ value to add
	*/
	void AddToX( const double x_ ){ x += x_; };

	/*!
	 * @brief add value to y component
	 * @param y_ value to add
	*/
	void AddToY( const double y_ ){ y += y_; };

	/*!
	 * @brief add value to z component
	 * @param z_ value to add
	*/
	void AddToZ( const double z_ ){ z += z_; };

	/*!
	 * @brief rotate the point of all three components around x-axis of coordinate system
	 * @param sine_phi sine of the angle
	 * @param cosine_phi cosine of the angle
	*/
	void RotateAroundXAxis( const double sine_phi, const double cosine_phi );

	/*!
	 * @brief rotate the point of all three components around y-axis of coordinate system
	 * @param sine_phi sine of the angle
	 * @param cosine_phi cosine of the angle
	*/
	void RotateAroundYAxis( const double sine_phi, const double cosine_phi );

	/*!
	 * @brief rotate the point of all three components around z-axis of coordinate system
	 * @param sine_phi sine of the angle
	 * @param cosine_phi cosine of the angle
	*/
	void RotateAroundZAxis( const double sine_phi, const double cosine_phi );

	/*!
	 * @brief rotate point that these three components form around a vector the components of n form
	 * @param axis Rotation axis
	 * @param angle angle
	 * @return error code
	*/
	MathError Rotate( const PrimitiveVector3 axis, const double angle );

};