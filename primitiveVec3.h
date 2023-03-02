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
 #include "generelMath.h"



  /*********************************************************************
	Definitions
 *********************************************************************/



/*!
 * @brief Class for a primitve 3D Vector with some operations
*/
class primitiveVec3 : public v3, virtual public mathObj{

	public:

	/*!
	 * @brief Default constructor
	*/
	primitiveVec3( void );

	/*!
	 * @brief Constructor
	 * @param xyz_ x, y and z values 
	*/
	primitiveVec3( const v3 xyz );

	/*!
	 * @brief Constructor
	 * @param x_ x value
	 * @param y_ y value
	 * @param z_ z value
	*/
	primitiveVec3( const double x, const double y, const double z );

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
	primitiveVec3 operator+ ( const primitiveVec3 v ) const;

	/*!
	 * @brief Substract second vector to this
	 * @param v Vector to substract
	 * @return Difference of this and v
	*/
	primitiveVec3 operator- ( const primitiveVec3 v ) const;

	/*!
	 * @brief Negation operator
	 * @return Negated vector
	*/
	primitiveVec3 operator- ( void ) const;

	/*!
	 * @brief Scale this vector
	 * @param scalar Factor to scale with
	 * @return Scaled vector
	*/
	primitiveVec3 operator* ( const double scalar ) const;

	/*!
	 * @brief Scalar product
	 * @param v Second vector
	 * @return Scalar product of vectors
	*/
	double operator* ( const primitiveVec3 v ) const;

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
	MATH_ERR normalize( void );

	/*!
	 * @brief Add value to x component
	 * @param x_ Value to add
	*/
	inline void addXM( const double x_ ){ x += x_; };

	/*!
	 * @brief Add value to y component
	 * @param y_ Value to add
	*/
	inline void addYM( const double y_ ){ y += y_; };

	/*!
	 * @brief Add value to z component
	 * @param z_ Value to add
	*/
	inline void addZM( const double z_ ){ z += z_; };

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