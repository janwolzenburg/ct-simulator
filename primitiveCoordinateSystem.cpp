/*********************************************************************
 * @file   primitiveCoordinateSystem.cpp
 * @brief  implementations
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

PrimitiveCoordinateSystem::PrimitiveCoordinateSystem( const PrimitiveVector3 origin, const PrimitiveVector3 ex, const PrimitiveVector3 ey, const PrimitiveVector3 ez ) : origin_( origin ),
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

PrimitiveCoordinateSystem::PrimitiveCoordinateSystem( const vector<char>& binary_data, vector<char>::const_iterator& current_byte ) : 
	origin_( DeSerialize<PrimitiveVector3>( binary_data, current_byte ) ),
	ex_( DeSerialize<PrimitiveVector3>( binary_data, current_byte ) ),
	ey_( DeSerialize<PrimitiveVector3>( binary_data, current_byte ) ),
	ez_( DeSerialize<PrimitiveVector3>( binary_data, current_byte ) )
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
	string new_string;
	string new_line = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) new_line += '\t';

	new_string += new_line + "p=" + origin_.ConvertToString();
	new_string += new_line + "ex_=" + ex_.ConvertToString() + new_line + "ey_=" + ey_.ConvertToString() + new_line + "ez_=" + ez_.ConvertToString();
	return new_string;
}

MathematicalObject::MathError PrimitiveCoordinateSystem::Rotate( const PrimitiveVector3 axis, const double angle ){
	MathError tempory_error = MathError::Ok;
	MathError error_code = MathError::Ok;

	if( ( tempory_error = ex_.Rotate( axis, angle ) ) != MathError::Ok ) error_code = tempory_error;
	if( ( tempory_error = ey_.Rotate( axis, angle ) ) != MathError::Ok ) error_code = tempory_error;
	if( ( tempory_error = ez_.Rotate( axis, angle ) ) != MathError::Ok ) error_code = tempory_error;

	return error_code;
}


size_t PrimitiveCoordinateSystem::Serialize( vector<char>& binary_data ) const{

	size_t number_of_bytes = 0;
	number_of_bytes += origin_.Serialize( binary_data );
	number_of_bytes += ex_.Serialize( binary_data );
	number_of_bytes += ey_.Serialize( binary_data );
	number_of_bytes += ez_.Serialize( binary_data );

	return number_of_bytes;
}