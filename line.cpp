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

#include "coordinateSystemTree.h"
#include "vector3D.h"
#include "line.h"
#include "surf.h"



/*********************************************************************
   Implementations
*********************************************************************/


/*
	line implementation
*/

line::line( const UnitVector3D v, const Point3D p ) : 
	r( v ), o( p )
{
	if( !v.IsSameSystem( p ) ) CheckForAndOutputError( MathError::Input, "Line origin_ and trajectory must be defined in the same coordinate system!" );
}

line::line( const Vector3D v, const Point3D p ) : 
	r( UnitVector3D{ v } ), o( p )
{
	if( IsNearlyEqualDistance( r.Length(), 0 ) ) CheckForAndOutputError( MathError::Input, "Trajectory vector must have length_!" );
	if( !v.IsSameSystem( p ) ) CheckForAndOutputError( MathError::Input, "Line origin_ and trajectory must be defined in the same coordinate system!" );
}

line::line( void ) : 
	line{ Vector3D{ Tuple3D{1, 0, 0}, DummySystem() }, Point3D{ Tuple3D{ 0, 0, 0 }, DummySystem() }}
{

}

string line::ToString( unsigned int newline_tabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) newLine += '\t';

	str += "r=" + r.ToString() + newLine + "u=" + o.ToString();
	return str;
}


double line::getPara( const Point3D p, bool* const success ) const{
	Point3D cP = p.ConvertTo( o );

	double t = ( cP.X() - o.X() ) / r.X();

	*success = ( getPnt( t ) == cP );
	return t;
}

double line::getAngle( const surf s ) const{

	// Angle between direction vector and surface normal
	double angle = r.GetAngle( s.Normal() );

	return abs( PI / 2 - angle );
};

Vector3D line::getLot( const Point3D p ) const{
	Vector3D vP{ p.ConvertTo( r.GetCoordinateSystem() ) };

	double linePara = ( r * vP - r * o );
	Vector3D s{ o + r * linePara };
	return s - vP;
}


double line::getDistance( const line l ) const{
	Vector3D n{ r ^ l.r };
	return abs( ( l.o - o ) * n ) / n.Length();
}

line line::projectOnXYPlane( const CoordinateSystem* const cSys ) const{
	return line{ this->r.ProjectOnXYPlane( cSys ), this->o.ProjectOnXYPlane( cSys ) };
}