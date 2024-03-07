/*********************************************************************
 * @file   generelMath.cpp
 * @brief  Implementations for generel mathematical objects
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/

/*********************************************************************
	Includes
 *********************************************************************/

#include <cmath>

#include "generelMath.h"
#include "simulation.h"



/*********************************************************************
	Implementations
 *********************************************************************/


bool IsNearlyEqual( const double a, const double b, 
										const double tolerance, const ComparisonMode mode ){
	// Fast comparison
	if( a == b ) return true;

	// Absolute comparison
	if( mode == Absolute ){
		return std::abs( a - b ) < tolerance;
	}

	// Relative comparison
	return RelativeDeviation( a, b ) < tolerance;
}

bool IsNearlyEqualDistance( const double a, const double b ){
	return IsNearlyEqual( a, b, max_distance_error_for_equality_mm, ComparisonMode::Absolute );
}


/*
	mathObj implementation
*/


MathematicalObject::MathError MathematicalObject::CheckForAndOutputError( const MathError code ) const{
	// Errorcode is not "OK"?
	if( code != MathError::Ok ){
		// Write to error stream
		math_error_out << '\n' << GetErrorString( code ) << " from " << typeid( *this ).name() << '\n';
	}

	return code;
}

MathematicalObject::MathError MathematicalObject::CheckForAndOutputError( const MathError code, const std::string message ) const{
	CheckForAndOutputError( code );
	math_error_out << message << '\n';

	return code;
}

std::string MathematicalObject::GetErrorString( const MathError code ) const{
	// Convert error code to string
	switch( code ){
		case MathError::Ok:
		return std::string( "No error" );

		case MathError::General:
		return std::string( "Math error" );

		case MathError::Operation:
		return std::string( "Math operation error" );

		case MathError::Bounds:
		return std::string( "Out of bounds error" );

		case MathError::Input:
		return std::string( "Invalid input error" );

		default:
		return std::string( "Unknown math error" );
	}
}

void MathematicalObject::Dump( std::ostream& output, const std::string objName ) const{
	// Write type, name and data to outstream

	output << typeid( *this ).name() << ' ' << objName << '\n' << this->ToString() << '\n' << '\n';
}