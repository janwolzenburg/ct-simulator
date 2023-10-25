#pragma once
/*********************************************************************
 * @file   vec3D.h
 * @brief  Classes for vectors, unit vectors and points
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/
#include "coordinates.h"
#include "cSysTree.h"



 /*********************************************************************
	Definitions
 *********************************************************************/

/*!
 * @brief Class for vectors in three dimensional cartesian coordinate systems
*/
class vec3 : protected coordinates{

	private:
	double len;				/*!< Length */


	public:

	/*!
	 * @brief Constructor
	 * @param coords Coordinates of vector
	*/
	vec3( const coordinates coords );

	/*!
	 * @brief Constructor
	 * @param xyz_ Components of vector
	 * @param cSys_ Coordinate system of components
	*/
	vec3( const v3 xyz_, const cartCSys* const cSys_ );

	/*!
	 * @brief Default constructor initializing components to zero coordinate system to global
	*/
	vec3( void );

	/*!
	 * @brief Convert vector's data to string
	 * @return String with vector's data
	*/
	string toStr( const unsigned int newLineTabulators = 0 ) const override;

	/*!
	 * @brief Comparison operator
	 * @param v2 Vector to compare with
	 * @return True when all components are equal
	*/
	bool operator== ( const vec3 v2 ) const{ return this->coordinates::operator==( v2 ); };

	/*!
	 * @brief Addition operator
	 * @param v2 Vector to add
	 * @return Sum of vectors in unit of this vector
	*/
	vec3 operator+ ( const vec3 v2 ) const{ return this->coordinates::operator+( v2 ); };

	/*!
	 * @brief Substraction operator
	 * @param v2 Vector to substract
	 * @return Difference (v1 - v2) of vectors in unit of this vector
	*/
	vec3 operator- ( const vec3 v2 ) const{ return this->coordinates::operator-( v2 ); };

	/*!
	 * @brief Negation operator
	 * @return Negated vector
	*/
	vec3 operator- ( void ) const{ return this->coordinates::operator-(); };

	/*!
	 * @brief Scale vector
	 * @param scalar Factor
	 * @return Scaled vector
	*/
	vec3 operator* ( const double scalar ) const{ return this->coordinates::operator*( scalar ); };

	/*!
	 * @brief Divide vector
	 * @param divisor Divisor
	 * @return Divided vector
	*/
	vec3 operator/ ( const double divisor ) const{ return this->coordinates::operator/( divisor ); };

	/*!
	 * @brief Calculate dot product between to vectors
	 * @param v2 Second vector
	 * @return Dot product in unit of this vector squared
	*/
	double operator* ( const vec3 v2 ) const;

	/*!
	 * @brief Calculate cross product
	 * @param v2 Second vector
	 * @return Cross product this * v2 in unit of this vector
	*/
	vec3 operator^( const vec3 v2 ) const;

	/*!
	 * @brief Get x component
	 * @return x component in local coordinate system
	*/
	double X( void ) const{ return x; };

	/*!
	 * @brief Get y component
	 * @return y component in local coordinate system
	*/
	double Y( void ) const{ return y; };

	/*!
	 * @brief Get z component
	 * @return z component in local coordinate system
	*/
	double Z( void ) const{ return z; };

	/*!
	 * @brief Get length of vector
	 * @return Length of vector in local coordinate system
	*/
	double Length( void ) const{ return len; };

	/*!
	 * @brief Get Pointer to coordinate system
	 * @return Pointer to this vector's coordinate system
	*/
	const cartCSys* CSys( void ) const{ return cSys; };

	/*!
	 * @brief Check if vector's are defined in same system
	 * @param v Second vector
	 * @return true when this and vector v are defined in the same coordinate system
	*/
	bool sameSystem( const vec3 v ) const;

	/*!
	 * @brief Convert vector to different coordinate system
	 * @param target_cSys System to convert to
	 * @return Converted vector
	*/
	vec3 convertTo( const cartCSys* const target_cSys ) const;

	/*!
	 * @brief Convert vector to different coordinate system
	 * @param v Vector to which system this gets converted
	 * @return Converted vector
	*/
	vec3 convertTo( const vec3 v ) const;

	/*!
	 * @brief Convert vector to different coordinate system
	 * @param l Line to which system this gets converted
	 * @return Converted vector
	*/
	vec3 convertTo( const line l ) const;

	/*!
	 * @brief Convert vector to different coordinate system
	 * @param s Surface to which system this gets converted
	 * @return Converted vector
	*/
	vec3 convertTo( const surf s ) const;

	/*!
	 * @brief Get x,y and z component
	 * @return x, y and z component in local coordinate system
	*/
	primitiveVec3 XYZ( void ) const{ return (primitiveVec3) *this; };

	/*!
	 * @brief Get x,y and z component
	 * @param target Coordinate system to convert components to
	 * @return x, y and z component in local coordinate system
	*/
	primitiveVec3 XYZ( const cartCSys* const target ) const;

	/*!
	 * @brief Get x,y and z component
	 * @param targetV Vector with coordinate system to convert components to
	 * @return x, y and z component in local coordinate system
	*/
	primitiveVec3 XYZ( const vec3 targetV ) const;

	/*!
	 * @brief Get global x,y and z component
	 * @return x, y and z component in global coordinate system
	*/
	primitiveVec3 gXYZ( void ) const;

	/*!
	 * @brief Get global x component
	 * @return x component in global coordinate system
	*/
	double gX( void ) const;

	/*!
	 * @brief Get global y component
	 * @return y component in global coordinate system
	*/
	double gY( void ) const;

	/*!
	 * @brief Get global z component
	 * @return z component in global coordinate system
	*/
	double gZ( void ) const;

	/*!
	 * @brief Calculates vector's length corresponding to vector component and stores it in member
	 * @return Error code
	*/
	MATH_ERR updateLength( void );

	/*!
	 * @brief Scale vector by factor
	 * @param scalar Factor
	 * @return Error code
	*/
	MATH_ERR scale( const double scalar );

	/*!
	 * @brief Normalize vector's length to one
	 * @return Error code
	 * @details Error code ist set when vector's length is zero
	*/
	MATH_ERR normalise( void );

	/*!
	 * @brief Add value to x component of this vector
	 * @param x_ Value to add in vector's unit
	 * @return Error code
	*/
	MATH_ERR addXM( const double x_ );

	/*!
	 * @brief Add value to y component of this vector
	 * @param y_ Value to add in vector's unit
	 * @return Error code
	*/
	MATH_ERR addYM( const double y_ );

	/*!
	 * @brief Add value to z component of this vector
	 * @param z_ Value to add in vector's unit
	 * @return Error code
	*/
	MATH_ERR addZM( const double z_ );

	/*!
	 * @brief Add value to x component
	 * @param x_ Value to add in vector's unit
	 * @return Vector with added value
	*/
	vec3 addX( const double x_ ) const;

	/*!
	 * @brief Add value to y component
	 * @param y_ Value to add in vector's unit
	 * @return Vector with added value
	*/
	vec3 addY( const double y_ ) const;

	/*!
	 * @brief Add value to z component
	 * @param z_ Value to add in vector's unit
	 * @return Vector with added value
	*/
	vec3 addZ( const double z_ ) const;

	/*!
	 * @brief Calculate angle between this and another vector
	 * @param v2 Second vector
	 * @return Angle in radians
	*/
	double angle( const vec3 v2 ) const;

	/*!
	 * @brief Checks whether two vectors are orthogonal
	 * @param v2 Second vector
	 * @return True when vectors are orthogonal
	*/
	bool isOrtho( const vec3 v2 ) const;

	/*!
	 * @brief Counterclockwise rotation of this vector around x-axis
	 * @param sinPhi Sine of the angle
	 * @param cosPhi Cosine of the angle
	 * @return Error code
	*/
	MATH_ERR rotXM( const double sinPhi, const double cosPhi );

	/*!
	 * @brief Counterclockwise rotation of this vector around y-axis
	 * @param sinPhi Sine of the angle
	 * @param cosPhi Cosine of the angle
	 * @return Error code
	*/
	MATH_ERR rotYM( const double sinPhi, const double cosPhi );

	/*!
	 * @brief Counterclockwise rotation of this vector around z-axis
	 * @param sinPhi Sine of the angle
	 * @param cosPhi Cosine of the angle
	 * @return Error code
	*/
	MATH_ERR rotZM( const double sinPhi, const double cosPhi );

	/*!
	 * @brief Counterclockwise rotation of this vector around x-axis
	 * @param phi Angle in radians
	 * @return Error code
	*/
	MATH_ERR rotXM( const double phi );

	/*!
	 * @brief Counterclockwise rotation of this vector around y-axis
	 * @param phi Angle in radians
	 * @return Error code
	*/
	MATH_ERR rotYM( const double phi );

	/*!
	 * @brief Counterclockwise rotation of this vector around z-axis
	 * @param phi Angle in radians
	 * @return Error code
	*/
	MATH_ERR rotZM( const double phi );

	/*!
	 * @brief Counterclockwise rotation of vector around x-axis
	 * @param phi Angle in radians
	 * @return Rotated vector
	*/
	vec3 rotX( const double phi ) const;

	/*!
	 * @brief Counterclockwise rotation of vector around y-axis
	 * @param phi Angle in radians
	 * @return Rotated vector
	*/
	vec3 rotY( const double phi ) const;

	/*!
	 * @brief Counterclockwise rotation of vector around z-axis
	 * @param phi Angle in radians
	 * @return Rotated vector
	*/
	vec3 rotZ( const double phi ) const;

	/*!
	 * @brief Counterclockwise rotation of this vector around second vector
	 * @param n Rotation axis vector
	 * @param phi Angle in radians
	 * @return Error code
	*/
	MATH_ERR rotNM( const vec3 n, const double phi );

	/*!
	 * @brief Counterclockwise rotation of vector around second vector
	 * @param n Rotation axis vector
	 * @param phi Angle in radians
	 * @return Rotated vector
	*/
	vec3 rotN( const vec3 n, const double phi ) const;

	/*!
	 * @brief Project vector on XY plane of given coordinate system
	 * @param cSys Coordinate system
	 * @return Projected vector
	*/
	vec3 projectOnXYPlane( const cartCSys* const cSys ) const;

	/*!
	 * @brief Flip x-values
	 * @return Vector with negated x values
	*/
	vec3 negateX( void ) const;

};



/*!
 * @brief Class for unit vectors with length one
*/
class uvec3 : public vec3{

	public:

	/*!
	 * @brief Converting constructor from vector
	 * @param v Vector to convert
	*/
	uvec3( const vec3 v ) : vec3( v ){
		normalise();
	};

	/*!
	 * @brief Constructor
	 * @param xyz_ Components
	 * @param cSys_ Coordinate system
	*/
	uvec3( const v3 xyz_, const cartCSys* const cSys_ ) : uvec3{ vec3{ xyz_, cSys_ } } {};

	/*!
	 * @brief Default constructor
	*/
	uvec3( void ) : uvec3{ v3{ 1, 0, 0 }, DUMMY_CSYS() } {};

	/*!
	 * @brief Scaling unit vector does not have an effect
	 * @param scalar
	 * @return
	*/
	MATH_ERR scale( const double scalar ) = delete;

	/*!
	 * @brief Add value to x component of this vector and normalise again
	 * @param x_ Value to add in vector's unit
	 * @return Error code
	*/
	MATH_ERR addXM( const double x_ ) = delete;

	/*!
	 * @brief Add value to y component of this vector and normalise again
	 * @param y_ Value to add in vector's unit
	 * @return Error code
	*/
	MATH_ERR addYM( const double y_ ) = delete;

	/*!
	 * @brief Add value to z component of this vector and normalise again
	 * @param z_ Value to add in vector's unit
	 * @return Error code
	*/
	MATH_ERR addZM( const double z_ ) = delete;
};



/*!
 * @brief Class for points in cartesian coordinate system
*/
class pnt3 : public vec3{

	using vec3::vec3;

	public:

	/*!
	 * @brief Converting constructor
	 * @param v Vector to convert from
	*/
	pnt3( const vec3 v ) : vec3( v ){};

	/*!
	 * @brief Convert point's data to string
	 * @return String with point's data
	*/
	string toStr( const unsigned int newLineTabulators = 0 ) const override;

	/*!
	 * @brief Convert point to different coordinate system
	 * @param target_cSys Target system
	 * @return Converted point
	*/
	pnt3 convertTo( const cartCSys* const target_cSys ) const;

	/*!
	 * @brief Convert point to different coordinate system
	 * @param targetP Point with target system
	 * @return Converted point
	*/
	pnt3 convertTo( const pnt3 targetP ) const;

	/*!
	 * @brief Get x,y and z component
	 * @return x, y and z component in local coordinate system
	*/
	primitiveVec3 XYZ( void ) const{ return vec3::XYZ(); };

	/*!
	 * @brief Get x,y and z component
	 * @param target Coordinate system to convert components to
	 * @return x, y and z component in local coordinate system
	*/
	primitiveVec3 XYZ( const cartCSys* const target ) const;

	/*!
	 * @brief Get x,y and z component
	 * @param targetV Point with coordinate system to convert components to
	 * @return x, y and z component in local coordinate system
	*/
	primitiveVec3 XYZ( const pnt3 targetP ) const;

	/*!
	 * @brief Get global x component
	 * @return x component in global coordinate system
	*/
	double gX( void ) const;

	/*!
	 * @brief Get global y component
	 * @return y component in global coordinate system
	*/
	double gY( void ) const;

	/*!
	 * @brief Get global z component
	 * @return z component in global coordinate system
	*/
	double gZ( void ) const;

	/*!
	 * @brief Get global x,y and z component
	 * @return x, y and z component in global coordinate system
	*/
	primitiveVec3 gXYZ( void ) const;

	/*!
	 * @brief Project point on XY plane of given coordinate system
	 * @param cSys Coordinate system
	 * @return Projected point
	*/
	pnt3 projectOnXYPlane( const cartCSys* const cSys ) const;
};