/*********************************************************************
 * @file   primitiveVector3.cpp
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
#include "primitiveVector3.h"



 /*********************************************************************
	Implementations
 *********************************************************************/


/*
	PrimitiveVector3 implementation
*/

string PrimitiveVector3::ConvertToString( [[maybe_unused]] const unsigned int newline_tabulators ) const{
	string instance_string;
	char character_array[ 64 ] = { 0 };
	snprintf( character_array, 64, "(%.6f,%.6f,%.6f)", x, y, z );

	instance_string += character_array;
	return instance_string;
}

bool PrimitiveVector3::operator== ( const PrimitiveVector3 vector_to_compare ) const{

	const PrimitiveVector3 difference_vector = vector_to_compare - *this;

	return IsNearlyEqualDistance( difference_vector.GetLength(), 0 );
}

PrimitiveVector3 PrimitiveVector3::operator/ ( const double divisor ) const{
	if( divisor == 0 ) return *this;
	return PrimitiveVector3{ x / divisor, y / divisor, z / divisor };
}

double PrimitiveVector3::GetLength( void ) const{
	return sqrt( pow( x, 2 ) + pow( y, 2 ) + pow( z, 2 ) );
}

void PrimitiveVector3::Scale( const double scalar ){
	x *= scalar;
	y *= scalar;
	z *= scalar;
}

MathematicalObject::MathError PrimitiveVector3::Normalise( void ){

	// new length
	const double length = this->GetLength();

	// length must not by zero
	if( IsNearlyEqualDistance( length, 0 ) ) return CheckForAndOutputError( MathError::Operation, "normalization only possible with vector that has length!" );

	// exit when length is already one
	if( IsNearlyEqualDistance( length, 1 ) ) return MathError::Ok;

	// calculate scaling factor as reciprocal of length
	const double length_reciprocal = 1 / length;

	// scale and return error code
	Scale( length_reciprocal );

	return MathError::Ok;
}

void PrimitiveVector3::RotateAroundXAxis( const double sine_phi, const double cosine_phi ){
	// apply rotation matrix
	double x_ = x;
	double y_ = y * cosine_phi - z * sine_phi;
	double z_ = y * sine_phi + z * cosine_phi;

	x = x_; y = y_; z = z_;
}

void PrimitiveVector3::RotateAroundYAxis( const double sine_phi, const double cosine_phi ){
	// apply rotation matrix
	double x_ = x * cosine_phi + z * sine_phi;
	double y_ = y;
	double z_ = z * cosine_phi - x * sine_phi;

	x = x_; y = y_; z = z_;
}

void PrimitiveVector3::RotateAroundZAxis( const double sine_phi, const double cosine_phi ){
	// apply rotation matrix
	double x_ = x * cosine_phi - y * sine_phi;
	double y_ = x * sine_phi + y * cosine_phi;
	double z_ = z;

	x = x_; y = y_; z = z_;
}

MathematicalObject::MathError PrimitiveVector3::Rotate( 
																const PrimitiveVector3 rotation_vector, 
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
	PrimitiveVector3 rotation_vector_copy{ rotation_vector };
	rotation_vector_copy.Normalise();
	
	// length of the axis projection on x-y plane
	const double projection_length = sqrt( pow( rotation_vector_copy.x, 2 ) 
																			 + pow( rotation_vector_copy.y, 2 ) );		
	// error doing square root?
	if (errno != 0)
		return CheckForAndOutputError(MathError::General, "error calculation square root!");
	
	// sine and cosine of angle theta:
	// angle between rotation axis projection onto x-y plane and x-axis
	double sine_theta = 0, cosine_theta = 1;	

	// avoid division by zero. d = 0 means rotation axis is parallel to z-axis
	if( projection_length > 0 ){
		sine_theta = rotation_vector_copy.y / projection_length;	// sine of the angle Theta
		cosine_theta = rotation_vector_copy.x / projection_length;// cosine of the angle Theta

		// clockwise rotation of rotation axis and this vector around z-axis 
		// to align rotation axis to x-z plane
		rotation_vector_copy.RotateAroundZAxis( -sine_theta, cosine_theta );
		this->RotateAroundZAxis( -sine_theta, cosine_theta );
	}

	// gamma is the angle between the rotation axis (aligned to x-z plane) and the z-axis
	// rotation vector has been normalised - sine of gamma is length / 1
	// cosine is just the z-component of rotation vector
	const double sine_gamma = projection_length;				
	const double cosine_gamma = rotation_vector_copy.z;	

	// clockwise rotation of this vector around y-axis
	this->RotateAroundYAxis( -sine_gamma, cosine_gamma );

	// the axis rotation vector is now aligned with the z-axis

	// sine and cosine of angle to rotate around
	const double sine_angle = sin( rotation_angle );
	const double cosine_angle = cos( rotation_angle );

	// counter-clockwise z-axis rotation of this vector by rotation angle
	this->RotateAroundZAxis( sine_angle, cosine_angle );
	// counter-clockwise y-axis rotation of this vector by gamma to reverse step 2
	this->RotateAroundYAxis( sine_gamma, cosine_gamma );

	if( projection_length > 0 ){
		// counter-clockwise z-axis rotation of this vector by theta to reverse step 1
		this->RotateAroundZAxis( sine_theta, cosine_theta );
	}
	return MathError::Ok;
}