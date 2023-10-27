/*********************************************************************
 * @file   vec3D3D.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


/*********************************************************************
	Includes
 *********************************************************************/

#include "vector3D.h"
#include "line.h"
#include "surface.h"



/*********************************************************************
   Implementations
*********************************************************************/


/*
	vec3D implementation
*/

Vector3D::Vector3D( const Coordinates coords ) :
	Coordinates{ coords }
{
	UpdateLength();
}

Vector3D::Vector3D( const Tuple3D xyz_, const CoordinateSystem* const coordinate_system ) :
	Vector3D{ Coordinates{ xyz_, coordinate_system } }
{}

Vector3D::Vector3D( void ) : Vector3D{ Coordinates{} }
{}

string Vector3D::ToString( [[maybe_unused]] const unsigned int newline_tabulators ) const{
	string str;
	char tempCharArr[ 256 ];
	snprintf( tempCharArr, 256, "(%.6f,%.6f,%.6f) l=%.6f", X(), Y(), Z(), length() );

	str += tempCharArr;
	return str;
}

double Vector3D::operator* ( const Vector3D v2 ) const{
	Vector3D convVec = v2.ConvertTo( this->coordinate_system_ );

	return x * convVec.x + y * convVec.y + z * convVec.z;
};

Vector3D Vector3D::operator^( const Vector3D v2 ) const{
	Vector3D convVec = v2.ConvertTo( this->coordinate_system_ );

	return Vector3D{ Tuple3D{ y * convVec.z - z * convVec.y, z * convVec.x - x * convVec.z, x * convVec.y - y * convVec.x }, this->coordinate_system_ };
};

bool Vector3D::HasSameSystem( const Vector3D v ) const{
	return this->Coordinates::HasSameSystem( v );
}

Vector3D Vector3D::ConvertTo( const CoordinateSystem* const target_coordinate_system ) const{

	if( this->coordinate_system_ == target_coordinate_system ) return *this;	// Same system return copy of this

	Coordinates tipCoords = this->Coordinates::ConvertTo( target_coordinate_system );
	Point3D tipPoint{ tipCoords, target_coordinate_system };

	Vector3D convVec = tipPoint - this->coordinate_system_->GetOriginInParentSystem();

	return convVec;
};

Vector3D Vector3D::ConvertTo( const Vector3D v ) const{
	return this->ConvertTo( v.coordinate_system_ );
};

Vector3D Vector3D::ConvertTo( const Line l ) const{
	return this->ConvertTo( l.direction() );
};

Vector3D Vector3D::ConvertTo( const Surface s ) const{
	return this->ConvertTo( s.direction_1() );
};

PrimitiveVector3 Vector3D::GetComponents( const CoordinateSystem* const target ) const{
	return Vector3D::ConvertTo( target ).GetComponents();
}

PrimitiveVector3 Vector3D::GetComponents( const Vector3D targetV ) const{
	return Vector3D::ConvertTo( targetV.coordinate_system_ ).GetComponents();;
}

PrimitiveVector3 Vector3D::GetGlobalComponents( void ) const{
	return this->GetComponents( GlobalSystem() );
};

double Vector3D::GetGlobalX( void ) const{
	return GetGlobalComponents().x;
}

double Vector3D::GetGlobalY( void ) const{
	return GetGlobalComponents().y; 
}

double Vector3D::GetGlobalZ( void ) const{
	return GetGlobalComponents().z; 
}

MathematicalObject::MathError Vector3D::UpdateLength( void ){
	// Reset error flag
	errno = 0;

	// New length_
	length_ = sqrt( pow( x, 2 ) + pow( y, 2 ) + pow( z, 2 ) );

	// Check error flag from sqrt()
	if( errno != 0 ) return CheckForAndOutputError( MathError::General, "Error calculating square root!" );
	return MathError::Ok;
}

MathematicalObject::MathError Vector3D::Scale( const double scalar ){
	// Scale each component
	PrimitiveVector3::Scale( scalar );

	// Update vec3Dtors length_
	return UpdateLength();
}

MathematicalObject::MathError Vector3D::Normalise( void ){
	MathematicalObject::MathError tErr = MathError::Ok;
	MathematicalObject::MathError err = MathError::Ok;

	if( ( tErr = PrimitiveVector3::Normalise() ) != MathError::Ok ) err = tErr;
	if( ( tErr = UpdateLength() ) != MathError::Ok ) err = tErr;

	// Scale and return error code
	return err;
}

MathematicalObject::MathError Vector3D::AddToX( const double x_ ){
	PrimitiveVector3::AddToX( x_ );
	return UpdateLength();
};

MathematicalObject::MathError Vector3D::AddToY( const double y_ ){
	PrimitiveVector3::AddToY( y_ );
	return UpdateLength();
};

MathematicalObject::MathError Vector3D::AddToZ( const double z_ ){
	PrimitiveVector3::AddToZ( z_ );
	return UpdateLength();
};

double Vector3D::GetAngle( const Vector3D v2 ) const{
	// Check if one vec3Dtor has length_ zero
	if( IsNearlyEqualDistance( length_, 0 ) || IsNearlyEqualDistance( v2.length_, 0 ) ) return 0.;

	const double scalarProduct = ( ( *this ) * v2 );
	const double lengthMult = this->length_ * v2.length_;

	if( IsNearlyEqualDistance( scalarProduct, lengthMult ) ) return 0.;

	// Return GetAngle in radians
	return acos( scalarProduct / lengthMult );
}

bool Vector3D::IsOrthogonal( const Vector3D v2 ) const{
	// One vec3Dtor has no length_?
	if( IsNearlyEqualDistance( this->length_, 0 ) || IsNearlyEqualDistance( v2.length_, 0 ) ) return false;

	// Check dot product
	return IsNearlyEqualDistance( ( *this ) * v2, 0 );
}

MathematicalObject::MathError Vector3D::RotateAroundXAxis( const double sinPhi, const double cosPhi ){
	PrimitiveVector3::RotateAroundXAxis( sinPhi, cosPhi );
	return UpdateLength();
}

MathematicalObject::MathError Vector3D::RotateAroundYAxis( const double sinPhi, const double cosPhi ){
	PrimitiveVector3::RotateAroundYAxis( sinPhi, cosPhi );
	return UpdateLength();
}

MathematicalObject::MathError Vector3D::RotateAroundZAxis( const double sinPhi, const double cosPhi ){
	PrimitiveVector3::RotateAroundZAxis( sinPhi, cosPhi );
	return UpdateLength();
}

MathematicalObject::MathError Vector3D::RotateAroundXAxis( const double phi ){
	return RotateAroundXAxis( sin( phi ), cos( phi ) );
}

MathematicalObject::MathError Vector3D::RotateAroundYAxis( const double phi ){
	return RotateAroundYAxis( sin( phi ), cos( phi ) );
}

MathematicalObject::MathError Vector3D::RotateAroundZAxis( const double phi ){
	return RotateAroundZAxis( sin( phi ), cos( phi ) );
}

MathematicalObject::MathError Vector3D::Rotate( const Vector3D n, const double phi ){
	Vector3D nCpy{ n.ConvertTo( *this ) };

	MathematicalObject::MathError tErr = MathError::Ok;
	MathematicalObject::MathError err = MathError::Ok;

	if( ( tErr = this->PrimitiveVector3::Rotate( nCpy, phi ) ) != MathError::Ok ) err = tErr;
	if( ( tErr = this->UpdateLength() ) != MathError::Ok ) err = tErr;

	return err;
}

Vector3D Vector3D::RotateConstant( const Vector3D n, const double phi ) const{
	Vector3D v( *this );
	v.Rotate( n, phi );
	return v;
}

Vector3D Vector3D::ProjectOnXYPlane( const CoordinateSystem* const coordinate_system ) const{
	Vector3D projectedVec = this->ConvertTo( coordinate_system );
	projectedVec.AddToZ( -projectedVec.z );

	return projectedVec;
}


Vector3D Vector3D::NegateXComponent( void ) const{
	return Vector3D{ Tuple3D{ -this->x, this->y, this->z }, this->coordinate_system_ };
}


/*
	pnt3D implementation
*/

string Point3D::ToString( [[maybe_unused]] const unsigned int newline_tabulators ) const{
	string str;

	char tempCharArr[ 256 ];
	snprintf( tempCharArr, 256, "(%.6f,%.6f,%.6f)", x, y, z );

	str += tempCharArr;
	return str;
}

Point3D Point3D::ConvertTo( const CoordinateSystem* const target_coordinate_system ) const{

	if( this->coordinate_system_ == target_coordinate_system ) return *this;

	Coordinates convCoords = this->Coordinates::ConvertTo( target_coordinate_system );

	return Point3D( convCoords, target_coordinate_system );


};

Point3D Point3D::ConvertTo( const Point3D targetP ) const{
	return Point3D::ConvertTo( targetP.coordinate_system_ );
}

PrimitiveVector3 Point3D::GetComponents( const CoordinateSystem* const target ) const{
	return Point3D::ConvertTo( target ).GetComponents();
}

PrimitiveVector3 Point3D::GetComponents( const Point3D targetP ) const{
	return Point3D::GetComponents( targetP.coordinate_system_ );
}

PrimitiveVector3 Point3D::GetGlobalComponents( void ) const{
	return this->Point3D::ConvertTo( GlobalSystem() ).GetComponents();
}

double Point3D::GetGlobalX( void ) const{
	return this->Point3D::GetGlobalComponents().x;
}

double Point3D::GetGlobalY( void ) const{
	return this->Point3D::GetGlobalComponents().y;
}

double Point3D::GetGlobalZ( void ) const{
	return this->Point3D::GetGlobalComponents().z;
}

Point3D Point3D::ProjectOnXYPlane( const CoordinateSystem* const coordinate_system ) const{
	Point3D projectedPnt = this->ConvertTo( coordinate_system );
	projectedPnt.AddToZ( -projectedPnt.z );

	return projectedPnt;
}