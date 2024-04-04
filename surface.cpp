/*********************************************************************
 * @file   Surface.cpp
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
#include "serialization.h"


/*********************************************************************
   Implementations
*********************************************************************/



/*
	Surface implementation
*/

Surface::Surface( const Unitvector3D v1, const Unitvector3D v2, const Point3D p )
	: direction_1_( v1 ),
	direction_2_( v2 ),
	origin_( p ),
	normal_( direction_1_ ^ direction_2_ )
{
	
	if( !direction_1_.HasSameSystem( origin_ ) || !direction_2_.HasSameSystem( origin_ ) ||
		!direction_1_.HasSameSystem( direction_2_ ) ) CheckForAndOutputError( MathError::Input, "surface origin and trajectories must be defined in the same coordinate system!" );

	if( !direction_1_.IsOrthogonal( direction_2_ ) ) CheckForAndOutputError( MathError::Input, "trajectory vectors must be orthogonal!" );

};

Surface::Surface( const vector<char>& binary_data, vector<char>::const_iterator& it, CoordinateSystem* cSys )
{
	direction_1_ = Unitvector3D{ Tuple3D{ binary_data, it }, cSys };
	direction_2_ = Unitvector3D{ Tuple3D{ binary_data, it }, cSys };
	origin_ = Point3D{ Tuple3D{ binary_data, it }, cSys };

	if( !direction_1_.IsOrthogonal( direction_2_ ) ) CheckForAndOutputError( MathError::Input, "trajectory vectors must be orthogonal!" );

	normal_ = direction_1_ ^ direction_2_;
}

Surface::Surface( void ) : 
Surface( Unitvector3D{ Tuple3D{1,0,0}, GlobalSystem() }, Unitvector3D{ Tuple3D{0,1,0}, GlobalSystem() }, Point3D{ Tuple3D{0,0,0}, GlobalSystem() })
{}

string Surface::ConvertToString( const unsigned int newline_tabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) newLine += '\t';

	str += "r1=" + direction_1_.ConvertToString() + newLine + "r2=" + direction_2_.ConvertToString() + newLine + "u=" + origin_.ConvertToString();

	return str;
}

Surface Surface::ConvertTo( const CoordinateSystem* const coordinate_system ) const{
	return Surface{ direction_1_.ConvertTo(coordinate_system), direction_2_.ConvertTo(coordinate_system), origin_.ConvertTo(coordinate_system) };
}

size_t Surface::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;
	num_bytes += direction_1_.GetComponents().Serialize( binary_data );
	num_bytes += direction_2_.GetComponents().Serialize( binary_data );
	num_bytes += origin_.GetComponents().Serialize( binary_data );

	return num_bytes;

}


/*
	BoundedSurface implementation
*/

BoundedSurface::BoundedSurface( const Unitvector3D v1, const Unitvector3D v2, const Point3D p,
								const NumberRange direction_1_range,
								const NumberRange direction_2_range )
	: Surface{ v1, v2, p },
	parameter_1_range_( direction_1_range ), 
	parameter_2_range_( direction_2_range )
{}

BoundedSurface::BoundedSurface( const Unitvector3D v1, const Unitvector3D v2, const Point3D p,
								const double parameter_1_min, const double parameter_1_max,
								const double parameter_2_min, const double parameter_2_max ) :
	BoundedSurface{ v1, v2, p, NumberRange{ parameter_1_min, parameter_1_max }, NumberRange{ parameter_2_min, parameter_2_max } }
{}


BoundedSurface::BoundedSurface( const Surface s,
								const NumberRange direction_1_range,
								const NumberRange direction_2_range )
	: BoundedSurface( s.direction_1(), s.direction_2(), s.origin(), direction_1_range, direction_2_range )
{}


BoundedSurface::BoundedSurface( const Surface surface,
								const double parameter_1_min, const double parameter_1_max,
								const double parameter_2_min, const double parameter_2_max ) :
	BoundedSurface{ surface, NumberRange{ parameter_1_min, parameter_1_max }, NumberRange{ parameter_2_min, parameter_2_max } }
{}

BoundedSurface::BoundedSurface( const vector<char>& binary_data, vector<char>::const_iterator& it, CoordinateSystem* cSys ) :
	Surface{ binary_data, it, cSys },
	parameter_1_range_{ binary_data, it },
	parameter_2_range_{ binary_data, it }
{}

string BoundedSurface::ConvertToString( const unsigned int newline_tabulators ) const{
	char tempCharArray[ 256 ];
	snprintf( tempCharArray, 256, "aMin=%.6f;aMax=%.6f;bMin=%.6f;bMax=%.6f", parameter_1_range_.start(), parameter_1_range_.end(), parameter_2_range_.start(), parameter_2_range_.end() );

	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) newLine += '\t';

	str += Surface::ConvertToString() + newLine + tempCharArray;

	return str;
}

Line BoundedSurface::GetCenterNormal( void ) const{

	return Line{ this->GetNormal(), this->GetCenter() };

}

size_t BoundedSurface::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;
	num_bytes += Surface::Serialize( binary_data );
	num_bytes += parameter_1_range_.Serialize( binary_data );
	num_bytes += parameter_2_range_.Serialize( binary_data );

	return num_bytes;

}