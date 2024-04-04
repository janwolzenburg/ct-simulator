/*********************************************************************
 * @file   vector3D.cpp
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

vector3D::vector3D( const Coordinates coords ) :
	Coordinates{ coords }
{
	UpdateLength();
}

vector3D::vector3D( const Tuple3D xyz_, const CoordinateSystem* const coordinate_system ) :
	vector3D{ Coordinates{ xyz_, coordinate_system } }
{}

vector3D::vector3D( void ) : vector3D{ Coordinates{} }
{}

string vector3D::ConvertToString( [[maybe_unused]] const unsigned int newline_tabulators ) const{
	string str;
	char tempCharArr[ 256 ];
	snprintf( tempCharArr, 256, "(%.6f,%.6f,%.6f) l=%.6f", X(), Y(), Z(), length() );

	str += tempCharArr;
	return str;
}

double vector3D::operator* ( const vector3D v2 ) const{
	vector3D convVec = v2.ConvertTo( this->coordinate_system_ );

	return x * convVec.x + y * convVec.y + z * convVec.z;
};

vector3D vector3D::operator^( const vector3D v2 ) const{
	vector3D convVec = v2.ConvertTo( this->coordinate_system_ );

	return vector3D{ Tuple3D{ y * convVec.z - z * convVec.y, z * convVec.x - x * convVec.z, x * convVec.y - y * convVec.x }, this->coordinate_system_ };
};

bool vector3D::HasSameSystem( const vector3D v ) const{
	return this->Coordinates::HasSameSystem( v );
}

vector3D vector3D::ConvertTo( const CoordinateSystem* const target_coordinate_system ) const{

	if( this->coordinate_system_ == target_coordinate_system ) return *this;	// same system return copy of this

	Coordinates tipCoords = this->Coordinates::ConvertTo( target_coordinate_system );
	Point3D tipPoint{ tipCoords, target_coordinate_system };

	vector3D convVec = tipPoint - this->coordinate_system_->GetOriginInParentSystem();

	return convVec;
};

vector3D vector3D::ConvertTo( const vector3D v ) const{
	return this->ConvertTo( v.coordinate_system_ );
};

vector3D vector3D::ConvertTo( const Line l ) const{
	return this->ConvertTo( l.direction() );
};

vector3D vector3D::ConvertTo( const Surface s ) const{
	return this->ConvertTo( s.direction_1() );
};

Primitivevector3 vector3D::GetComponents( const CoordinateSystem* const target ) const{
	return vector3D::ConvertTo( target ).GetComponents();
}

Primitivevector3 vector3D::GetComponents( const vector3D targetV ) const{
	return vector3D::ConvertTo( targetV.coordinate_system_ ).GetComponents();;
}

Primitivevector3 vector3D::GetGlobalComponents( void ) const{
	return this->GetComponents( GetGlobalSystem() );
};

double vector3D::GetGlobalX( void ) const{
	return GetGlobalComponents().x;
}

double vector3D::GetGlobalY( void ) const{
	return GetGlobalComponents().y; 
}

double vector3D::GetGlobalZ( void ) const{
	return GetGlobalComponents().z; 
}

MathematicalObject::MathError vector3D::UpdateLength( void ){
	// reset error flag
	errno = 0;

	// new length
	length_ = sqrt( pow( x, 2 ) + pow( y, 2 ) + pow( z, 2 ) );

	// check error flag from sqrt()
	if( errno != 0 ) return CheckForAndOutputError( MathError::General, "error calculating square root!" );
	return MathError::Ok;
}

MathematicalObject::MathError vector3D::Scale( const double scalar ){
	// scale each component
	Primitivevector3::Scale( scalar );

	// update vectors length
	return UpdateLength();
}

MathematicalObject::MathError vector3D::Normalise( void ){
	MathematicalObject::MathError tErr = MathError::Ok;
	MathematicalObject::MathError err = MathError::Ok;

	if( ( tErr = Primitivevector3::Normalise() ) != MathError::Ok ) err = tErr;
	if( ( tErr = UpdateLength() ) != MathError::Ok ) err = tErr;

	// scale and return error code
	return err;
}

MathematicalObject::MathError vector3D::AddToX( const double x_ ){
	Primitivevector3::AddToX( x_ );
	return UpdateLength();
};

MathematicalObject::MathError vector3D::AddToY( const double y_ ){
	Primitivevector3::AddToY( y_ );
	return UpdateLength();
};

MathematicalObject::MathError vector3D::AddToZ( const double z_ ){
	Primitivevector3::AddToZ( z_ );
	return UpdateLength();
};

double vector3D::GetAngle( const vector3D v2 ) const{
	// check if one vector has length zero
	if( IsNearlyEqualDistance( length_, 0 ) || IsNearlyEqualDistance( v2.length_, 0 ) ) return 0.;

	const double scalarProduct = ( ( *this ) * v2 );
	const double lengthMult = this->length_ * v2.length_;

	if( IsNearlyEqualDistance( scalarProduct, lengthMult ) ) return 0.;

	// return angle in radians
	return acos( scalarProduct / lengthMult );
}

bool vector3D::IsOrthogonal( const vector3D v2 ) const{
	// one vector has no length?
	if( IsNearlyEqualDistance( this->length_, 0 ) || IsNearlyEqualDistance( v2.length_, 0 ) ) return false;

	// check dot product
	return IsNearlyEqualDistance( ( *this ) * v2, 0 );
}

MathematicalObject::MathError vector3D::RotateAroundXAxis( const double sinPhi, const double cosine_phi ){
	Primitivevector3::RotateAroundXAxis( sinPhi, cosine_phi );
	return UpdateLength();
}

MathematicalObject::MathError vector3D::RotateAroundYAxis( const double sinPhi, const double cosine_phi ){
	Primitivevector3::RotateAroundYAxis( sinPhi, cosine_phi );
	return UpdateLength();
}

MathematicalObject::MathError vector3D::RotateAroundZAxis( const double sinPhi, const double cosine_phi ){
	Primitivevector3::RotateAroundZAxis( sinPhi, cosine_phi );
	return UpdateLength();
}

MathematicalObject::MathError vector3D::RotateAroundXAxis( const double phi ){
	return RotateAroundXAxis( sin( phi ), cos( phi ) );
}

MathematicalObject::MathError vector3D::RotateAroundYAxis( const double phi ){
	return RotateAroundYAxis( sin( phi ), cos( phi ) );
}

MathematicalObject::MathError vector3D::RotateAroundZAxis( const double phi ){
	return RotateAroundZAxis( sin( phi ), cos( phi ) );
}

MathematicalObject::MathError vector3D::Rotate( const vector3D n, const double phi ){
	vector3D nCpy{ n.ConvertTo( *this ) };

	MathematicalObject::MathError tErr = MathError::Ok;
	MathematicalObject::MathError err = MathError::Ok;

	if( ( tErr = this->Primitivevector3::Rotate( nCpy, phi ) ) != MathError::Ok ) err = tErr;
	if( ( tErr = this->UpdateLength() ) != MathError::Ok ) err = tErr;

	return err;
}

vector3D vector3D::RotateConstant( const vector3D n, const double phi ) const{
	vector3D v( *this );
	v.Rotate( n, phi );
	return v;
}

vector3D vector3D::ProjectOnXYPlane( const CoordinateSystem* const coordinate_system ) const{
	vector3D projectedVec = this->ConvertTo( coordinate_system );
	projectedVec.AddToZ( -projectedVec.z );

	return projectedVec;
}


vector3D vector3D::NegateXComponent( void ) const{
	return vector3D{ Tuple3D{ -this->x, this->y, this->z }, this->coordinate_system_ };
}


/*
	pnt3D implementation
*/

string Point3D::ConvertToString( [[maybe_unused]] const unsigned int newline_tabulators ) const{
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

Primitivevector3 Point3D::GetComponents( const CoordinateSystem* const target ) const{
	return Point3D::ConvertTo( target ).GetComponents();
}

Primitivevector3 Point3D::GetComponents( const Point3D targetP ) const{
	return Point3D::GetComponents( targetP.coordinate_system_ );
}

Primitivevector3 Point3D::GetGlobalComponents( void ) const{
	return this->Point3D::ConvertTo( GetGlobalSystem() ).GetComponents();
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