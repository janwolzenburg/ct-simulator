#pragma once
/*********************************************************************
 * @file   PrimitiveVector3.h
 * @brief  Class for a primitve 3D vector
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
 * @brief Class for a primitive 3D Vector with some operations
*/
class PrimitiveVector3 : public Tuple3D, public MathematicalObject{

	public:

	/*!
	 * @brief Constructor
	 * @param x_ x value
	 * @param y_ y value
	 * @param z_ z value
	*/
	PrimitiveVector3( const double x, const double y, const double z ) : Tuple3D{ x, y, z } {};

	/*!
	* @brief Constructor
	* @param xyz_ x, y and z values
	*/
	PrimitiveVector3( const Tuple3D xyz ) : Tuple3D{ xyz } {};

	/*!
	 * @brief Default constructor
	*/
	PrimitiveVector3( void ) : PrimitiveVector3{ 0., 0., 0. } {};

	/*!
	 * @brief Constructor from serialized data
	 * @param binary_data Reference to vector with binary data
	 * @param it Iterator to start of data in vector
	*/
	PrimitiveVector3( const vector<char>& binary_data, vector<char>::const_iterator& it ) : Tuple3D{ binary_data, it }{};

	/*!
	 * @brief Convert data to string
	 * @param newline_tabulators
	 * @return String with data
	*/
	string ToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief Comparison operator
	 * @param v Vector to compare with
	 * @return True when all components between this and second vector are almost equal
	*/
	bool operator== ( const PrimitiveVector3 v ) const;

	/*!
	 * @brief Add second vector to this
	 * @param v Vector to add
	 * @return Sum of this and v
	*/
	PrimitiveVector3 operator+ ( const PrimitiveVector3 v ) const{ return PrimitiveVector3{ x + v.x, y + v.y, z + v.z }; };

	/*!
	 * @brief Substract second vector to this
	 * @param v Vector to substract
	 * @return Difference of this and v
	*/
	PrimitiveVector3 operator- ( const PrimitiveVector3 v ) const{ return PrimitiveVector3{ x - v.x, y - v.y, z - v.z }; };

	/*!
	 * @brief Negation operator
	 * @return Negated vector
	*/
	PrimitiveVector3 operator- ( void ) const{ return PrimitiveVector3{ -x, -y, -z }; };

	/*!
	 * @brief Scale this vector
	 * @param scalar Factor to scale with
	 * @return Scaled vector
	*/
	PrimitiveVector3 operator* ( const double scalar ) const{ return PrimitiveVector3{ scalar * x, scalar * y, scalar * z }; };

	/*!
	 * @brief Scalar product
	 * @param v Second vector
	 * @return Scalar product of vectors
	*/
	double operator* ( const PrimitiveVector3 v ) const{ return this->x * v.x + this->y * v.y + this->z * v.z; };

	/*!
	 * @brief Divide this vector
	 * @param divisor Value to divide by
	 * @return Divided vector
	*/
	PrimitiveVector3 operator/ ( const double divisor ) const;

	/*!
	 * @brief Get length_ of primitve vector
	 * @return Length
	*/
	double Length( void ) const;

	/*!
	 * @brief Scale this vector
	 * @param scalar Factor to scale by
	*/
	void Scale( const double scalar );

	/*!
	 * @brief Normalise values of components
	 * @return Error code
	*/
	MathError Normalise( void );

	/*!
	 * @brief Add value to x component
	 * @param x_ Value to add
	*/
	void AddToX( const double x_ ){ x += x_; };

	/*!
	 * @brief Add value to y component
	 * @param y_ Value to add
	*/
	void AddToY( const double y_ ){ y += y_; };

	/*!
	 * @brief Add value to z component
	 * @param z_ Value to add
	*/
	void AddToZ( const double z_ ){ z += z_; };

	/*!
	 * @brief Rotate the point of all three components around x-axis of coordinate system
	 * @param sinPhi Sine of the GetAngle
	 * @param cosPhi Cosine of the GetAngle
	*/
	void RotateAroundXAxis( const double sinPhi, const double cosPhi );

	/*!
	 * @brief Rotate the point of all three components around y-axis of coordinate system
	 * @param sinPhi Sine of the GetAngle
	 * @param cosPhi Cosine of the GetAngle
	*/
	void RotateAroundYAxis( const double sinPhi, const double cosPhi );

	/*!
	 * @brief Rotate the point of all three components around z-axis of coordinate system
	 * @param sinPhi Sine of the GetAngle
	 * @param cosPhi Cosine of the GetAngle
	*/
	void RotateAroundZAxis( const double sinPhi, const double cosPhi );

	/*!
	 * @brief Rotate point that these three components form around a vector the components of n form
	 * @param axis Rotation axis
	 * @param GetAngle Angle
	 * @return Error code
	*/
	MathError Rotate( const PrimitiveVector3 axis, const double angle );

};