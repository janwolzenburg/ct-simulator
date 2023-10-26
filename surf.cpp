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

#include "cSysTree.h"
#include "vec3D.h"
#include "line.h"
#include "surf.h"
#include "serialization.h"


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
	
	if( !r1.sameSystem( o ) || !r2.sameSystem( o ) ||
		!r1.sameSystem( r2 ) ) checkErr( MATH_ERR::INPUT, "Surface origin and trajectories must be defined in the same coordinate system!" );

	if( !r1.isOrtho( r2 ) ) checkErr( MATH_ERR::INPUT, "Trajectory vectors must be orthogonal!" );
};

surf::surf( const vector<char>& binary_data, vector<char>::const_iterator& it, cartCSys* cSys )
{
	r1 = uvec3{ Tuple3D{ binary_data, it }, cSys };
	r2 = uvec3{ Tuple3D{ binary_data, it }, cSys };
	o = pnt3{ Tuple3D{ binary_data, it }, cSys };

	if( !r1.isOrtho( r2 ) ) checkErr( MATH_ERR::INPUT, "Trajectory vectors must be orthogonal!" );

}

surf::surf( void ) : 
surf( uvec3{ Tuple3D{1,0,0}, GLOBAL_CSYS() }, uvec3{ Tuple3D{0,1,0}, GLOBAL_CSYS() }, pnt3{ Tuple3D{0,0,0}, GLOBAL_CSYS() })
{}

string surf::toStr( const unsigned int newLineTabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newLineTabulators; i++ ) newLine += '\t';

	str += "r1=" + r1.toStr() + newLine + "r2=" + r2.toStr() + newLine + "u=" + o.toStr();

	return str;
}

surf surf::convertTo( const cartCSys* const cSys_ ) const{
	return surf( r1.convertTo( cSys_ ), r2.convertTo( cSys_ ), o.convertTo( cSys_ ) );
}

size_t surf::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;
	num_bytes += r1.XYZ().Serialize( binary_data );
	num_bytes += r2.XYZ().Serialize( binary_data );
	num_bytes += o.XYZ().Serialize( binary_data );

	return num_bytes;

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


surfLim::surfLim( const vector<char>& binary_data, vector<char>::const_iterator& it, cartCSys* cSys ) :
	surf{ binary_data, it, cSys },
	pAMin( DeSerializeBuildIn( (double) -1., binary_data, it ) ),
	pAMax( DeSerializeBuildIn( (double) -1., binary_data, it ) ),
	pBMin( DeSerializeBuildIn( (double) -1., binary_data, it ) ),
	pBMax( DeSerializeBuildIn( (double) -1., binary_data, it ) )
{

}

string surfLim::toStr( const unsigned int newLineTabulators ) const{
	char tempCharArray[ 256 ];
	snprintf( tempCharArray, 256, "aMin=%.6f;aMax=%.6f;bMin=%.6f;bMax=%.6f", pAMin, pAMax, pBMin, pBMax );

	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newLineTabulators; i++ ) newLine += '\t';

	str += surf::toStr() + newLine + tempCharArray;

	return str;
}

line  surfLim::NormalLine( void ) const{

	return line{ this->Normal(), this->getCenter() };

}

size_t surfLim::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;
	num_bytes += surf::Serialize( binary_data );
	num_bytes += SerializeBuildIn( pAMin, binary_data );
	num_bytes += SerializeBuildIn( pAMax, binary_data );
	num_bytes += SerializeBuildIn( pBMin, binary_data );
	num_bytes += SerializeBuildIn( pBMax, binary_data );

	return num_bytes;

}