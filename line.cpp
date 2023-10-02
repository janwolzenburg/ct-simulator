/*********************************************************************
 * @file   line.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


/*********************************************************************
	Includes
 *********************************************************************/
#include <string>
using std::string;

#include "cartesian.h"
#include "cSysTree.h"
#include "vec3D.h"
#include "line.h"
#include "surf.h"



/*********************************************************************
   Implementations
*********************************************************************/


/*
	line implementation
*/

line::line( const vec3 v, const pnt3 p )
	: r( uvec3{ v } ),
	o( p ){
	if( iseqErr( r.Length(), 0 ) ) checkErr( MATH_ERR::INPUT, "Trajectory vector must have length!" );
	if( !v.sameSystem( p ) ) checkErr( MATH_ERR::INPUT, "Line origin and trajectory must be defined in the same coordinate system!" );
}

line::line( void ) : 
	line{ vec3{ v3{1, 0, 0}, DUMMY_CSYS() }, pnt3{ v3{ 0, 0, 0 }, DUMMY_CSYS() }}
{

}

string line::toStr( unsigned int newLineTabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newLineTabulators; i++ ) newLine += '\t';

	str += "r=" + r.toStr() + newLine + "u=" + o.toStr();
	return str;
}

pnt3 line::setOrigin( const pnt3 newO ){ 
	return o = newO.convertTo( r ); 
};

line line::convertTo( const cartCSys* const target ) const{
	return line{ r.convertTo( target ), o.convertTo( target ) };
};

pnt3 line::getPnt( const double linPara ) const{
	pnt3 p{ o + ( r * linPara ) };
	return  p;
}

double line::getPara( const pnt3 p, bool* const success ) const{
	pnt3 cP = p.convertTo( o );

	double t = ( cP.X() - o.X() ) / r.X();

	*success = ( getPnt( t ) == cP );
	return t;
}

double line::getAngle( const surf s ) const{

	// Angle between direction vector and surface normal
	double angle = r.angle( s.Normal() );

	return abs( PI / 2 - angle );
};

vec3 line::getLot( const pnt3 p ) const{
	vec3 vP{ p.convertTo( r.CSys() ) };

	double linePara = ( r * vP - r * o );
	vec3 s{ o + r * linePara };
	return s - vP;
}

double line::getDistance( const pnt3 p ) const{
	return getLot( p ).Length();
};

double line::getDistance( const line l ) const{
	vec3 n{ r ^ l.r };
	return abs( ( l.o - o ) * n ) / n.Length();
}

line line::projectOnXYPlane( const cartCSys* const cSys ) const{
	return line{ this->r.projectOnXYPlane( cSys ), this->o.projectOnXYPlane( cSys ) };
}