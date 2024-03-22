/*********************************************************************
 * @file   PrimitiveVector3.cpp
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
#include "primitiveVector3.h"



 /*********************************************************************
	Implementations
 *********************************************************************/


/*
	PrimitiveVector3 implementation
*/

string PrimitiveVector3::ToString( [[maybe_unused]] const unsigned int newline_tabulators ) const{
	string str;
	char tempCharArr[ 64 ] = { 0 };
	snprintf( tempCharArr, 64, "(%.6f,%.6f,%.6f)", x, y, z );

	str += tempCharArr;
	return str;
}

bool PrimitiveVector3::operator== ( const PrimitiveVector3 v ) const{

	const PrimitiveVector3 diffVec = v - *this;

	return IsNearlyEqualDistance( diffVec.GetLength(), 0 );
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
	const double len = this->GetLength();

	// length must not by zero
	if( IsNearlyEqualDistance( len, 0 ) ) return CheckForAndOutputError( MathError::Operation, "Normalization only possible with vector that has length!" );

	// exit when length is already one
	if( IsNearlyEqualDistance( len, 1 ) ) return MathError::Ok;

	// calculate scaling factor as reciprocal of length
	const double lenRec = 1 / len;

	// Scale and return error code
	Scale( lenRec );

	return MathError::Ok;
}

void PrimitiveVector3::RotateAroundXAxis( const double sinPhi, const double cosPhi ){
	// apply rotation matrix
	double x_ = x;
	double y_ = y * cosPhi - z * sinPhi;
	double z_ = y * sinPhi + z * cosPhi;

	x = x_; y = y_; z = z_;
}

void PrimitiveVector3::RotateAroundYAxis( const double sinPhi, const double cosPhi ){
	// apply rotation matrix
	double x_ = x * cosPhi + z * sinPhi;
	double y_ = y;
	double z_ = z * cosPhi - x * sinPhi;

	x = x_; y = y_; z = z_;
}

void PrimitiveVector3::RotateAroundZAxis( const double sinPhi, const double cosPhi ){
	// apply rotation matrix
	double x_ = x * cosPhi - y * sinPhi;
	double y_ = x * sinPhi + y * cosPhi;
	double z_ = z;

	x = x_; y = y_; z = z_;
}

MathematicalObject::MathError PrimitiveVector3::Rotate( 
																const PrimitiveVector3 rotation_vector, 
																const double rotation_angle ){
	// Steps for rotation:
	// 1: rotate around z axis to tilt rot. axis into x-z plane 
	//		Could also rotate into y-z plane an rotate around x-axis in next step
	// 2: rotate around y-axis to align rot. axis with z-axis
	// 3. rotate this PrimitiveVector3 by phi around z-axis
	// 4. Undo previous rotation steps 1 and 2 in reverse order

	// n must have direction
	if( IsNearlyEqualDistance( rotation_vector.GetLength(), 0 ) ) 
		return CheckForAndOutputError( MathError::Input, "Rotation axis must have length!" );

	// create copy and Normalise
	PrimitiveVector3 rotation_vector_copy{ rotation_vector };
	rotation_vector_copy.Normalise();
	
	// length of the axis projection on x-y plane
	const double projection_length = sqrt( pow( rotation_vector_copy.x, 2 ) 
																			 + pow( rotation_vector_copy.y, 2 ) );		
	// error doing square root?
	if( errno != 0 ) 
		return CheckForAndOutputError( MathError::General, "Error calculation square root!" );	
	
	// Sine and cosine of angle theta:
	// angle between rotation axis projection onto x-y plane and x-axis
	double sin_theta = 0, cos_theta = 1;	

	// avoid division by zero. d = 0 means rotation axis is parallel to z-axis
	if( projection_length > 0 ){
		sin_theta = rotation_vector_copy.y / projection_length;	// Sine of the angle Theta
		cos_theta = rotation_vector_copy.x / projection_length;	// cosine of the angle Theta

		// clockwise rotation of rotation axis and this vector around z-axis 
		// to align rotation axis to x-z plane
		rotation_vector_copy.RotateAroundZAxis( -sin_theta, cos_theta );
		this->RotateAroundZAxis( -sin_theta, cos_theta );
	}

	// gamma is the angle between the rotation axis (aligned to x-z plane) and the z-axis
	// Rotation vector has been normalised - sine of gamma is length / 1
	// cosine is just the z-component of rotation vector
	const double sin_gamma = projection_length;				
	const double cos_gamma = rotation_vector_copy.z;	

	// clockwise rotation of this vector around y-axis
	this->RotateAroundYAxis( -sin_gamma, cos_gamma );

	// the axis rotation vector is now aligned with the z-axis

	// Sine and cosine of angle to rotate around
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