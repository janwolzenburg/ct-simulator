/*********************************************************************
 * @file   surface.cpp
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
#include "serialization.h"


/*********************************************************************
   Implementations
*********************************************************************/



/*
	Surface implementation
*/

Surface::Surface( const Unitvector3D direction_1, const Unitvector3D direction_2, const Point3D origin )
	: direction_1_( direction_1 ),
	direction_2_( direction_2 ),
	origin_( origin ),
	normal_( direction_1_ ^ direction_2_ )
{
	
	if( !direction_1_.HasSameSystem( origin_ ) || !direction_2_.HasSameSystem( origin_ ) ||
		!direction_1_.HasSameSystem( direction_2_ ) ) CheckForAndOutputError( MathError::Input, "surface origin and trajectories must be defined in the same coordinate system!" );

	if( !direction_1_.IsOrthogonal( direction_2_ ) ) CheckForAndOutputError( MathError::Input, "trajectory vectors must be orthogonal!" );

};

Surface::Surface( const vector<char>& binary_data, vector<char>::const_iterator& current_byte, CoordinateSystem* coordinate_system )
{
	direction_1_ = Unitvector3D{ Tuple3D{ binary_data, current_byte }, coordinate_system };
	direction_2_ = Unitvector3D{ Tuple3D{ binary_data, current_byte }, coordinate_system };
	origin_ = Point3D{ Tuple3D{ binary_data, current_byte }, coordinate_system };

	if( !direction_1_.IsOrthogonal( direction_2_ ) ) CheckForAndOutputError( MathError::Input, "trajectory vectors must be orthogonal!" );

	normal_ = direction_1_ ^ direction_2_;
}

Surface::Surface( void ) : 
Surface( Unitvector3D{ Tuple3D{1,0,0}, GetGlobalSystem() }, Unitvector3D{ Tuple3D{0,1,0}, GetGlobalSystem() }, Point3D{ Tuple3D{0,0,0}, GetGlobalSystem() })
{}

string Surface::ConvertToString( const unsigned int newline_tabulators ) const{
	string new_string;
	string new_line = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) new_line += '\t';

	new_string += "r1=" + direction_1_.ConvertToString() + new_line + "r2=" + direction_2_.ConvertToString() + new_line + "u=" + origin_.ConvertToString();

	return new_string;
}

Surface Surface::ConvertTo( const CoordinateSystem* const coordinate_system ) const{
	return Surface{ direction_1_.ConvertTo(coordinate_system), direction_2_.ConvertTo(coordinate_system), origin_.ConvertTo(coordinate_system) };
}

size_t Surface::Serialize( vector<char>& binary_data ) const{

	size_t number_of_bytes = 0;
	number_of_bytes += direction_1_.GetComponents().Serialize( binary_data );
	number_of_bytes += direction_2_.GetComponents().Serialize( binary_data );
	number_of_bytes += origin_.GetComponents().Serialize( binary_data );

	return number_of_bytes;

}


/*
	BoundedSurface implementation
*/

BoundedSurface::BoundedSurface( const Unitvector3D direction_1, const Unitvector3D direction_2, const Point3D origin,
								const NumberRange parameter_1_range,
								const NumberRange parameter_2_range )
	: Surface{ direction_1, direction_2, origin },
	parameter_1_range_( parameter_1_range ), 
	parameter_2_range_( parameter_2_range )
{}

BoundedSurface::BoundedSurface( const Unitvector3D direction_1, const Unitvector3D direction_2, const Point3D origin,
								const double parameter_1_min, const double parameter_1_max,
								const double parameter_2_min, const double parameter_2_max ) :
	BoundedSurface{ direction_1, direction_2, origin, NumberRange{ parameter_1_min, parameter_1_max }, NumberRange{ parameter_2_min, parameter_2_max } }
{}


BoundedSurface::BoundedSurface( const Surface s,
								const NumberRange parameter_1_range,
								const NumberRange parameter_2_range )
	: BoundedSurface( s.direction_1(), s.direction_2(), s.origin(), parameter_1_range, parameter_2_range )
{}


BoundedSurface::BoundedSurface( const Surface surface,
								const double parameter_1_min, const double parameter_1_max,
								const double parameter_2_min, const double parameter_2_max ) :
	BoundedSurface{ surface, NumberRange{ parameter_1_min, parameter_1_max }, NumberRange{ parameter_2_min, parameter_2_max } }
{}

BoundedSurface::BoundedSurface( const vector<char>& binary_data, vector<char>::const_iterator& current_byte, CoordinateSystem* cSys ) :
	Surface{ binary_data, current_byte, cSys },
	parameter_1_range_{ binary_data, current_byte },
	parameter_2_range_{ binary_data, current_byte }
{}

string BoundedSurface::ConvertToString( const unsigned int newline_tabulators ) const{
	char tempory_character_arrayay[ 256 ];
	snprintf( tempory_character_arrayay, 256, "aMin=%.6f;aMax=%.6f;bMin=%.6f;bMax=%.6f", parameter_1_range_.start(), parameter_1_range_.end(), parameter_2_range_.start(), parameter_2_range_.end() );

	string new_string;
	string new_line = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) new_line += '\t';

	new_string += Surface::ConvertToString() + new_line + tempory_character_arrayay;

	return new_string;
}

Line BoundedSurface::GetCenterNormal( void ) const{

	return Line{ this->GetNormal(), this->GetCenter() };

}

size_t BoundedSurface::Serialize( vector<char>& binary_data ) const{

	size_t number_of_bytes = 0;
	number_of_bytes += Surface::Serialize( binary_data );
	number_of_bytes += parameter_1_range_.Serialize( binary_data );
	number_of_bytes += parameter_2_range_.Serialize( binary_data );

	return number_of_bytes;

}