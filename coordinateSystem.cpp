/*********************************************************************
 * @file   cartesian.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


/*********************************************************************
	Includes
 *********************************************************************/
#include <format>
#include <algorithm>
#include "coordinateSystem.h"
#include "coordinateSystemTree.h"
#include "vector3D.h"
#include "line.h"
#include "surface.h"
#include "serialization.h"


 /*********************************************************************
	Implementations
 *********************************************************************/


/*
	CoordinateSystem implementation
*/

CoordinateSystem::CoordinateSystem( const PrimitiveVector3 origin, const PrimitiveVector3 ex, const PrimitiveVector3 ey, const PrimitiveVector3 ez,
					const CoordinateSystem* const parent, const string name )
	: PrimitiveCoordinateSystem{ origin, ex, ey, ez },
	parent_( parent ),
	name_( name )
{}

CoordinateSystem::CoordinateSystem( void )
	: CoordinateSystem( PrimitiveVector3{ Tuple3D{ 0, 0, 0 } }, PrimitiveVector3{ Tuple3D{ 1, 0, 0 } }, PrimitiveVector3{ Tuple3D{ 0, 1, 0 } }, PrimitiveVector3{ Tuple3D{ 0, 0, 1 } }, nullptr, "Uninitialised system" ){};

string CoordinateSystem::ConvertToString( const unsigned int newline_tabulators ) const{
	string new_string;
	string new_line = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) new_line += '\t';

	new_string += new_line + name_;
	new_string += new_line + PrimitiveCoordinateSystem::ConvertToString();
	new_string += new_line + "parent coordinate_system_:" + std::format( "{:#X}", reinterpret_cast<std::uintptr_t>( parent_ ) );
	return new_string;
}


CoordinateSystem* CoordinateSystem::CreateCopy( const string new_name ) const{
	// only one global coordinate system allowed! parent of copy will be global system
	if (this->IsGlobal()) {
		return GetCoordinateSystemTree().AddSystem(origin_, ex_, ey_, ez_, this, new_name);
	}

	return GetCoordinateSystemTree().AddSystem( origin_, ex_, ey_, ez_, parent_, new_name );
}

void CoordinateSystem::CopyPrimitiveFrom( const CoordinateSystem* const source_system ){

	origin_ = source_system->origin_;
	ex_ = source_system->ex_;
	ey_ = source_system->ey_;
	ez_ = source_system->ez_;

	parent_ = source_system->parent_;

}

CoordinateSystem* CoordinateSystem::AddCoordinateSystem( const PrimitiveVector3 origin, const PrimitiveVector3 ex, const PrimitiveVector3 ey, const PrimitiveVector3 ez, const string name ) const{
	return GetCoordinateSystemTree().AddSystem( origin, ex, ey, ez, this, name );
}

vector<const CoordinateSystem *> CoordinateSystem::GetPathFromGlobal( void ) const{
	vector<const CoordinateSystem *> path_from_global;

	if( this->IsGlobal() ) return path_from_global;

	const CoordinateSystem *current_system = this->parent_;	// start with parent of this system

	// loop while current coordinate system is not the global system
	while( !current_system->IsGlobal() ){
		path_from_global.push_back( current_system );
		current_system = current_system->parent_;
	}

	// reverse order to get path of coordinate systems traverse
	std::reverse( path_from_global.begin(), path_from_global.end() );

	return path_from_global;
}


Point3D CoordinateSystem::GetOriginPoint( void ) const{
	return Point3D{ Tuple3D{0, 0, 0}, this };
}

Point3D CoordinateSystem::GetOriginInParentSystem( void ) const{
	if( this->IsGlobal() ) return Point3D{ origin_, this };
	return Point3D{ origin_, parent_ };
}

Unitvector3D CoordinateSystem::GetEx( void ) const{
	return Unitvector3D{ Tuple3D{1, 0, 0},  this };
}

Unitvector3D CoordinateSystem::GetEy( void ) const{
	return Unitvector3D{ Tuple3D{0, 1, 0},   this };
}

Unitvector3D CoordinateSystem::GetEz( void ) const{
	return Unitvector3D{ Tuple3D{0, 0, 1},  this };
}

Line CoordinateSystem::GetXAxis( void ) const{
	const CoordinateSystem* parent_ptr = parent_;
	if( this->IsGlobal() ) parent_ptr = GetGlobalSystem();

	return Line{ Vector3D{ex_, parent_ptr},  Point3D{origin_, parent_ptr} };
}

Line CoordinateSystem::GetYAxis( void ) const{
	const CoordinateSystem* parent_ptr = parent_;
	if( this->IsGlobal() ) parent_ptr = GetGlobalSystem();

	return Line{ Vector3D{ey_, parent_ptr},  Point3D{origin_, parent_ptr} };
}

Line CoordinateSystem::GetZAxis( void ) const{
	const CoordinateSystem* parent_ptr = parent_;
	if( this->IsGlobal() ) parent_ptr = GetGlobalSystem();

	return Line{ Vector3D{ez_, parent_ptr},  Point3D{origin_, parent_ptr} };
}

Surface CoordinateSystem::GetXYPlane( void ) const{
	const CoordinateSystem* parent_ptr = parent_;
	if( this->IsGlobal() ) parent_ptr = GetGlobalSystem();

	return Surface{ Vector3D{ex_, parent_ptr}, Vector3D{ey_, parent_ptr}, Point3D{origin_, parent_ptr } };
}

Surface CoordinateSystem::GetYZPlane( void ) const{
	const CoordinateSystem* parent_ptr = parent_;
	if( this->IsGlobal() ) parent_ptr = GetGlobalSystem();

	return Surface{ Vector3D{ey_, parent_ptr}, Vector3D{ez_, parent_ptr}, Point3D{origin_, parent_ptr } };
}

Surface CoordinateSystem::GetXZPlane( void ) const{
	const CoordinateSystem* parent_ptr = parent_;
	if( this->IsGlobal() ) parent_ptr = GetGlobalSystem();

	return Surface{ Vector3D{ex_, parent_ptr}, Vector3D{ez_, parent_ptr}, Point3D{origin_, parent_ptr } };
}

MathematicalObject::MathError CoordinateSystem::Translate( const Vector3D direction ){
	if( this->IsGlobal() ){
		return CheckForAndOutputError( MathError::Operation, "global coordinate system cannot be translated!" );
	}

	PrimitiveCoordinateSystem::Translate( direction.GetComponents( parent_ ) );

	return MathError::Ok;
}

MathematicalObject::MathError CoordinateSystem::Rotate( const Unitvector3D axis, const double rotation_angle ){
	if( this->IsGlobal() ){
		return CheckForAndOutputError( MathError::Operation, "global coordinate system cannot be rotated!" );
	}

	return PrimitiveCoordinateSystem::Rotate( axis.GetComponents( parent_ ), rotation_angle );
}

MathematicalObject::MathError CoordinateSystem::Rotate( const Line axis, const double rotation_angle ){
	MathError tempory_error = MathError::Ok;
	MathError error_code = MathError::Ok;

	// rotate coordinate system's unit vectors
	if( ( tempory_error = Rotate( axis.direction(), rotation_angle ) ) != MathError::Ok ) error_code = tempory_error;

	// move rotation center to origin_ of rotation axis
	Translate( -axis.origin() );

	// rotate position vector of origin_ around rotation axis
	if( ( tempory_error = origin_.Rotate( axis.direction().GetComponents( parent_ ), rotation_angle ) ) != MathError::Ok ) error_code = tempory_error;

	// translate back
	Translate( axis.origin() );

	return error_code;
}

void CoordinateSystem::SetPrimitive( const PrimitiveCoordinateSystem new_primitive ){

	this->origin_ = new_primitive.origin();
	this->ex_ = new_primitive.ex();
	this->ey_ = new_primitive.ey();
	this->ez_ = new_primitive.ez();

}

size_t CoordinateSystem::Serialize( vector<char>& binary_data ) const{

	size_t number_of_bytes = 0;

	number_of_bytes += GetOriginPoint().GetGlobalComponents().Serialize( binary_data );

	number_of_bytes += GetEx().GetGlobalComponents().Serialize( binary_data );
	number_of_bytes += GetEy().GetGlobalComponents().Serialize( binary_data );
	number_of_bytes += GetEz().GetGlobalComponents().Serialize( binary_data );
	
	number_of_bytes += SerializeBuildIn<string>( name_, binary_data );

	return number_of_bytes;

}