/*********************************************************************
 * @file   PrimitiveCoordinateSystem.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



/*********************************************************************
	Includes
 *********************************************************************/
#include "primitiveCoordinateSystem.h"
#include "serialization.h"


 /*********************************************************************
	Implementations
 *********************************************************************/


/*
	PrimitiveCoordinateSystem implementation
*/

PrimitiveCoordinateSystem::PrimitiveCoordinateSystem( const Primitivevector3 origin, const Primitivevector3 ex, const Primitivevector3 ey, const Primitivevector3 ez )
	: origin_( origin ),
	ex_( ex ),
	ey_( ey ),
	ez_( ez )
{
	// normalise vectors
	ex_.Normalise();
	ey_.Normalise();
	ez_.Normalise();

	// vector have to be orthogonal to each other
	if( !IsNearlyEqualDistance( ex_ * ey_, 0 ) ||
		!IsNearlyEqualDistance( ey_ * ez_, 0 ) ||
		!IsNearlyEqualDistance( ex_ * ez_, 0 ) ) CheckForAndOutputError( MathError::Input, "unit axis must be orthogonal to each other!" );
}

PrimitiveCoordinateSystem::PrimitiveCoordinateSystem( const vector<char>& binary_data, vector<char>::const_iterator& it ) : 
	origin_( DeSerialize<Primitivevector3>( binary_data, it ) ),
	ex_( DeSerialize<Primitivevector3>( binary_data, it ) ),
	ey_( DeSerialize<Primitivevector3>( binary_data, it ) ),
	ez_( DeSerialize<Primitivevector3>( binary_data, it ) )
{
	// normalise vectors
	ex_.Normalise();
	ey_.Normalise();
	ez_.Normalise();

	// vector have to be orthogonal to each other
	if( !IsNearlyEqualDistance( ex_ * ey_, 0 ) ||
		!IsNearlyEqualDistance( ey_ * ez_, 0 ) ||
		!IsNearlyEqualDistance( ex_ * ez_, 0 ) ) CheckForAndOutputError( MathError::Input, "unit axis must be orthogonal to each other!" );
}

string PrimitiveCoordinateSystem::ConvertToString( const unsigned int newline_tabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) newLine += '\t';

	str += newLine + "p=" + origin_.ConvertToString();
	str += newLine + "ex_=" + ex_.ConvertToString() + newLine + "ey_=" + ey_.ConvertToString() + newLine + "ez_=" + ez_.ConvertToString();
	return str;
}

MathematicalObject::MathError PrimitiveCoordinateSystem::Rotate( const Primitivevector3 n, const double phi ){
	MathError tErr = MathError::Ok;
	MathError errCode = MathError::Ok;

	if( ( tErr = ex_.Rotate( n, phi ) ) != MathError::Ok ) errCode = tErr;
	if( ( tErr = ey_.Rotate( n, phi ) ) != MathError::Ok ) errCode = tErr;
	if( ( tErr = ez_.Rotate( n, phi ) ) != MathError::Ok ) errCode = tErr;

	return errCode;
}


size_t PrimitiveCoordinateSystem::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;
	num_bytes += origin_.Serialize( binary_data );
	num_bytes += ex_.Serialize( binary_data );
	num_bytes += ey_.Serialize( binary_data );
	num_bytes += ez_.Serialize( binary_data );

	return num_bytes;
}