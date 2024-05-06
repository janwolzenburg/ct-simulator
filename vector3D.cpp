/*********************************************************************
 * @file   vector3D.cpp
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
	Vector3D implementation
*/

Vector3D::Vector3D( const Coordinates coordinates ) :
	Coordinates( coordinates )
{
	UpdateLength();
}

Vector3D::Vector3D( const Tuple3D components, const CoordinateSystem* const coordinate_system ) :
	Vector3D{ Coordinates{ components, coordinate_system } }
{}

Vector3D::Vector3D( void ) : Vector3D{ Coordinates{} }
{}

string Vector3D::ConvertToString( [[maybe_unused]] const unsigned int newline_tabulators ) const{
	string new_string;
	char tempory_character_array[ 256 ];
	snprintf( tempory_character_array, 256, "(%.6f,%.6f,%.6f) l=%.6f", X(), Y(), Z(), length() );

	new_string += tempory_character_array;
	return new_string;
}

double Vector3D::operator* ( const Vector3D second_vector ) const{
	const Vector3D converted_vector = second_vector.ConvertTo( this->coordinate_system_ );

	return x * converted_vector.x + y * converted_vector.y + z * converted_vector.z;
};

Vector3D Vector3D::operator^( const Vector3D second_vector ) const{
	const Vector3D converted_vector = second_vector.ConvertTo( this->coordinate_system_ );

	return Vector3D{ Tuple3D{ y * converted_vector.z - z * converted_vector.y, z * converted_vector.x - x * converted_vector.z, x * converted_vector.y - y * converted_vector.x }, this->coordinate_system_ };
};

bool Vector3D::HasSameSystem( const Vector3D vector_to_compare ) const{
	return this->Coordinates::HasSameSystem( vector_to_compare );
}

Vector3D Vector3D::ConvertTo( const CoordinateSystem* const target_coordinate_system ) const{

	if( this->coordinate_system_ == target_coordinate_system ) return *this;	// same system return copy of this

	const Coordinates tip_coordinates = this->Coordinates::ConvertTo( target_coordinate_system );
	const Point3D tip_point{ tip_coordinates, target_coordinate_system };

	const Vector3D converted_vector = tip_point - this->coordinate_system_->GetOriginInParentSystem();

	return converted_vector;
};

Vector3D Vector3D::ConvertTo( const Vector3D target_coordinate_system ) const{
	return this->ConvertTo( target_coordinate_system.coordinate_system_ );
};

Vector3D Vector3D::ConvertTo( const Line target_coordinate_system ) const{
	return this->ConvertTo( target_coordinate_system.direction() );
};

Vector3D Vector3D::ConvertTo( const Surface target_coordinate_system ) const{
	return this->ConvertTo( target_coordinate_system.direction_1() );
};

PrimitiveVector3 Vector3D::GetComponents( const CoordinateSystem* const target_coordinate_system ) const{
	return Vector3D::ConvertTo( target_coordinate_system ).GetComponents();
}

PrimitiveVector3 Vector3D::GetComponents( const Vector3D target_coordinate_system ) const{
	return Vector3D::ConvertTo( target_coordinate_system.coordinate_system_ ).GetComponents();;
}

PrimitiveVector3 Vector3D::GetGlobalComponents( void ) const{
	return this->GetComponents( GetGlobalSystem() );
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
	// reset error flag
	errno = 0;

	// new length
	length_ = sqrt( pow( x, 2 ) + pow( y, 2 ) + pow( z, 2 ) );

	// check error flag from sqrt()
	if( errno != 0 ) return CheckForAndOutputError( MathError::General, "error calculating square root!" );
	return MathError::Ok;
}

MathematicalObject::MathError Vector3D::Scale( const double scalar ){
	// scale each component
	PrimitiveVector3::Scale( scalar );

	// update vectors length
	return UpdateLength();
}

MathematicalObject::MathError Vector3D::Normalise( void ){
	MathematicalObject::MathError tempory_error = MathError::Ok;
	MathematicalObject::MathError err = MathError::Ok;

	if( ( tempory_error = PrimitiveVector3::Normalise() ) != MathError::Ok ) err = tempory_error;
	if( ( tempory_error = UpdateLength() ) != MathError::Ok ) err = tempory_error;

	// scale and return error code
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

double Vector3D::GetAngle( const Vector3D second_vector ) const{
	// check if one vector has length zero
	if( IsNearlyEqualDistance( length_, 0 ) || IsNearlyEqualDistance( second_vector.length_, 0 ) ) return 0.;

	const double scalar_product = ( ( *this ) * second_vector );
	const double lengt_product = this->length_ * second_vector.length_;

	if( IsNearlyEqualDistance( scalar_product, lengt_product ) ) return 0.;

	// return angle in radians
	return acos( scalar_product / lengt_product );
}

bool Vector3D::IsOrthogonal( const Vector3D second_vector ) const{
	// one vector has no length?
	if( IsNearlyEqualDistance( this->length_, 0 ) || IsNearlyEqualDistance( second_vector.length_, 0 ) ) return false;

	// check dot product
	return IsNearlyEqualDistance( ( *this ) * second_vector, 0 );
}

MathematicalObject::MathError Vector3D::RotateAroundXAxis( const double sine_phi, const double cosine_phi ){
	PrimitiveVector3::RotateAroundXAxis( sine_phi, cosine_phi );
	return UpdateLength();
}

MathematicalObject::MathError Vector3D::RotateAroundYAxis( const double sine_phi, const double cosine_phi ){
	PrimitiveVector3::RotateAroundYAxis( sine_phi, cosine_phi );
	return UpdateLength();
}

MathematicalObject::MathError Vector3D::RotateAroundZAxis( const double sine_phi, const double cosine_phi ){
	PrimitiveVector3::RotateAroundZAxis( sine_phi, cosine_phi );
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

MathematicalObject::MathError Vector3D::Rotate( const Vector3D axis, const double rotation_angle ){
	Vector3D nCpy{ axis.ConvertTo( *this ) };

	MathematicalObject::MathError tempory_error = MathError::Ok;
	MathematicalObject::MathError err = MathError::Ok;

	if( ( tempory_error = this->PrimitiveVector3::Rotate( nCpy, rotation_angle ) ) != MathError::Ok ) err = tempory_error;
	if( ( tempory_error = this->UpdateLength() ) != MathError::Ok ) err = tempory_error;

	return err;
}

Vector3D Vector3D::RotateConstant( const Vector3D n, const double phi ) const{
	Vector3D rotated_vector( *this );
	rotated_vector.Rotate( n, phi );
	return rotated_vector;
}

Vector3D Vector3D::ProjectOnXYPlane( const CoordinateSystem* const coordinate_system ) const{
	Vector3D projected_vector = this->ConvertTo( coordinate_system );
	projected_vector.AddToZ( -projected_vector.z );

	return projected_vector;
}


Vector3D Vector3D::NegateXComponent( void ) const{
	return Vector3D{ Tuple3D{ -this->x, this->y, this->z }, this->coordinate_system_ };
}


/*
	Point3D implementation
*/

string Point3D::ConvertToString( [[maybe_unused]] const unsigned int newline_tabulators ) const{
	string new_string;

	char tempory_character_array[ 256 ];
	snprintf( tempory_character_array, 256, "(%.6f,%.6f,%.6f)", x, y, z );

	new_string += tempory_character_array;
	return new_string;
}

Point3D Point3D::ConvertTo( const CoordinateSystem* const target_coordinate_system ) const{

	if( this->coordinate_system_ == target_coordinate_system ) return *this;

	const Coordinates converted_coordinates = this->Coordinates::ConvertTo( target_coordinate_system );

	return Point3D( converted_coordinates, target_coordinate_system );


};

Point3D Point3D::ConvertTo( const Point3D target_coordinate_system ) const{
	return Point3D::ConvertTo( target_coordinate_system.coordinate_system_ );
}

PrimitiveVector3 Point3D::GetComponents( const CoordinateSystem* const target_coordinate_system ) const{
	return Point3D::ConvertTo( target_coordinate_system ).GetComponents();
}

PrimitiveVector3 Point3D::GetComponents( const Point3D target_coordinate_system ) const{
	return Point3D::GetComponents( target_coordinate_system.coordinate_system_ );
}

PrimitiveVector3 Point3D::GetGlobalComponents( void ) const{
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
	Point3D projected_point = this->ConvertTo( coordinate_system );
	projected_point.AddToZ( -projected_point.z );

	return projected_point;
}


Vector3D::Vector3D( const Unitvector3D& unit_vector ) :
	Coordinates{ Tuple3D{ unit_vector.X(), unit_vector.Y(), unit_vector.Z() }, unit_vector.GetCoordinateSystem() }
{

}