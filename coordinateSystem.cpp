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

CoordinateSystem::CoordinateSystem( const PrimitiveVector3 origin_, const PrimitiveVector3 ex_, const PrimitiveVector3 ey_, const PrimitiveVector3 ez_,
					const CoordinateSystem* const parent_, const string name_ )
	: PrimitiveCoordinateSystem{ origin_, ex_, ey_, ez_ },
	parent_( parent_ ),
	name_( name_ ){};

CoordinateSystem::CoordinateSystem( void )
	: CoordinateSystem( PrimitiveVector3{ Tuple3D{ 0, 0, 0 } }, PrimitiveVector3{ Tuple3D{ 1, 0, 0 } }, PrimitiveVector3{ Tuple3D{ 0, 1, 0 } }, PrimitiveVector3{ Tuple3D{ 0, 0, 1 } }, nullptr, "Uninitialised system" ){};

string CoordinateSystem::ToString( const unsigned int newline_tabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) newLine += '\t';

	str += newLine + name_;
	str += newLine + PrimitiveCoordinateSystem::ToString();
	str += newLine + "parent coordinate_system_:" + std::format( "{:#X}", reinterpret_cast<std::uintptr_t>( parent_ ) );
	return str;
}


CoordinateSystem* CoordinateSystem::CreateCopy( const string newName ) const{
	// only one global coordinate system allowed! Parent of copy will be global system
	if (this->IsGlobal()) {
		return CoordinateSystems().AddSystem(origin_, ex_, ey_, ez_, this, newName);
	}

	return CoordinateSystems().AddSystem( origin_, ex_, ey_, ez_, parent_, newName );
}

void CoordinateSystem::CopyPrimitiveFrom( const CoordinateSystem* const sourceCSys ){

	origin_ = sourceCSys->origin_;
	ex_ = sourceCSys->ex_;
	ey_ = sourceCSys->ey_;
	ez_ = sourceCSys->ez_;

	parent_ = sourceCSys->parent_;

}

CoordinateSystem* CoordinateSystem::AddCoordinateSystem( const PrimitiveVector3 origin_, const PrimitiveVector3 ex_, const PrimitiveVector3 ey_, const PrimitiveVector3 ez_, const string name_ ) const{
	return CoordinateSystems().AddSystem( origin_, ex_, ey_, ez_, this, name_ );
}

vector<const CoordinateSystem *> CoordinateSystem::GetPathFromGlobal( void ) const{
	vector<const CoordinateSystem *> path;

	if( this->IsGlobal() ) return path;

	const CoordinateSystem *cur_cSys = this->parent_;	// Start with parent of this system

	// Loop while current coordinate system is not the global system
	while( !cur_cSys->IsGlobal() ){
		path.push_back( cur_cSys );
		cur_cSys = cur_cSys->parent_;
	}

	// Reverse order to get path of coordinate systems traverse
	std::reverse( path.begin(), path.end() );

	return path;
}


Point3D CoordinateSystem::GetOriginPoint( void ) const{
	return Point3D{ Tuple3D{0, 0, 0}, this };
}

Point3D CoordinateSystem::GetOriginInParentSystem( void ) const{
	if( this->IsGlobal() ) return Point3D{ origin_, this };
	return Point3D{ origin_, parent_ };
}

UnitVector3D CoordinateSystem::GetEx( void ) const{
	return UnitVector3D{ Tuple3D{1, 0, 0},  this };
}

UnitVector3D CoordinateSystem::GetEy( void ) const{
	return UnitVector3D{ Tuple3D{0, 1, 0},   this };
}

UnitVector3D CoordinateSystem::GetEz( void ) const{
	return UnitVector3D{ Tuple3D{0, 0, 1},  this };
}

Line CoordinateSystem::GetXAxis( void ) const{
	const CoordinateSystem* parent_ptr = parent_;
	if( this->IsGlobal() ) parent_ptr = GlobalSystem();

	return Line{ Vector3D{ex_, parent_ptr},  Point3D{origin_, parent_ptr} };
}

Line CoordinateSystem::GetYAxis( void ) const{
	const CoordinateSystem* parent_ptr = parent_;
	if( this->IsGlobal() ) parent_ptr = GlobalSystem();

	return Line{ Vector3D{ey_, parent_ptr},  Point3D{origin_, parent_ptr} };
}

Line CoordinateSystem::GetZAxis( void ) const{
	const CoordinateSystem* parent_ptr = parent_;
	if( this->IsGlobal() ) parent_ptr = GlobalSystem();

	return Line{ Vector3D{ez_, parent_ptr},  Point3D{origin_, parent_ptr} };
}

Surface CoordinateSystem::GetXYPlane( void ) const{
	const CoordinateSystem* parent_ptr = parent_;
	if( this->IsGlobal() ) parent_ptr = GlobalSystem();

	return Surface{ Vector3D{ex_, parent_ptr}, Vector3D{ey_, parent_ptr}, Point3D{origin_, parent_ptr } };
}

Surface CoordinateSystem::GetYZPlane( void ) const{
	const CoordinateSystem* parent_ptr = parent_;
	if( this->IsGlobal() ) parent_ptr = GlobalSystem();

	return Surface{ Vector3D{ey_, parent_ptr}, Vector3D{ez_, parent_ptr}, Point3D{origin_, parent_ptr } };
}

Surface CoordinateSystem::GetXZPlane( void ) const{
	const CoordinateSystem* parent_ptr = parent_;
	if( this->IsGlobal() ) parent_ptr = GlobalSystem();

	return Surface{ Vector3D{ex_, parent_ptr}, Vector3D{ez_, parent_ptr}, Point3D{origin_, parent_ptr } };
}

MathematicalObject::MathError CoordinateSystem::Translate( const Vector3D dV ){
	if( this->IsGlobal() ){
		return CheckForAndOutputError( MathError::Operation, "Global coordinate system cannot be translated!" );
	}

	PrimitiveCoordinateSystem::Translate( dV.GetComponents( parent_ ) );

	return MathError::Ok;
}

MathematicalObject::MathError CoordinateSystem::Rotate( const UnitVector3D n, const double phi ){
	if( this->IsGlobal() ){
		return CheckForAndOutputError( MathError::Operation, "Global coordinate system cannot be rotated!" );
	}

	return PrimitiveCoordinateSystem::Rotate( n.GetComponents( parent_ ), phi );
}

MathematicalObject::MathError CoordinateSystem::Rotate( const Line l, const double phi ){
	MathError tErr = MathError::Ok;
	MathError errCode = MathError::Ok;

	// Rotate coordinate system's unit vectors
	if( ( tErr = Rotate( l.direction(), phi ) ) != MathError::Ok ) errCode = tErr;

	// Move rotation center to origin_ of rotation axis
	Translate( -l.origin() );

	// Rotate position vector of origin_ around rotation axis
	if( ( tErr = origin_.Rotate( l.direction().GetComponents( parent_ ), phi ) ) != MathError::Ok ) errCode = tErr;

	// translate back
	Translate( l.origin() );

	return errCode;
}

void CoordinateSystem::SetPrimitive( const PrimitiveCoordinateSystem primitiveCSys ){

	this->origin_ = primitiveCSys.origin();
	this->ex_ = primitiveCSys.ex();
	this->ey_ = primitiveCSys.ey();
	this->ez_ = primitiveCSys.ez();

}

size_t CoordinateSystem::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;

	num_bytes += GetOriginPoint().GetGlobalComponents().Serialize( binary_data );

	num_bytes += GetEx().GetGlobalComponents().Serialize( binary_data );
	num_bytes += GetEy().GetGlobalComponents().Serialize( binary_data );
	num_bytes += GetEz().GetGlobalComponents().Serialize( binary_data );
	
	num_bytes += SerializeBuildIn<string>( name_, binary_data );

	return num_bytes;

}