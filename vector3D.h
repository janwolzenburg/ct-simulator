#pragma once
/*********************************************************************
 * @file   vector3D.h
 * @brief  Classes for vectors, unit vectors and points
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/
#include "coordinates.h"
#include "coordinateSystemTree.h"



 /*********************************************************************
	Definitions
 *********************************************************************/

/*!
 * @brief Class for vectors in three dimensional cartesian coordinate systems
*/
class Vector3D : protected Coordinates{

	public:

	/*!
	 * @brief Constructor
	 * @param coords Coordinates of vector
	*/
	Vector3D( const Coordinates coords );

	/*!
	 * @brief Constructor
	 * @param xyz_ Components of vector
	 * @param coordinate_system Coordinate system of components
	*/
	Vector3D( const Tuple3D xyz_, const CoordinateSystem* const coordinate_system );

	/*!
	 * @brief Default constructor initializing components to zero coordinate system to global
	*/
	Vector3D( void );

	/*!
	 * @brief Convert vector's data to string
	 * @return String with vector's data
	*/
	string ToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief Comparison operator
	 * @param v2 Vector to compare with
	 * @return True when all components are equal
	*/
	bool operator== ( const Vector3D v2 ) const{ return this->Coordinates::operator==( v2 ); };

	/*!
	 * @brief Addition operator
	 * @param v2 Vector to add
	 * @return Sum of vectors in unit of this vector
	*/
	Vector3D operator+ ( const Vector3D v2 ) const{ return this->Coordinates::operator+( v2 ); };

	/*!
	 * @brief Substraction operator
	 * @param v2 Vector to substract
	 * @return Difference (v1 - v2) of vectors in unit of this vector
	*/
	Vector3D operator- ( const Vector3D v2 ) const{ return this->Coordinates::operator-( v2 ); };

	/*!
	 * @brief Negation operator
	 * @return Negated vector
	*/
	Vector3D operator- ( void ) const{ return this->Coordinates::operator-(); };

	/*!
	 * @brief Scale vector
	 * @param scalar Factor
	 * @return Scaled vector
	*/
	Vector3D operator* ( const double scalar ) const{ return this->Coordinates::operator*( scalar ); };

	/*!
	 * @brief Divide vector
	 * @param divisor Divisor
	 * @return Divided vector
	*/
	Vector3D operator/ ( const double divisor ) const{ return this->Coordinates::operator/( divisor ); };

	/*!
	 * @brief Calculate dot product between to vectors
	 * @param v2 Second vector
	 * @return Dot product in unit of this vector squared
	*/
	double operator* ( const Vector3D v2 ) const;

	/*!
	 * @brief Calculate cross product
	 * @param v2 Second vector
	 * @return Cross product this * v2 in unit of this vector
	*/
	Vector3D operator^( const Vector3D v2 ) const;

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
	 * @brief Get length_ of vector
	 * @return Length of vector in local coordinate system
	*/
	double Length( void ) const{ return length_; };

	/*!
	 * @brief Get Pointer to coordinate system
	 * @return Pointer to this vector's coordinate system
	*/
	const CoordinateSystem* GetCoordinateSystem( void ) const{ return coordinate_system_; };

	/*!
	 * @brief Check if vector's are defined in same system
	 * @param v Second vector
	 * @return true when this and vector v are defined in the same coordinate system
	*/
	bool IsSameSystem( const Vector3D v ) const;

	/*!
	 * @brief Convert vector to different coordinate system
	 * @param target_coordinate_system System to convert to
	 * @return Converted vector
	*/
	Vector3D ConvertTo( const CoordinateSystem* const target_coordinate_system ) const;

	/*!
	 * @brief Convert vector to different coordinate system
	 * @param v Vector to which system this gets converted
	 * @return Converted vector
	*/
	Vector3D ConvertTo( const Vector3D v ) const;

	/*!
	 * @brief Convert vector to different coordinate system
	 * @param l Line to which system this gets converted
	 * @return Converted vector
	*/
	Vector3D ConvertTo( const line l ) const;

	/*!
	 * @brief Convert vector to different coordinate system
	 * @param s Surface to which system this gets converted
	 * @return Converted vector
	*/
	Vector3D ConvertTo( const surf s ) const;

	/*!
	 * @brief Get x,y and z component
	 * @return x, y and z component in local coordinate system
	*/
	PrimitiveVector3 Components( void ) const{ return static_cast<PrimitiveVector3>( *this ); };

	/*!
	 * @brief Get x,y and z component
	 * @param target Coordinate system to convert components to
	 * @return x, y and z component in local coordinate system
	*/
	PrimitiveVector3 Components( const CoordinateSystem* const target ) const;

	/*!
	 * @brief Get x,y and z component
	 * @param targetV Vector with coordinate system to convert components to
	 * @return x, y and z component in local coordinate system
	*/
	PrimitiveVector3 Components( const Vector3D targetV ) const;

	/*!
	 * @brief Get global x,y and z component
	 * @return x, y and z component in global coordinate system
	*/
	PrimitiveVector3 GlobalComponents( void ) const;

	/*!
	 * @brief Get global x component
	 * @return x component in global coordinate system
	*/
	double GlobalX( void ) const;

	/*!
	 * @brief Get global y component
	 * @return y component in global coordinate system
	*/
	double GlobalY( void ) const;

	/*!
	 * @brief Get global z component
	 * @return z component in global coordinate system
	*/
	double GlobalZ( void ) const;


	/*!
	 * @brief Scale vector by factor
	 * @param scalar Factor
	 * @return Error code
	*/
	MathError Scale( const double scalar );

	/*!
	 * @brief Normalise vector's length_ to one
	 * @return Error code
	 * @details Error code ist set when vector's length_ is zero
	*/
	MathError Normalise( void );

	/*!
	 * @brief Add value to x component of this vector
	 * @param x_ Value to add in vector's unit
	 * @return Error code
	*/
	MathError AddToX( const double x_ );

	/*!
	 * @brief Add value to y component of this vector
	 * @param y_ Value to add in vector's unit
	 * @return Error code
	*/
	MathError AddToY( const double y_ );

	/*!
	 * @brief Add value to z component of this vector
	 * @param z_ Value to add in vector's unit
	 * @return Error code
	*/
	MathError AddToZ( const double z_ );

	/*!
	 * @brief Calculate GetAngle between this and another vector
	 * @param v2 Second vector
	 * @return Angle in radians
	*/
	double GetAngle( const Vector3D v2 ) const;

	/*!
	 * @brief Checks whether two vectors are orthogonal
	 * @param v2 Second vector
	 * @return True when vectors are orthogonal
	*/
	bool IsOrthogonal( const Vector3D v2 ) const;

	/*!
	 * @brief Counterclockwise rotation of this vector around x-axis
	 * @param sinPhi Sine of the GetAngle
	 * @param cosPhi Cosine of the GetAngle
	 * @return Error code
	*/
	MathError RotateAroundXAxis( const double sinPhi, const double cosPhi );

	/*!
	 * @brief Counterclockwise rotation of this vector around y-axis
	 * @param sinPhi Sine of the GetAngle
	 * @param cosPhi Cosine of the GetAngle
	 * @return Error code
	*/
	MathError RotateAroundYAxis( const double sinPhi, const double cosPhi );

	/*!
	 * @brief Counterclockwise rotation of this vector around z-axis
	 * @param sinPhi Sine of the GetAngle
	 * @param cosPhi Cosine of the GetAngle
	 * @return Error code
	*/
	MathError RotateAroundZAxis( const double sinPhi, const double cosPhi );

	/*!
	 * @brief Counterclockwise rotation of this vector around x-axis
	 * @param phi Angle in radians
	 * @return Error code
	*/
	MathError RotateAroundXAxis( const double phi );

	/*!
	 * @brief Counterclockwise rotation of this vector around y-axis
	 * @param phi Angle in radians
	 * @return Error code
	*/
	MathError RotateAroundYAxis( const double phi );

	/*!
	 * @brief Counterclockwise rotation of this vector around z-axis
	 * @param phi Angle in radians
	 * @return Error code
	*/
	MathError RotateAroundZAxis( const double phi );

	/*!
	 * @brief Counterclockwise rotation of this vector around second vector
	 * @param n Rotation axis vector
	 * @param phi Angle in radians
	 * @return Error code
	*/
	MathError Rotate( const Vector3D n, const double phi );

	/*!
	 * @brief Counterclockwise rotation of vector around second vector
	 * @param n Rotation axis vector
	 * @param phi Angle in radians
	 * @return Rotated vector
	*/
	Vector3D RotateConstant( const Vector3D n, const double phi ) const;

	/*!
	 * @brief Project vector on XY plane of given coordinate system
	 * @param coordinate_system_ Coordinate system
	 * @return Projected vector
	*/
	Vector3D ProjectOnXYPlane( const CoordinateSystem* const cSys ) const;

	/*!
	 * @brief Flip x-values
	 * @return Vector with negated x values
	*/
	Vector3D NegateXComponent( void ) const;


	private:
	double length_;				/*!< Length */

	/*!
	 * @brief Calculates vector's length_ corresponding to vector component and stores it in member
	 * @return Error code
	*/
	MathError UpdateLength( void );

};



/*!
 * @brief Class for unit vectors with length_ one
*/
class UnitVector3D : public Vector3D{

	public:

	/*!
	 * @brief Converting constructor from vector
	 * @param v Vector to convert
	*/
	UnitVector3D( const Vector3D v ) : Vector3D( v ){
		Normalise();
	};

	/*!
	 * @brief Constructor
	 * @param xyz_ Components
	 * @param coordinate_system Coordinate system
	*/
	UnitVector3D( const Tuple3D xyz_, const CoordinateSystem* const coordinate_system ) : UnitVector3D{ Vector3D{ xyz_, coordinate_system } } {};

	/*!
	 * @brief Default constructor
	*/
	UnitVector3D( void ) : UnitVector3D{ Tuple3D{ 1, 0, 0 }, DummySystem() } {};

	/*!
	 * @brief Scaling unit vector does not have an effect
	 * @param scalar
	 * @return
	*/
	MathError Scale( const double scalar ) = delete;

	/*!
	 * @brief Add value to x component of this vector and Normalise again
	 * @param x_ Value to add in vector's unit
	 * @return Error code
	*/
	MathError AddToX( const double x_ ) = delete;

	/*!
	 * @brief Add value to y component of this vector and Normalise again
	 * @param y_ Value to add in vector's unit
	 * @return Error code
	*/
	MathError AddToY( const double y_ ) = delete;

	/*!
	 * @brief Add value to z component of this vector and Normalise again
	 * @param z_ Value to add in vector's unit
	 * @return Error code
	*/
	MathError AddToZ( const double z_ ) = delete;
};



/*!
 * @brief Class for points in cartesian coordinate system
*/
class Point3D : public Vector3D{

	using Vector3D::Vector3D;

	public:

	/*!
	 * @brief Converting constructor
	 * @param v Vector to convert from
	*/
	Point3D( const Vector3D v ) : Vector3D( v ){};

	/*!
	 * @brief Convert point's data to string
	 * @return String with point's data
	*/
	string ToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief Convert point to different coordinate system
	 * @param target_coordinate_system Target system
	 * @return Converted point
	*/
	Point3D ConvertTo( const CoordinateSystem* const target_coordinate_system ) const;

	/*!
	 * @brief Convert point to different coordinate system
	 * @param targetP Point with target system
	 * @return Converted point
	*/
	Point3D ConvertTo( const Point3D targetP ) const;

	/*!
	 * @brief Get x,y and z component
	 * @return x, y and z component in local coordinate system
	*/
	PrimitiveVector3 Components( void ) const{ return Vector3D::Components(); };

	/*!
	 * @brief Get x,y and z component
	 * @param target Coordinate system to convert components to
	 * @return x, y and z component in local coordinate system
	*/
	PrimitiveVector3 Components( const CoordinateSystem* const target ) const;

	/*!
	 * @brief Get x,y and z component
	 * @param targetV Point with coordinate system to convert components to
	 * @return x, y and z component in local coordinate system
	*/
	PrimitiveVector3 Components( const Point3D targetP ) const;

	/*!
	 * @brief Get global x component
	 * @return x component in global coordinate system
	*/
	double GlobalX( void ) const;

	/*!
	 * @brief Get global y component
	 * @return y component in global coordinate system
	*/
	double GlobalY( void ) const;

	/*!
	 * @brief Get global z component
	 * @return z component in global coordinate system
	*/
	double GlobalZ( void ) const;

	/*!
	 * @brief Get global x,y and z component
	 * @return x, y and z component in global coordinate system
	*/
	PrimitiveVector3 GlobalComponents( void ) const;

	/*!
	 * @brief Project point on XY plane of given coordinate system
	 * @param coordinate_system_ Coordinate system
	 * @return Projected point
	*/
	Point3D ProjectOnXYPlane( const CoordinateSystem* const cSys ) const;
};