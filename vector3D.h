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
	 * @param coordinates coordinates of vector
	*/
	vector3D( const Coordinates coordinates );

	/*!
	 * @brief constructor
	 * @param components components of vector
	 * @param coordinate_system coordinate system of components
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
	 * @param vector_to_compare vector to compare with
	 * @return true when all components are equal
	*/
	bool operator== ( const vector3D vector_to_compare ) const{ return this->Coordinates::operator==( vector_to_compare ); };

	/*!
	 * @brief addition operator
	 * @param summand vector to add
	 * @return sum of vectors in unit of this vector
	*/
	vector3D operator+ ( const vector3D summand ) const{ return this->Coordinates::operator+( summand ); };

	/*!
	 * @brief substraction operator
	 * @param subtrahend vector to substract
	 * @return difference (v1 - v2) of vectors in unit of this vector
	*/
	vector3D operator- ( const vector3D subtrahend ) const{ return this->Coordinates::operator-( subtrahend ); };

	/*!
	 * @brief negation operator
	 * @return negated vector
	*/
	vector3D operator- ( void ) const{ return this->Coordinates::operator-(); };

	/*!
	 * @brief scale vector
	 * @param scalar factor
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
	 * @param second_vector second vector
	 * @return dot product of the vectors
	*/
	double operator* ( const vector3D second_vector ) const;

	/*!
	 * @brief calculate cross product
	 * @param second_vector second vector
	 * @return cross product this * second_vector in unit of this vector
	*/
	vector3D operator^( const vector3D second_vector ) const;

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
	 * @return length of vector in local coordinate system
	*/
	double length( void ) const{ return length_; };

	/*!
	 * @brief get Pointer to coordinate system
	 * @return pointer to this vector's coordinate system
	*/
	const CoordinateSystem* GetCoordinateSystem( void ) const{ return coordinate_system_; };

	/*!
	 * @brief check if vector's are defined in same system
	 * @param vector_to_compare second vector
	 * @return true when this and vector_to_compare are defined in the same coordinate system
	*/
	bool HasSameSystem( const vector3D vector_to_compare ) const;

	/*!
	 * @brief convert vector to different coordinate system
	 * @param target_coordinate_system System to convert to
	 * @return converted vector
	*/
	vector3D ConvertTo( const CoordinateSystem* const target_coordinate_system ) const;

	/*!
	 * @brief convert vector to different coordinate system
	 * @param target_coordinate_system vector to which system this gets converted
	 * @return converted vector
	*/
	vector3D ConvertTo( const vector3D target_coordinate_system ) const;

	/*!
	 * @brief convert vector to different coordinate system
	 * @param target_coordinate_system Line to which system this gets converted
	 * @return converted vector
	*/
	vector3D ConvertTo( const Line target_coordinate_system ) const;

	/*!
	 * @brief convert vector to different coordinate system
	 * @param target_coordinate_system Surface to which system this gets converted
	 * @return converted vector
	*/
	vector3D ConvertTo( const Surface target_coordinate_system ) const;

	/*!
	 * @brief get x,y and z component
	 * @return x, y and z component in local coordinate system
	*/
	Primitivevector3 GetComponents( void ) const{ return static_cast<Primitivevector3>( *this ); };

	/*!
	 * @brief get x,y and z component
	 * @param target_coordinate_system coordinate system to convert components to
	 * @return x, y and z component in local coordinate system
	*/
	Primitivevector3 GetComponents( const CoordinateSystem* const target_coordinate_system ) const;

	/*!
	 * @brief get x,y and z component
	 * @param target_coordinate_system vector with coordinate system to convert components to
	 * @return x, y and z component in local coordinate system
	*/
	Primitivevector3 GetComponents( const vector3D target_coordinate_system ) const;

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
	 * @param scalar factor
	 * @return error code
	*/
	MathError Scale( const double scalar );

	/*!
	 * @brief normalise vector's length to one
	 * @return error code
	 * @details error code ist set when vector's length is zero
	*/
	MathError Normalise( void );

	/*!
	 * @brief add value to x component of this vector
	 * @param x_ value to add in vector's unit
	 * @return error code
	*/
	MathError AddToX( const double x_ );

	/*!
	 * @brief add value to y component of this vector
	 * @param y_ value to add in vector's unit
	 * @return error code
	*/
	MathError AddToY( const double y_ );

	/*!
	 * @brief add value to z component of this vector
	 * @param z_ value to add in vector's unit
	 * @return error code
	*/
	MathError AddToZ( const double z_ );

	/*!
	 * @brief calculate angle between this and another vector
	 * @param second_vector second vector
	 * @return angle in radians
	*/
	double GetAngle( const vector3D second_vector ) const;

	/*!
	 * @brief checks whether two vectors are orthogonal
	 * @param second_vector second vector
	 * @return true when vectors are orthogonal
	*/
	bool IsOrthogonal( const vector3D second_vector ) const;

	/*!
	 * @brief counterclockwise rotation of this vector around x-axis
	 * @param sine_phi sine of the angle
	 * @param cosine_phi cosine of the angle
	 * @return error code
	*/
	MathError RotateAroundXAxis( const double sine_phi, const double cosine_phi );

	/*!
	 * @brief counterclockwise rotation of this vector around y-axis
	 * @param sine_phi sine of the angle
	 * @param cosine_phi cosine of the angle
	 * @return error code
	*/
	MathError RotateAroundYAxis( const double sine_phi, const double cosine_phi );

	/*!
	 * @brief counterclockwise rotation of this vector around z-axis
	 * @param sine_phi sine of the angle
	 * @param cosine_phi cosine of the angle
	 * @return error code
	*/
	MathError RotateAroundZAxis( const double sine_phi, const double cosine_phi );

	/*!
	 * @brief counterclockwise rotation of this vector around x-axis
	 * @param rotation_angle angle in radians
	 * @return error code
	*/
	MathError RotateAroundXAxis( const double rotation_angle );

	/*!
	 * @brief counterclockwise rotation of this vector around y-axis
	 * @param rotation_angle angle in radians
	 * @return error code
	*/
	MathError RotateAroundYAxis( const double rotation_angle );

	/*!
	 * @brief counterclockwise rotation of this vector around z-axis
	 * @param rotation_angle angle in radians
	 * @return error code
	*/
	MathError RotateAroundZAxis( const double rotation_angle );

	/*!
	 * @brief counterclockwise rotation of this vector around second vector
	 * @param axis rotation axis vector
	 * @param rotation_angle angle in radians
	 * @return error code
	*/
	MathError Rotate( const vector3D axis, const double rotation_angle );

	/*!
	 * @brief counterclockwise rotation of vector around second vector
	 * @param axis rotation axis vector
	 * @param rotation_angle rotation_angle in radians
	 * @return rotated vector
	*/
	vector3D RotateConstant( const vector3D axis, const double rotation_angle ) const;

	/*!
	 * @brief project vector on XY plane of given coordinate system
	 * @param coordinate_system coordinate system
	 * @return projected vector
	*/
	vector3D ProjectOnXYPlane( const CoordinateSystem* const coordinate_system ) const;

	/*!
	 * @brief flip x-values
	 * @return vector with negated x values
	*/
	vector3D NegateXComponent( void ) const;


	private:
	double length_;				/*!< length */

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
	 * @param source_vector vector to convert
	*/
	Unitvector3D( const vector3D source_vector ) : vector3D( source_vector ){
		Normalise();
	};

	/*!
	 * @brief constructor
	 * @param components components
	 * @param coordinate_system coordinate system
	*/
	Unitvector3D( const Tuple3D components, const CoordinateSystem* const coordinate_system ) : Unitvector3D{ vector3D{ components, coordinate_system } } {};

	/*!
	 * @brief default constructor
	*/
	Unitvector3D( void ) : Unitvector3D{ Tuple3D{ 1, 0, 0 }, GetDummySystem() } {};

	/*!
	 * @brief deleted function
	*/
	MathError Scale( const double scalar ) = delete;

	/*!
	 * @brief deleted function 
	*/
	MathError AddToX( const double x ) = delete;

	/*!
	 * @brief deleted function
	*/
	MathError AddToY( const double y ) = delete;

	/*!
	 * @brief deleted function
	*/
	MathError AddToZ( const double z ) = delete;
};



/*!
 * @brief class for points in cartesian coordinate system
*/
class Point3D : public vector3D{

	using vector3D::vector3D;

	public:

	/*!
	 * @brief converting constructor
	 * @param source_vector vector to convert from
	*/
	Point3D( const vector3D source_vector ) : vector3D( source_vector ){};

	/*!
	 * @brief convert point's data to string
	 * @return string with point's data
	*/
	string ConvertToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief convert point to different coordinate system
	 * @param target_coordinate_system Target system
	 * @return converted point
	*/
	Point3D ConvertTo( const CoordinateSystem* const target_coordinate_system ) const;

	/*!
	 * @brief convert point to different coordinate system
	 * @param target_coordinate_system Point with target system
	 * @return converted point
	*/
	Point3D ConvertTo( const Point3D target_coordinate_system ) const;

	/*!
	 * @brief get x,y and z component
	 * @return x, y and z component in local coordinate system
	*/
	Primitivevector3 GetComponents( void ) const{ return vector3D::GetComponents(); };

	/*!
	 * @brief get x,y and z component
	 * @param target_coordinate_system coordinate system to convert components to
	 * @return x, y and z component in local coordinate system
	*/
	Primitivevector3 GetComponents( const CoordinateSystem* const target_coordinate_system ) const;

	/*!
	 * @brief get x,y and z component
	 * @param target_coordinate_system Point with coordinate system to convert components to
	 * @return x, y and z component in local coordinate system
	*/
	Primitivevector3 GetComponents( const Point3D target_coordinate_system ) const;

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
	 * @param coordinate_system coordinate system
	 * @return projected point
	*/
	Point3D ProjectOnXYPlane( const CoordinateSystem* const coordinate_system ) const;
};