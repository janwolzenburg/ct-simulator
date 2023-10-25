#pragma once
/*********************************************************************
 * @file   primitiveVec3.h
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
 * @brief Class for a primitve 3D Vector with some operations
*/
class primitiveVec3 : public v3, public mathObj{

	public:

	/*!
	 * @brief Constructor
	 * @param x_ x value
	 * @param y_ y value
	 * @param z_ z value
	*/
	primitiveVec3( const double x, const double y, const double z ) : v3( x, y, z ) {};

	/*!
	* @brief Constructor
	* @param xyz_ x, y and z values
	*/
	primitiveVec3( const v3 xyz ) : v3( xyz ) {};

	/*!
	 * @brief Default constructor
	*/
	primitiveVec3( void ) : primitiveVec3{ 0., 0., 0. } {};

	/*!
	 * @brief Constructor from serialized data
	 * @param binData Reference to vector with binary data
	 * @param it Iterator to start of data in vector
	*/
	primitiveVec3( const vector<char>& binData, vector<char>::const_iterator& it );

	/*!
	 * @brief Convert data to string
	 * @param newLineTabulators
	 * @return String with data
	*/
	string toStr( const unsigned int newLineTabulators = 0 ) const override;

	/*!
	 * @brief Comparison operator
	 * @param v Vector to compare with
	 * @return True when all components between this and second vector are almost equal
	*/
	bool operator== ( const primitiveVec3 v ) const;

	/*!
	 * @brief Negated comparison operator
	 * @param v Vector to compare with
	 * @return False when all components between this and second vector are almost equal
	*/
	bool operator!= ( const primitiveVec3 v ) const;

	/*!
	 * @brief Add second vector to this
	 * @param v Vector to add
	 * @return Sum of this and v
	*/
	primitiveVec3 operator+ ( const primitiveVec3 v ) const{ return primitiveVec3{ x + v.x, y + v.y, z + v.z }; };

	/*!
	 * @brief Substract second vector to this
	 * @param v Vector to substract
	 * @return Difference of this and v
	*/
	primitiveVec3 operator- ( const primitiveVec3 v ) const{ return primitiveVec3{ x - v.x, y - v.y, z - v.z }; };

	/*!
	 * @brief Negation operator
	 * @return Negated vector
	*/
	primitiveVec3 operator- ( void ) const{ return primitiveVec3{ -x, -y, -z }; };

	/*!
	 * @brief Scale this vector
	 * @param scalar Factor to scale with
	 * @return Scaled vector
	*/
	primitiveVec3 operator* ( const double scalar ) const{ return primitiveVec3{ scalar * x, scalar * y, scalar * z }; };

	/*!
	 * @brief Scalar product
	 * @param v Second vector
	 * @return Scalar product of vectors
	*/
	double operator* ( const primitiveVec3 v ) const{ return this->x * v.x + this->y * v.y + this->z * v.z; };

	/*!
	 * @brief Divide this vector
	 * @param divisor Value to divide by
	 * @return Divided vector
	*/
	primitiveVec3 operator/ ( const double divisor ) const;

	/*!
	 * @brief Get length of primitve vector
	 * @return Length
	*/
	double Length( void ) const;

	/*!
	 * @brief Scale this vector
	 * @param scalar Factor to scale by
	*/
	void scale( const double scalar );

	/*!
	 * @brief Normalize values of components
	 * @return Error code
	*/
	MATH_ERR normalise( void );

	/*!
	 * @brief Add value to x component
	 * @param x_ Value to add
	*/
	void addXM( const double x_ ){ x += x_; };

	/*!
	 * @brief Add value to y component
	 * @param y_ Value to add
	*/
	void addYM( const double y_ ){ y += y_; };

	/*!
	 * @brief Add value to z component
	 * @param z_ Value to add
	*/
	void addZM( const double z_ ){ z += z_; };

	/*!
	 * @brief Rotate the point of all three components around x-axis of coordinate system
	 * @param sinPhi Sine of the angle
	 * @param cosPhi Cosine of the angle
	*/
	void rotXM( const double sinPhi, const double cosPhi );

	/*!
	 * @brief Rotate the point of all three components around y-axis of coordinate system
	 * @param sinPhi Sine of the angle
	 * @param cosPhi Cosine of the angle
	*/
	void rotYM( const double sinPhi, const double cosPhi );

	/*!
	 * @brief Rotate the point of all three components around z-axis of coordinate system
	 * @param sinPhi Sine of the angle
	 * @param cosPhi Cosine of the angle
	*/
	void rotZM( const double sinPhi, const double cosPhi );

	/*!
	 * @brief Rotate point that these three components form around a vector the components of n form
	 * @param n Rotation axis
	 * @param phi Angle
	 * @return Error code
	*/
	MATH_ERR rotNM( const primitiveVec3 n, const double phi );

};