/*********************************************************************
 * @file   Primitivevector3.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



/*********************************************************************
	Includes
 *********************************************************************/
#include <cmath>
#include <string>
using std::string;

#include "generelMath.h"
#include "primitivevector3.h"



 /*********************************************************************
	Implementations
 *********************************************************************/


/*
	Primitivevector3 implementation
*/

string Primitivevector3::ConvertToString( [[maybe_unused]] const unsigned int newline_tabulators ) const{
	string str;
	char tempCharArr[ 64 ] = { 0 };
	snprintf( tempCharArr, 64, "(%.6f,%.6f,%.6f)", x, y, z );

	str += tempCharArr;
	return str;
}

bool Primitivevector3::operator== ( const Primitivevector3 v ) const{

	const Primitivevector3 diffVec = v - *this;

	return IsNearlyEqualDistance( diffVec.GetLength(), 0 );
}

Primitivevector3 Primitivevector3::operator/ ( const double divisor ) const{
	if( divisor == 0 ) return *this;
	return Primitivevector3{ x / divisor, y / divisor, z / divisor };
}

double Primitivevector3::GetLength( void ) const{
	return sqrt( pow( x, 2 ) + pow( y, 2 ) + pow( z, 2 ) );
}

void Primitivevector3::Scale( const double scalar ){
	x *= scalar;
	y *= scalar;
	z *= scalar;
}

MathematicalObject::MathError Primitivevector3::Normalise( void ){

	// new length
	const double len = this->GetLength();

	// length must not by zero
	if( IsNearlyEqualDistance( len, 0 ) ) return CheckForAndOutputError( MathError::Operation, "normalization only possible with vector that has length!" );

	// exit when length is already one
	if( IsNearlyEqualDistance( len, 1 ) ) return MathError::Ok;

	// calculate scaling factor as reciprocal of length
	const double lenRec = 1 / len;

	// scale and return error code
	Scale( lenRec );

	return MathError::Ok;
}

void Primitivevector3::RotateAroundXAxis( const double sine_phi, const double cosine_phi ){
	// apply rotation matrix
	double x_ = x;
	double y_ = y * cosine_phi - z * sine_phi;
	double z_ = y * sine_phi + z * cosine_phi;

	x = x_; y = y_; z = z_;
}

void Primitivevector3::RotateAroundYAxis( const double sine_phi, const double cosine_phi ){
	// apply rotation matrix
	double x_ = x * cosine_phi + z * sine_phi;
	double y_ = y;
	double z_ = z * cosine_phi - x * sine_phi;

	x = x_; y = y_; z = z_;
}

void Primitivevector3::RotateAroundZAxis( const double sine_phi, const double cosine_phi ){
	// apply rotation matrix
	double x_ = x * cosine_phi - y * sine_phi;
	double y_ = x * sine_phi + y * cosine_phi;
	double z_ = z;

	x = x_; y = y_; z = z_;
}

MathematicalObject::MathError Primitivevector3::Rotate( 
																const Primitivevector3 rotation_vector, 
																const double rotation_angle ){
	// steps for rotation:
	// 1: rotate around z axis to tilt rot. axis into x-z plane 
	//		Could also rotate into y-z plane an rotate around x-axis in next step
	// 2: rotate around y-axis to align rot. axis with z-axis
	// 3. rotate this primitive vector by phi around z-axis
	// 4. Undo previous rotation steps 1 and 2 in reverse order

	// n must have direction
	if( IsNearlyEqualDistance( rotation_vector.GetLength(), 0 ) ) 
		return CheckForAndOutputError( MathError::Input, "rotation axis must have length!" );

	// create copy and Normalise
	Primitivevector3 rotation_vector_copy{ rotation_vector };
	rotation_vector_copy.Normalise();
	
	// length of the axis projection on x-y plane
	const double projection_length = sqrt( pow( rotation_vector_copy.x, 2 ) 
																			 + pow( rotation_vector_copy.y, 2 ) );		
	// error doing square root?
	if (errno != 0)
		return CheckForAndOutputError(MathError::General, "error calculation square root!");
	
	// sine and cosine of angle theta:
	// angle between rotation axis projection onto x-y plane and x-axis
	double sin_theta = 0, cos_theta = 1;	

	// avoid division by zero. d = 0 means rotation axis is parallel to z-axis
	if( projection_length > 0 ){
		sin_theta = rotation_vector_copy.y / projection_length;	// sine of the angle Theta
		cos_theta = rotation_vector_copy.x / projection_length;	// cosine of the angle Theta

		// clockwise rotation of rotation axis and this vector around z-axis 
		// to align rotation axis to x-z plane
		rotation_vector_copy.RotateAroundZAxis( -sin_theta, cos_theta );
		this->RotateAroundZAxis( -sin_theta, cos_theta );
	}

	// gamma is the angle between the rotation axis (aligned to x-z plane) and the z-axis
	// rotation vector has been normalised - sine of gamma is length / 1
	// cosine is just the z-component of rotation vector
	const double sin_gamma = projection_length;				
	const double cos_gamma = rotation_vector_copy.z;	

	// clockwise rotation of this vector around y-axis
	this->RotateAroundYAxis( -sin_gamma, cos_gamma );

	// the axis rotation vector is now aligned with the z-axis

	// sine and cosine of angle to rotate around
	const double sin_angle = sin( rotation_angle );
	const double cos_angle = cos( rotation_angle );

	// counter-clockwise z-axis rotation of this vector by rotation angle
	this->RotateAroundZAxis( sin_angle, cos_angle );
	// counter-clockwise y-axis rotation of this vector by gamma to reverse step 2
	this->RotateAroundYAxis( sin_gamma, cos_gamma );

	if( projection_length > 0 ){
		// counter-clockwise z-axis rotation of this vector by theta to reverse step 1
		this->RotateAroundZAxis( sin_theta, cos_theta );
	}
	return MathError::Ok;
}