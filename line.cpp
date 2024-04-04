/*********************************************************************
 * @file   Line.cpp
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
#include "surface.h"



/*********************************************************************
   Implementations
*********************************************************************/


/*
	Line implementation
*/

Line::Line( const Unitvector3D v, const Point3D p ) : 
	direction_( v ), origin_( p )
{
	if( !v.HasSameSystem( p ) ) CheckForAndOutputError( MathError::Input, "line origin and trajectory must be defined in the same coordinate system!" );
}

Line::Line( const vector3D v, const Point3D p ) : 
	direction_( Unitvector3D{ v } ), origin_( p )
{
	if( IsNearlyEqualDistance( direction_.length(), 0 ) ) CheckForAndOutputError( MathError::Input, "trajectory vector must have length!" );
	if( !v.HasSameSystem( p ) ) CheckForAndOutputError( MathError::Input, "line origin and trajectory must be defined in the same coordinate system!" );
}

Line::Line( void ) : 
	Line{ vector3D{ Tuple3D{1, 0, 0}, DummySystem() }, Point3D{ Tuple3D{ 0, 0, 0 }, DummySystem() }}
{

}

string Line::ConvertToString( unsigned int newline_tabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) newLine += '\t';

	str += "r=" + direction_.ConvertToString() + newLine + "u=" + origin_.ConvertToString();
	return str;
}

Point3D Line::GetPointFast( const double line_parameter ) const{

	return Point3D{ Tuple3D{
		origin_.X() + direction_.X() * line_parameter,
		origin_.Y() + direction_.Y() * line_parameter,
		origin_.Z() + direction_.Z() * line_parameter
	}, this->direction_.GetCoordinateSystem() };

}

double Line::GetLineParameter( const Point3D p, bool* const solution_found_ ) const{
	Point3D cP = p.ConvertTo( origin_ );

	double t = ( cP.X() - origin_.X() ) / direction_.X();

	*solution_found_ = ( GetPoint( t ) == cP );
	return t;
}

double Line::GetAngle( const Surface& s ) const{

	// angle between direction vector and surface normal
	const double arc_angle = direction_.GetAngle( s.GetNormal() );

	return abs( PI / 2 - arc_angle );
};

vector3D Line::GetLot( const Point3D p ) const{
	vector3D vP{ p.ConvertTo( direction_.GetCoordinateSystem() ) };

	double linePara = ( direction_ * vP - direction_ * origin_ );
	vector3D s{ origin_ + direction_ * linePara };
	return s - vP;
}


double Line::GetDistance( const Line l ) const{
	vector3D n{ direction_ ^ l.direction_ };
	return abs( ( l.origin_ - origin_ ) * n ) / n.length();
}

Line Line::ProjectOnXYPlane( const CoordinateSystem* const cSys ) const{
	return Line{ this->direction_.ProjectOnXYPlane( cSys ), this->origin_.ProjectOnXYPlane( cSys ) };
}