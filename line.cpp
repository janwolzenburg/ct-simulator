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
#include "vec3D.h"
#include "line.h"
#include "surf.h"



/*********************************************************************
   Implementations
*********************************************************************/



/*
	line implementation
*/

line::line( const vec3D v, const pnt3D p )
	: r( uvec3D{ v } ),
	o( p ){
	if( iseqErr( r.Length(), 0 ) ) checkErr( MATH_ERR::INPUT, "Trajectory vec3Dtor must have length!" );
	if( !v.sameSystem( p ) ) checkErr( MATH_ERR::INPUT, "Line origin and trajectory must be defined in the same coordinate system!" );
}

std::string line::toStr( unsigned int newLineTabulators ) const{
	std::string str;
	std::string newLine = { '\n' };

	for( unsigned int i = 0; i < newLineTabulators; i++ ) newLine += '\t';

	str += "r=" + r.toStr() + newLine + "u=" + o.toStr();
	return str;
}

pnt3D line::O( void ) const { 
	return o;
};

uvec3D line::R( void ) const { 
	return r;
};

line line::convertTo( const cartCSys* const target ) const{
	return line{ r.convertTo( target ), o.convertTo( target ) };
};

pnt3D line::getPnt( const double linPara ) const{
	pnt3D p{ o + ( r * linPara ) };
	return  p;
}

double line::getPara( const pnt3D p, bool* const success ) const{
	pnt3D cP = p.convertTo( o );

	double t = ( cP.X() - o.X() ) / r.X();

	*success = ( getPnt( t ) == cP );
	return t;
}

double line::getAngle( const surf s ) const{
	return PI / 2 - r.angle( s.Normal() );
};

vec3D line::getLot( const pnt3D p ) const{
	vec3D vP{ p.convertTo( r.CSys() ) };

	double linePara = ( r * vP - r * o );
	vec3D s{ o + r * linePara };
	return s - vP;
}

double line::getDistance( const pnt3D p ) const{
	return getLot( p ).Length();
};

double line::getDistance( const line l ) const{
	vec3D n{ r ^ l.r };
	return abs( ( l.o - o ) * n ) / n.Length();
}

line line::projectOnXYPlane( const cartCSys* const cSys ) const{
	return line{ this->r.projectOnXYPlane( cSys ), this->o.projectOnXYPlane( cSys ) };
}