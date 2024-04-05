/*********************************************************************
 * @file   line.cpp
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

Line::Line( const Unitvector3D direction, const Point3D origin ) : 
	direction_( direction ), origin_( origin )
{
	if( !direction.HasSameSystem( origin ) ) CheckForAndOutputError( MathError::Input, "line origin and trajectory must be defined in the same coordinate system!" );
}

Line::Line( const vector3D direction, const Point3D origin ) :
	direction_( Unitvector3D{ direction } ), origin_( origin )
{
	if( IsNearlyEqualDistance( direction_.length(), 0 ) ) CheckForAndOutputError( MathError::Input, "trajectory vector must have length!" );
	if( !direction.HasSameSystem( origin ) ) CheckForAndOutputError( MathError::Input, "line origin and trajectory must be defined in the same coordinate system!" );
}

Line::Line( void ) : 
	Line{ vector3D{ Tuple3D{1, 0, 0}, GetDummySystem() }, Point3D{ Tuple3D{ 0, 0, 0 }, GetDummySystem() }}
{

}

string Line::ConvertToString( unsigned int newline_tabulators ) const{
	string new_string;
	string new_line = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) new_line += '\t';

	new_string += "r=" + direction_.ConvertToString() + new_line + "u=" + origin_.ConvertToString();
	return new_string;
}

Point3D Line::GetPointFast( const double line_parameter ) const{

	return Point3D{ Tuple3D{
		origin_.X() + direction_.X() * line_parameter,
		origin_.Y() + direction_.Y() * line_parameter,
		origin_.Z() + direction_.Z() * line_parameter
	}, this->direction_.GetCoordinateSystem() };

}

double Line::GetLineParameter( const Point3D point_on_line, bool* const solution_found ) const{
	const Point3D converted_point = point_on_line.ConvertTo( origin_ );

	const double parameter = ( converted_point.X() - origin_.X() ) / direction_.X();

	*solution_found = ( GetPoint( parameter ) == converted_point );
	return parameter;
}

double Line::GetAngle( const Surface& surface ) const{

	// angle between direction vector and surface normal
	const double rotation_angle = direction_.GetAngle( surface.GetNormal() );

	return abs( PI / 2 - rotation_angle );
};

vector3D Line::GetLot( const Point3D point ) const{
	const vector3D converted_point{ point.ConvertTo( direction_.GetCoordinateSystem() ) };

	const double line_parameter = ( direction_ * converted_point - direction_ * origin_ );
	const vector3D lot_line_intersection_point{ origin_ + direction_ * line_parameter };

	return lot_line_intersection_point - converted_point;
}


double Line::GetDistance( const Line line ) const{
	const vector3D normal{ direction_ ^ line.direction_ };
	return abs( ( line.origin_ - origin_ ) * normal ) / normal.length();
}

Line Line::ProjectOnXYPlane( const CoordinateSystem* const coordinate_system ) const{
	return Line{ this->direction_.ProjectOnXYPlane( coordinate_system ), this->origin_.ProjectOnXYPlane( coordinate_system ) };
}