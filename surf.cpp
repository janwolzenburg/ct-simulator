/*********************************************************************
 * @file   surf.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


/*********************************************************************
	Includes
 *********************************************************************/

#include "cartesian.h"
#include "cSysTree.h"
#include "vec3D.h"
#include "line.h"
#include "surf.h"



/*********************************************************************
   Implementations
*********************************************************************/



/*
	surf implementation
*/

surf::surf( const uvec3 v1, const uvec3 v2, const pnt3 p )
	: r1( v1 ),
	r2( v2 ),
	o( p ){
	if( iseqErr( r1.Length(), 0 ) ) checkErr( MATH_ERR::INPUT, "Trajectory vector r1  must have length!" );
	if( iseqErr( r2.Length(), 0 ) ) checkErr( MATH_ERR::INPUT, "Trajectory vector r2 must have length!" );
	if( !r1.sameSystem( o ) || !r2.sameSystem( o ) ||
		!r1.sameSystem( r2 ) ) checkErr( MATH_ERR::INPUT, "Surface origin and trajectories must be defined in the same coordinate system!" );

	if( !r1.isOrtho( r2 ) ) checkErr( MATH_ERR::INPUT, "Trajectory vectors must be orthogonal!" );
};

surf::surf( void ) : 
surf( uvec3{ v3{1,0,0}, GLOBAL_CSYS() }, uvec3{ v3{0,1,0}, GLOBAL_CSYS() }, pnt3{ v3{0,0,0}, GLOBAL_CSYS() })
{}

string surf::toStr( const unsigned int newLineTabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newLineTabulators; i++ ) newLine += '\t';

	str += "r1=" + r1.toStr() + newLine + "r2=" + r2.toStr() + newLine + "u=" + o.toStr();

	return str;
}

pnt3 surf::getPnt( const double surfParaA, const double surfParaB ) const{
	return  o + ( r1 * surfParaA + r2 * surfParaB );
}

uvec3 surf::Normal( void ) const{
	return  r1 ^ r2;
};

surf surf::convertTo( const cartCSys* const cSys_ ) const{
	return surf( r1.convertTo( cSys_ ), r2.convertTo( cSys_ ), o.convertTo( cSys_ ) );
}


/*
	surfLim implementation
*/

surfLim::surfLim( const uvec3 v1, const uvec3 v2, const pnt3 p,
				  const double aMin, const double aMax,
				  const double bMin, const double bMax )
	: surf( v1, v2, p ),
	pAMin( aMin ), pAMax( aMax ),
	pBMin( bMin ), pBMax( bMax ){
	// Check limits
	if( pAMin >= pAMax ) checkErr( MATH_ERR::INPUT, "Minimum limit A must be smaller than maximum limit!" );
	if( pBMin >= pBMax ) checkErr( MATH_ERR::INPUT, "Minimum limit B must be smaller than maximum limit!" );
};

surfLim::surfLim( const surf s,
				  const double aMin, const double aMax,
				  const double bMin, const double bMax )
	: surfLim( s.R1(), s.R2(), s.O(),
			   aMin, aMax, bMin, bMax ){}

surfLim::surfLim( void ) : 
	surfLim( surf{}, 0,1,0,1)
{}

string surfLim::toStr( const unsigned int newLineTabulators ) const{
	char tempCharArray[ 256 ];
	snprintf( tempCharArray, 256, "aMin=%.6f;aMax=%.6f;bMin=%.6f;bMax=%.6f", pAMin, pAMax, pBMin, pBMax );

	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newLineTabulators; i++ ) newLine += '\t';

	str += surf::toStr() + newLine + tempCharArray;

	return str;
}

surfLim surfLim::convertTo( const cartCSys* const cSys_ ) const{
	return surfLim{ this->surf::convertTo( cSys_ ), pAMin, pAMax, pBMin, pBMax };
}

bool surfLim::parasInBounds( const double a, const double b ) const{
	return pAMin <= a && a <= pAMax && pBMin <= b && b <= pBMax;
}

pnt3 surfLim::getCenter( void ) const{
	return this->getPnt( ( pAMax + pAMin ) / 2, ( pBMax + pBMin ) / 2 );
}

line  surfLim::NormalLine( void ) const{

	return line{ this->Normal(), this->getCenter() };

}