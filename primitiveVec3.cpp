/*********************************************************************
 * @file   primitiveVec3.cpp
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

#include "generel.h"
#include "generelMath.h"
#include "primitiveVec3.h"



 /*********************************************************************
	Implementations
 *********************************************************************/


/*
	primitiveVec3 implementation
*/

primitiveVec3::primitiveVec3( const v3 xyz ) : v3( xyz )
{}

primitiveVec3::primitiveVec3( const double x_, const double y_, const double z_ ) : v3( x_, y_, z_ )
{}

primitiveVec3::primitiveVec3( void ) : primitiveVec3( 0,0,0 ){}

string primitiveVec3::toStr( [[maybe_unused]] const unsigned int newLineTabulators ) const{
	string str;
	char tempCharArr[ 64 ] = { 0 };
	snprintf( tempCharArr, 64, "(%.6f,%.6f,%.6f)", x, y, z );

	str += tempCharArr;
	return str;
}

primitiveVec3::primitiveVec3( const vector<char>& binData, vector<char>::const_iterator& it ) :
	v3{ binData, it }
{}

bool primitiveVec3::operator== ( const primitiveVec3 v ) const{

	primitiveVec3 diffVec = v - *this;

	return iseqErr( diffVec.Length(), 0 );
}

bool primitiveVec3::operator!= ( const primitiveVec3 v ) const{
	return !( this->operator==( v ) );
}

primitiveVec3 primitiveVec3::operator+ ( const primitiveVec3 v ) const{
	return primitiveVec3{ x + v.x, y + v.y, z + v.z };
}

primitiveVec3 primitiveVec3::operator- ( const primitiveVec3 v ) const{
	return primitiveVec3{ x - v.x, y - v.y, z - v.z };
}

primitiveVec3 primitiveVec3::operator- ( void ) const{
	return primitiveVec3{ -x, -y, -z };
}

primitiveVec3 primitiveVec3::operator* ( const double scalar ) const{
	return primitiveVec3{ scalar * x, scalar * y, scalar * z  };
}

double primitiveVec3::operator* ( const primitiveVec3 v ) const{
	return this->x * v.x + this->y * v.y + this->z * v.z;
}

primitiveVec3 primitiveVec3::operator/ ( const double divisor ) const{
	if( divisor == 0 ) return *this;
	return primitiveVec3{ x / divisor, y / divisor, z / divisor };
}

double primitiveVec3::Length( void ) const{
	return sqrt( pow( x, 2 ) + pow( y, 2 ) + pow( z, 2 ) );
}

void primitiveVec3::scale( const double scalar ){
	x *= scalar;
	y *= scalar;
	z *= scalar;
}

mathObj::MATH_ERR primitiveVec3::normalize( void ){

	// New length
	double len = this->Length();

	// Length must not by zero
	if( iseqErr( len, 0 ) ) return checkErr( MATH_ERR::OPERATION, "Normalization only possible with vector that has length!" );

	// Exit when length is already one
	if( iseqErr( len, 1 ) ) return MATH_ERR::OK;

	// Calculate scaling factor as reciprocal of length
	double lenRec = 1 / len;

	// Scale and return error code
	scale( lenRec );

	return MATH_ERR::OK;
}

void primitiveVec3::rotXM( const double sinPhi, const double cosPhi ){
	// Apply rotation matrix
	double x_ = x;
	double y_ = y * cosPhi - z * sinPhi;
	double z_ = y * sinPhi + z * cosPhi;

	x = x_; y = y_; z = z_;
};

void primitiveVec3::rotYM( const double sinPhi, const double cosPhi ){
	// Apply rotation matrix
	double x_ = x * cosPhi + z * sinPhi;
	double y_ = y;
	double z_ = z * cosPhi - x * sinPhi;

	x = x_; y = y_; z = z_;
};

void primitiveVec3::rotZM( const double sinPhi, const double cosPhi ){
	// Apply rotation matrix
	double x_ = x * cosPhi - y * sinPhi;
	double y_ = x * sinPhi + y * cosPhi;
	double z_ = z;

	x = x_; y = y_; z = z_;
};

mathObj::MATH_ERR primitiveVec3::rotNM( const primitiveVec3 n, const double phi ){
	// Steps for rotation:
	// 1: rotate around z axis to tilt rot. axis into x-z plane
	// 2: rotate around y-axis to align rot. axis with z-axis
	// 3. rotate this primitiveVec3 by phi around z-axis
	// 4. Undo previous rotation steps 1 and 2 in reverse order

	// n must have direction
	if( iseqErr( n.Length(), 0 ) ) return checkErr( MATH_ERR::INPUT, "Rotation axis must have length!" );

	// Create copy and normalize
	primitiveVec3 nCpy{ n };
	nCpy.normalize();

	double d = sqrt( pow( nCpy.x, 2 ) + pow( nCpy.y, 2 ) );		// Length of the axis projection on x-y plane
	if( errno != 0 ) return checkErr( MATH_ERR::GENERAL, "Error calculation square root!" );		// Check error flag

	double sinThe = 0, cosThe = 1;								// Sine and cosine of angle Theta (angle between rot. axis projection onto x-y plane and x axis)

	// Avoid division by zero. d = 0 means rot. axis is parallel to z
	if( d > 0 ){
		sinThe = nCpy.y / d;		// Sine of the angle Theta
		cosThe = nCpy.x / d;		// Cosine of the angle Theta

		// Clockwise rotation of rotation axis and this vector around z-axis to align rotation axis to x-z plane
		nCpy.rotZM( -sinThe, cosThe );
		this->rotZM( -sinThe, cosThe );
	}

	// Gamma is the angle between the rotation axis (aligned to x-z plane) and the z-axis
	double sinGam = d;				// Rotation axis vector has been normalized - sine of Gamma is d / 1
	double cosGam = nCpy.z;			// Cosine is just the z-component of vector n_z / 1

	// Clockwise rotation of this vector around y-axis
	this->rotYM( -sinGam, cosGam );

	// The axis rotation vector is now aligned with the z-axis

	// Sine and cosine of angle to rotate around
	double sinPhi = sin( phi );
	double cosPhi = cos( phi );

	// Counter-clockwise z-axis rotation of this vector by Phi
	this->rotZM( sinPhi, cosPhi );

	// Counter-clockwise y-axis rotation of this vector by Gamma to reverse step 2
	this->rotYM( sinGam, cosGam );


	if( d > 0 ){
		// Counter-clockwise z-axis rotation of this vector by Theta to reverse step 1
		this->rotZM( sinThe, cosThe );
	}
	return MATH_ERR::OK;
};

