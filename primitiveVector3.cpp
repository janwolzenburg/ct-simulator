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

	// New length_
	double len = this->GetLength();

	// Length must not by zero
	if( IsNearlyEqualDistance( len, 0 ) ) return CheckForAndOutputError( MathError::Operation, "Normalization only possible with vector that has length_!" );

	// Exit when length_ is already one
	if( IsNearlyEqualDistance( len, 1 ) ) return MathError::Ok;

	// Calculate scaling factor as reciprocal of length_
	double lenRec = 1 / len;

	// Scale and return error code
	Scale( lenRec );

	return MathError::Ok;
}

void PrimitiveVector3::RotateAroundXAxis( const double sinPhi, const double cosPhi ){
	// Apply rotation matrix
	double x_ = x;
	double y_ = y * cosPhi - z * sinPhi;
	double z_ = y * sinPhi + z * cosPhi;

	x = x_; y = y_; z = z_;
}

void PrimitiveVector3::RotateAroundYAxis( const double sinPhi, const double cosPhi ){
	// Apply rotation matrix
	double x_ = x * cosPhi + z * sinPhi;
	double y_ = y;
	double z_ = z * cosPhi - x * sinPhi;

	x = x_; y = y_; z = z_;
}

void PrimitiveVector3::RotateAroundZAxis( const double sinPhi, const double cosPhi ){
	// Apply rotation matrix
	double x_ = x * cosPhi - y * sinPhi;
	double y_ = x * sinPhi + y * cosPhi;
	double z_ = z;

	x = x_; y = y_; z = z_;
}

MathematicalObject::MathError PrimitiveVector3::Rotate( const PrimitiveVector3 n, const double phi ){
	// Steps for rotation:
	// 1: rotate around z axis to tilt rot. axis into x-z plane
	// 2: rotate around y-axis to align rot. axis with z-axis
	// 3. rotate this PrimitiveVector3 by phi around z-axis
	// 4. Undo previous rotation steps 1 and 2 in reverse order

	// n must have direction
	if( IsNearlyEqualDistance( n.GetLength(), 0 ) ) return CheckForAndOutputError( MathError::Input, "Rotation axis must have length_!" );

	// Create copy and Normalise
	PrimitiveVector3 nCpy{ n };
	nCpy.Normalise();

	double d = sqrt( pow( nCpy.x, 2 ) + pow( nCpy.y, 2 ) );		// Length of the axis projection on x-y plane
	if( errno != 0 ) return CheckForAndOutputError( MathError::General, "Error calculation square root!" );		// Check error flag

	double sinThe = 0, cosThe = 1;								// Sine and cosine of angle Theta (angle between rot. axis projection onto x-y plane and x axis)

	// Avoid division by zero. d = 0 means rot. axis is parallel to z
	if( d > 0 ){
		sinThe = nCpy.y / d;		// Sine of the angle Theta
		cosThe = nCpy.x / d;		// Cosine of the angle Theta

		// Clockwise rotation of rotation axis and this vector around z-axis to align rotation axis to x-z plane
		nCpy.RotateAroundZAxis( -sinThe, cosThe );
		this->RotateAroundZAxis( -sinThe, cosThe );
	}

	// Gamma is the angle between the rotation axis (aligned to x-z plane) and the z-axis
	double sinGam = d;				// Rotation axis vector has been normalised - sine of Gamma is d / 1
	double cosGam = nCpy.z;			// Cosine is just the z-component of vector n_z / 1

	// Clockwise rotation of this vector around y-axis
	this->RotateAroundYAxis( -sinGam, cosGam );

	// The axis rotation vector is now aligned with the z-axis

	// Sine and cosine of angle to rotate around
	double sinPhi = sin( phi );
	double cosPhi = cos( phi );

	// Counter-clockwise z-axis rotation of this vector by Phi
	this->RotateAroundZAxis( sinPhi, cosPhi );

	// Counter-clockwise y-axis rotation of this vector by Gamma to reverse step 2
	this->RotateAroundYAxis( sinGam, cosGam );


	if( d > 0 ){
		// Counter-clockwise z-axis rotation of this vector by Theta to reverse step 1
		this->RotateAroundZAxis( sinThe, cosThe );
	}
	return MathError::Ok;
}

