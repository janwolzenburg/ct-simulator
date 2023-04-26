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

#include <iostream>
#include <cmath>

#include "generelMath.h"
#include "simulation.h"



/*********************************************************************
	Implementations
 *********************************************************************/


bool iseqErr( const double a, const double b, const double absoluteTolerance ){
	// Fast comparison
	if( a == b ) return true;

	// Inprecise comparison
	if( std::abs( a - b ) < absoluteTolerance ) return true;
	return false;
}

bool isEqErrPercent( const double a, const double b, const double fraction ){

	// Fast comparison
	if( a == b ) return true;

	// Inprecise comparison
	if( std::abs( a - b ) / std::abs( a ) < fraction ) return true;
	return false;

}

bool iseqErr( const double a, const double b ){
	

	return iseqErr( a, b, EPSILON );
}



/*
	mathObj implementation
*/


mathObj::MATH_ERR mathObj::checkErr( const MATH_ERR code ) const{
	// Errorcode is not "OK"?
	if( code != MATH_ERR::OK ){
		// Write to error stream
		mathErrOut << '\n' << errStr( code ) << " from " << typeid( *this ).name() << '\n';
	}

	return code;
}

mathObj::MATH_ERR mathObj::checkErr( const MATH_ERR code, const std::string message ) const{
	checkErr( code );
	mathErrOut << message << '\n';

	return code;
}

std::string mathObj::errStr( const MATH_ERR code ) const{
	// Convert error code to string
	switch( code ){
		case MATH_ERR::OK:
		return std::string( "No error" );

		case MATH_ERR::GENERAL:
		return std::string( "Math error" );

		case MATH_ERR::OPERATION:
		return std::string( "Math operation error" );

		case MATH_ERR::BOUNDS:
		return std::string( "Out of bounds error" );

		case MATH_ERR::INPUT:
		return std::string( "Invalid input error" );

		default:
		return std::string( "Unknown math error" );
	}
}

void mathObj::dump( std::ostream& output, const std::string objName ) const{
	// Write type, name and data to outstream

	output << typeid( *this ).name() << ' ' << objName << '\n' << this->toStr() << '\n' << '\n';
}