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
	vector3D implementation
*/

vector3D::vector3D( const Coordinates coordinates ) :
	Coordinates( coordinates )
{
	UpdateLength();
}

vector3D::vector3D( const Tuple3D components, const CoordinateSystem* const coordinate_system ) :
	vector3D{ Coordinates{ components, coordinate_system } }
{}

vector3D::vector3D( void ) : vector3D{ Coordinates{} }
{}

string vector3D::ConvertToString( [[maybe_unused]] const unsigned int newline_tabulators ) const{
	string new_string;
	char tempory_character_array[ 256 ];
	snprintf( tempory_character_array, 256, "(%.6f,%.6f,%.6f) l=%.6f", X(), Y(), Z(), length() );

	new_string += tempory_character_array;
	return new_string;
}

double vector3D::operator* ( const vector3D second_vector ) const{
	const vector3D converted_vector = second_vector.ConvertTo( this->coordinate_system_ );

	return x * converted_vector.x + y * converted_vector.y + z * converted_vector.z;
};

vector3D vector3D::operator^( const vector3D second_vector ) const{
	const vector3D converted_vector = second_vector.ConvertTo( this->coordinate_system_ );

	return vector3D{ Tuple3D{ y * converted_vector.z - z * converted_vector.y, z * converted_vector.x - x * converted_vector.z, x * converted_vector.y - y * converted_vector.x }, this->coordinate_system_ };
};

bool vector3D::HasSameSystem( const vector3D vector_to_compare ) const{
	return this->Coordinates::HasSameSystem( vector_to_compare );
}

vector3D vector3D::ConvertTo( const CoordinateSystem* const target_coordinate_system ) const{

	if( this->coordinate_system_ == target_coordinate_system ) return *this;	// same system return copy of this

	const Coordinates tip_coordinates = this->Coordinates::ConvertTo( target_coordinate_system );
	const Point3D tip_point{ tip_coordinates, target_coordinate_system };

	const vector3D converted_vector = tip_point - this->coordinate_system_->GetOriginInParentSystem();

	return converted_vector;
};

vector3D vector3D::ConvertTo( const vector3D target_coordinate_system ) const{
	return this->ConvertTo( target_coordinate_system.coordinate_system_ );
};

vector3D vector3D::ConvertTo( const Line target_coordinate_system ) const{
	return this->ConvertTo( target_coordinate_system.direction() );
};

vector3D vector3D::ConvertTo( const Surface target_coordinate_system ) const{
	return this->ConvertTo( target_coordinate_system.direction_1() );
};

Primitivevector3 vector3D::GetComponents( const CoordinateSystem* const target_coordinate_system ) const{
	return vector3D::ConvertTo( target_coordinate_system ).GetComponents();
}

Primitivevector3 vector3D::GetComponents( const vector3D target_coordinate_system ) const{
	return vector3D::ConvertTo( target_coordinate_system.coordinate_system_ ).GetComponents();;
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
	MathematicalObject::MathError tempory_error = MathError::Ok;
	MathematicalObject::MathError err = MathError::Ok;

	if( ( tempory_error = Primitivevector3::Normalise() ) != MathError::Ok ) err = tempory_error;
	if( ( tempory_error = UpdateLength() ) != MathError::Ok ) err = tempory_error;

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

double vector3D::GetAngle( const vector3D second_vector ) const{
	// check if one vector has length zero
	if( IsNearlyEqualDistance( length_, 0 ) || IsNearlyEqualDistance( second_vector.length_, 0 ) ) return 0.;

	const double scalar_product = ( ( *this ) * second_vector );
	const double lengt_product = this->length_ * second_vector.length_;

	if( IsNearlyEqualDistance( scalar_product, lengt_product ) ) return 0.;

	// return angle in radians
	return acos( scalar_product / lengt_product );
}

bool vector3D::IsOrthogonal( const vector3D second_vector ) const{
	// one vector has no length?
	if( IsNearlyEqualDistance( this->length_, 0 ) || IsNearlyEqualDistance( second_vector.length_, 0 ) ) return false;

	// check dot product
	return IsNearlyEqualDistance( ( *this ) * second_vector, 0 );
}

MathematicalObject::MathError vector3D::RotateAroundXAxis( const double sine_phi, const double cosine_phi ){
	Primitivevector3::RotateAroundXAxis( sine_phi, cosine_phi );
	return UpdateLength();
}

MathematicalObject::MathError vector3D::RotateAroundYAxis( const double sine_phi, const double cosine_phi ){
	Primitivevector3::RotateAroundYAxis( sine_phi, cosine_phi );
	return UpdateLength();
}

MathematicalObject::MathError vector3D::RotateAroundZAxis( const double sine_phi, const double cosine_phi ){
	Primitivevector3::RotateAroundZAxis( sine_phi, cosine_phi );
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

MathematicalObject::MathError vector3D::Rotate( const vector3D axis, const double rotation_angle ){
	vector3D nCpy{ axis.ConvertTo( *this ) };

	MathematicalObject::MathError tempory_error = MathError::Ok;
	MathematicalObject::MathError err = MathError::Ok;

	if( ( tempory_error = this->Primitivevector3::Rotate( nCpy, rotation_angle ) ) != MathError::Ok ) err = tempory_error;
	if( ( tempory_error = this->UpdateLength() ) != MathError::Ok ) err = tempory_error;

	return err;
}

vector3D vector3D::RotateConstant( const vector3D n, const double phi ) const{
	vector3D rotated_vector( *this );
	rotated_vector.Rotate( n, phi );
	return rotated_vector;
}

vector3D vector3D::ProjectOnXYPlane( const CoordinateSystem* const coordinate_system ) const{
	vector3D projected_vector = this->ConvertTo( coordinate_system );
	projected_vector.AddToZ( -projected_vector.z );

	return projected_vector;
}


vector3D vector3D::NegateXComponent( void ) const{
	return vector3D{ Tuple3D{ -this->x, this->y, this->z }, this->coordinate_system_ };
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

Primitivevector3 Point3D::GetComponents( const CoordinateSystem* const target_coordinate_system ) const{
	return Point3D::ConvertTo( target_coordinate_system ).GetComponents();
}

Primitivevector3 Point3D::GetComponents( const Point3D target_coordinate_system ) const{
	return Point3D::GetComponents( target_coordinate_system.coordinate_system_ );
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
	Point3D projected_point = this->ConvertTo( coordinate_system );
	projected_point.AddToZ( -projected_point.z );

	return projected_point;
}