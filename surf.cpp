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

#include "coordinateSystemTree.h"
#include "vector3D.h"
#include "line.h"
#include "surf.h"
#include "serialization.h"


/*********************************************************************
   Implementations
*********************************************************************/



/*
	surf implementation
*/

surf::surf( const UnitVector3D v1, const UnitVector3D v2, const Point3D p )
	: r1( v1 ),
	r2( v2 ),
	o( p ){
	
	if( !r1.HasSameSystem( o ) || !r2.HasSameSystem( o ) ||
		!r1.HasSameSystem( r2 ) ) CheckForAndOutputError( MathError::Input, "Surface origin_ and trajectories must be defined in the same coordinate system!" );

	if( !r1.IsOrthogonal( r2 ) ) CheckForAndOutputError( MathError::Input, "Trajectory vectors must be orthogonal!" );
};

surf::surf( const vector<char>& binary_data, vector<char>::const_iterator& it, CoordinateSystem* cSys )
{
	r1 = UnitVector3D{ Tuple3D{ binary_data, it }, cSys };
	r2 = UnitVector3D{ Tuple3D{ binary_data, it }, cSys };
	o = Point3D{ Tuple3D{ binary_data, it }, cSys };

	if( !r1.IsOrthogonal( r2 ) ) CheckForAndOutputError( MathError::Input, "Trajectory vectors must be orthogonal!" );

}

surf::surf( void ) : 
surf( UnitVector3D{ Tuple3D{1,0,0}, GlobalSystem() }, UnitVector3D{ Tuple3D{0,1,0}, GlobalSystem() }, Point3D{ Tuple3D{0,0,0}, GlobalSystem() })
{}

string surf::ToString( const unsigned int newline_tabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) newLine += '\t';

	str += "r1=" + r1.ToString() + newLine + "r2=" + r2.ToString() + newLine + "u=" + o.ToString();

	return str;
}

surf surf::convertTo( const CoordinateSystem* const coordinate_system ) const{
	return surf( r1.ConvertTo( coordinate_system ), r2.ConvertTo( coordinate_system ), o.ConvertTo( coordinate_system ) );
}

size_t surf::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;
	num_bytes += r1.GetComponents().Serialize( binary_data );
	num_bytes += r2.GetComponents().Serialize( binary_data );
	num_bytes += o.GetComponents().Serialize( binary_data );

	return num_bytes;

}


/*
	surfLim implementation
*/

surfLim::surfLim( const UnitVector3D v1, const UnitVector3D v2, const Point3D p,
				  const double aMin, const double aMax,
				  const double bMin, const double bMax )
	: surf( v1, v2, p ),
	pAMin( aMin ), pAMax( aMax ),
	pBMin( bMin ), pBMax( bMax ){
	// Check limits
	if( pAMin >= pAMax ) CheckForAndOutputError( MathError::Input, "Minimum limit A must be smaller than maximum limit!" );
	if( pBMin >= pBMax ) CheckForAndOutputError( MathError::Input, "Minimum limit B must be smaller than maximum limit!" );
};

surfLim::surfLim( const surf s,
				  const double aMin, const double aMax,
				  const double bMin, const double bMax )
	: surfLim( s.R1(), s.R2(), s.O(),
			   aMin, aMax, bMin, bMax ){}


surfLim::surfLim( const vector<char>& binary_data, vector<char>::const_iterator& it, CoordinateSystem* cSys ) :
	surf{ binary_data, it, cSys },
	pAMin( DeSerializeBuildIn( (double) -1., binary_data, it ) ),
	pAMax( DeSerializeBuildIn( (double) -1., binary_data, it ) ),
	pBMin( DeSerializeBuildIn( (double) -1., binary_data, it ) ),
	pBMax( DeSerializeBuildIn( (double) -1., binary_data, it ) )
{

}

string surfLim::ToString( const unsigned int newline_tabulators ) const{
	char tempCharArray[ 256 ];
	snprintf( tempCharArray, 256, "aMin=%.6f;aMax=%.6f;bMin=%.6f;bMax=%.6f", pAMin, pAMax, pBMin, pBMax );

	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) newLine += '\t';

	str += surf::ToString() + newLine + tempCharArray;

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