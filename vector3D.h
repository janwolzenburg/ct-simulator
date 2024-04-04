#pragma once
/*********************************************************************
 * @file   vector3D.h
 * @brief  classes for vectors, unit vectors and points
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
 * @brief class for vectors in three dimensional cartesian coordinate systems
*/
class vector3D : protected Coordinates{

	public:

	/*!
	 * @brief constructor
	 * @param coords Coordinates of vector
	*/
	vector3D( const Coordinates coords );

	/*!
	 * @brief constructor
	 * @param components Components of vector
	 * @param coordinate_system Coordinate system of components
	*/
	vector3D( const Tuple3D components, const CoordinateSystem* const coordinate_system );

	/*!
	 * @brief default constructor initializing components to zero coordinate system to global
	*/
	vector3D( void );

	/*!
	 * @brief convert vector's data to string
	 * @return string with vector's data
	*/
	string ConvertToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief comparison operator
	 * @param operand vector to compare with
	 * @return true when all components are equal
	*/
	bool operator== ( const vector3D operand ) const{ return this->Coordinates::operator==( operand ); };

	/*!
	 * @brief addition operator
	 * @param summand vector to add
	 * @return sum of vectors in unit of this vector
	*/
	vector3D operator+ ( const vector3D summand ) const{ return this->Coordinates::operator+( summand ); };

	/*!
	 * @brief substraction operator
	 * @param subtrahend vector to substract
	 * @return GetDifference (v1 - v2) of vectors in unit of this vector
	*/
	vector3D operator- ( const vector3D subtrahend ) const{ return this->Coordinates::operator-( subtrahend ); };

	/*!
	 * @brief negation operator
	 * @return negated vector
	*/
	vector3D operator- ( void ) const{ return this->Coordinates::operator-(); };

	/*!
	 * @brief scale vector
	 * @param scalar Factor
	 * @return scaled vector
	*/
	vector3D operator* ( const double scalar ) const{ return this->Coordinates::operator*( scalar ); };

	/*!
	 * @brief divide vector
	 * @param divisor Divisor
	 * @return divided vector
	*/
	vector3D operator/ ( const double divisor ) const{ return this->Coordinates::operator/( divisor ); };

	/*!
	 * @brief calculate dot product between to vectors
	 * @param operand Second vector
	 * @return dot product in unit of this vector squared
	*/
	double operator* ( const vector3D operand ) const;

	/*!
	 * @brief calculate cross product
	 * @param operand Second vector
	 * @return Cross product this * operand in unit of this vector
	*/
	vector3D operator^( const vector3D operand ) const;

	/*!
	 * @brief get x component
	 * @return x component in local coordinate system
	*/
	double X( void ) const{ return x; };

	/*!
	 * @brief get y component
	 * @return y component in local coordinate system
	*/
	double Y( void ) const{ return y; };

	/*!
	 * @brief get z component
	 * @return z component in local coordinate system
	*/
	double Z( void ) const{ return z; };

	/*!
	 * @brief get length of vector
	 * @return Length of vector in local coordinate system
	*/
	double length( void ) const{ return length_; };

	/*!
	 * @brief get Pointer to coordinate system
	 * @return pointer to this vector's coordinate system
	*/
	const CoordinateSystem* GetCoordinateSystem( void ) const{ return coordinate_system_; };

	/*!
	 * @brief check if vector's are defined in same system
	 * @param v Second vector
	 * @return true when this and vector v are defined in the same coordinate system
	*/
	bool HasSameSystem( const vector3D v ) const;

	/*!
	 * @brief convert vector to different coordinate system
	 * @param target_coordinate_system System to convert to
	 * @return Converted vector
	*/
	vector3D ConvertTo( const CoordinateSystem* const target_coordinate_system ) const;

	/*!
	 * @brief convert vector to different coordinate system
	 * @param coordinate_system_source vector to which system this gets converted
	 * @return Converted vector
	*/
	vector3D ConvertTo( const vector3D coordinate_system_source ) const;

	/*!
	 * @brief convert vector to different coordinate system
	 * @param coordinate_system_source Line to which system this gets converted
	 * @return Converted vector
	*/
	vector3D ConvertTo( const Line coordinate_system_source ) const;

	/*!
	 * @brief convert vector to different coordinate system
	 * @param coordinate_system_source Surface to which system this gets converted
	 * @return Converted vector
	*/
	vector3D ConvertTo( const Surface coordinate_system_source ) const;

	/*!
	 * @brief get x,y and z component
	 * @return x, y and z component in local coordinate system
	*/
	Primitivevector3 GetComponents( void ) const{ return static_cast<Primitivevector3>( *this ); };

	/*!
	 * @brief get x,y and z component
	 * @param target_coordinate_system Coordinate system to convert components to
	 * @return x, y and z component in local coordinate system
	*/
	Primitivevector3 GetComponents( const CoordinateSystem* const target_coordinate_system ) const;

	/*!
	 * @brief get x,y and z component
	 * @param coordinate_system_source vector with coordinate system to convert components to
	 * @return x, y and z component in local coordinate system
	*/
	Primitivevector3 GetComponents( const vector3D coordinate_system_source ) const;

	/*!
	 * @brief get global x,y and z component
	 * @return x, y and z component in global coordinate system
	*/
	Primitivevector3 GetGlobalComponents( void ) const;

	/*!
	 * @brief get global x component
	 * @return x component in global coordinate system
	*/
	double GetGlobalX( void ) const;

	/*!
	 * @brief get global y component
	 * @return y component in global coordinate system
	*/
	double GetGlobalY( void ) const;

	/*!
	 * @brief get global z component
	 * @return z component in global coordinate system
	*/
	double GetGlobalZ( void ) const;

	/*!
	 * @brief scale vector by factor
	 * @param scalar Factor
	 * @return error code
	*/
	MathError Scale( const double scalar );

	/*!
	 * @brief normalise vector's length to one
	 * @return error code
	 * @details Error code ist set when vector's length is zero
	*/
	MathError Normalise( void );

	/*!
	 * @brief add value to x component of this vector
	 * @param x_ Value to add in vector's unit
	 * @return error code
	*/
	MathError AddToX( const double x_ );

	/*!
	 * @brief add value to y component of this vector
	 * @param y_ Value to add in vector's unit
	 * @return error code
	*/
	MathError AddToY( const double y_ );

	/*!
	 * @brief add value to z component of this vector
	 * @param z_ Value to add in vector's unit
	 * @return error code
	*/
	MathError AddToZ( const double z_ );

	/*!
	 * @brief calculate angle between this and another vector
	 * @param v2 Second vector
	 * @return angle in radians
	*/
	double GetAngle( const vector3D v2 ) const;

	/*!
	 * @brief checks whether two vectors are orthogonal
	 * @param v2 Second vector
	 * @return true when vectors are orthogonal
	*/
	bool IsOrthogonal( const vector3D v2 ) const;

	/*!
	 * @brief counterclockwise rotation of this vector around x-axis
	 * @param sinPhi Sine of the angle
	 * @param cosine_phi Cosine of the angle
	 * @return error code
	*/
	MathError RotateAroundXAxis( const double sinPhi, const double cosine_phi );

	/*!
	 * @brief counterclockwise rotation of this vector around y-axis
	 * @param sinPhi Sine of the angle
	 * @param cosine_phi Cosine of the angle
	 * @return error code
	*/
	MathError RotateAroundYAxis( const double sinPhi, const double cosine_phi );

	/*!
	 * @brief counterclockwise rotation of this vector around z-axis
	 * @param sinPhi Sine of the angle
	 * @param cosine_phi Cosine of the angle
	 * @return error code
	*/
	MathError RotateAroundZAxis( const double sinPhi, const double cosine_phi );

	/*!
	 * @brief counterclockwise rotation of this vector around x-axis
	 * @param arc_angle Angle in radians
	 * @return error code
	*/
	MathError RotateAroundXAxis( const double arc_angle );

	/*!
	 * @brief counterclockwise rotation of this vector around y-axis
	 * @param arc_angle Angle in radians
	 * @return error code
	*/
	MathError RotateAroundYAxis( const double arc_angle );

	/*!
	 * @brief counterclockwise rotation of this vector around z-axis
	 * @param arc_angle Angle in radians
	 * @return error code
	*/
	MathError RotateAroundZAxis( const double arc_angle );

	/*!
	 * @brief counterclockwise rotation of this vector around second vector
	 * @param n Rotation axis vector
	 * @param arc_angle Angle in radians
	 * @return error code
	*/
	MathError Rotate( const vector3D n, const double arc_angle );

	/*!
	 * @brief counterclockwise rotation of vector around second vector
	 * @param n Rotation axis vector
	 * @param arc_angle arc_angle in radians
	 * @return rotated vector
	*/
	vector3D RotateConstant( const vector3D axis, const double arc_angle ) const;

	/*!
	 * @brief project vector on XY plane of given coordinate system
	 * @param coordinate_system Coordinate system
	 * @return projected vector
	*/
	vector3D ProjectOnXYPlane( const CoordinateSystem* const coordinate_system ) const;

	/*!
	 * @brief flip x-values
	 * @return vector with negated x values
	*/
	vector3D NegateXComponent( void ) const;


	private:
	double length_;				/*!< Length */

	/*!
	 * @brief calculates vector's length corresponding to vector component and stores it in member
	 * @return error code
	*/
	MathError UpdateLength( void );

};



/*!
 * @brief class for unit vectors with length one
*/
class Unitvector3D : public vector3D{

	public:

	/*!
	 * @brief converting constructor from vector
	 * @param v vector to convert
	*/
	Unitvector3D( const vector3D v ) : vector3D( v ){
		Normalise();
	};

	/*!
	 * @brief constructor
	 * @param components Components
	 * @param coordinate_system Coordinate system
	*/
	Unitvector3D( const Tuple3D components, const CoordinateSystem* const coordinate_system ) : Unitvector3D{ vector3D{ components, coordinate_system } } {};

	/*!
	 * @brief default constructor
	*/
	Unitvector3D( void ) : Unitvector3D{ Tuple3D{ 1, 0, 0 }, GetDummySystem() } {};

	/*!
	 * @brief scaling unit vector does not have an effect
	 * @param scalar
	 * @return
	*/
	MathError Scale( const double scalar ) = delete;

	/*!
	 * @brief add value to x component of this vector and Normalise again
	 * @param x_ Value to add in vector's unit
	 * @return error code
	*/
	MathError AddToX( const double x_ ) = delete;

	/*!
	 * @brief add value to y component of this vector and Normalise again
	 * @param y_ Value to add in vector's unit
	 * @return error code
	*/
	MathError AddToY( const double y_ ) = delete;

	/*!
	 * @brief add value to z component of this vector and Normalise again
	 * @param z_ Value to add in vector's unit
	 * @return error code
	*/
	MathError AddToZ( const double z_ ) = delete;
};



/*!
 * @brief class for points in cartesian coordinate system
*/
class Point3D : public vector3D{

	using vector3D::vector3D;

	public:

	/*!
	 * @brief converting constructor
	 * @param v vector to convert from
	*/
	Point3D( const vector3D v ) : vector3D( v ){};

	/*!
	 * @brief convert point's data to string
	 * @return string with point's data
	*/
	string ConvertToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief convert point to different coordinate system
	 * @param target_coordinate_system Target system
	 * @return Converted point
	*/
	Point3D ConvertTo( const CoordinateSystem* const target_coordinate_system ) const;

	/*!
	 * @brief convert point to different coordinate system
	 * @param coordinate_system_source Point with target system
	 * @return Converted point
	*/
	Point3D ConvertTo( const Point3D coordinate_system_source ) const;

	/*!
	 * @brief get x,y and z component
	 * @return x, y and z component in local coordinate system
	*/
	Primitivevector3 GetComponents( void ) const{ return vector3D::GetComponents(); };

	/*!
	 * @brief get x,y and z component
	 * @param target_coordinate_system Coordinate system to convert components to
	 * @return x, y and z component in local coordinate system
	*/
	Primitivevector3 GetComponents( const CoordinateSystem* const target_coordinate_system ) const;

	/*!
	 * @brief get x,y and z component
	 * @param coordinate_system_source Point with coordinate system to convert components to
	 * @return x, y and z component in local coordinate system
	*/
	Primitivevector3 GetComponents( const Point3D coordinate_system_source ) const;

	/*!
	 * @brief get global x component
	 * @return x component in global coordinate system
	*/
	double GetGlobalX( void ) const;

	/*!
	 * @brief get global y component
	 * @return y component in global coordinate system
	*/
	double GetGlobalY( void ) const;

	/*!
	 * @brief get global z component
	 * @return z component in global coordinate system
	*/
	double GetGlobalZ( void ) const;

	/*!
	 * @brief get global x,y and z component
	 * @return x, y and z component in global coordinate system
	*/
	Primitivevector3 GetGlobalComponents( void ) const;

	/*!
	 * @brief project point on XY plane of given coordinate system
	 * @param coordinate_system Coordinate system
	 * @return projected point
	*/
	Point3D ProjectOnXYPlane( const CoordinateSystem* const coordinate_system ) const;
};