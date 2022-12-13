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
#include <string>
using std::string;
 
 #include "cartesian.h"
 #include "vec3D.h"
 #include "line.h"
#include "surf.h"



/*********************************************************************
   Implementations
*********************************************************************/


/*
	vec3D implementation
*/

vec3D::vec3D( const coordinates coords ) :
	coordinates( coords ){
	updateLength();
}

vec3D::vec3D( const vec3 xyz_, const cartCSys* const cSys_ ) :
	vec3D( coordinates{ xyz_, cSys_ } ){}

vec3D::vec3D( void ) : vec3D( vec3{0 ,0 ,0}, DUMMY_CSYS ){}

std::string vec3D::toStr( [[maybe_unused]] const unsigned int newLineTabulators ) const{
	std::string str;
	char tempCharArr[ 256 ];
	snprintf( tempCharArr, 256, "(%.6f,%.6f,%.6f) l=%.6f", X(), Y(), Z(), Length() );

	str += tempCharArr;
	return str;
}

bool vec3D::operator== ( const vec3D v2 ) const{
	//vec3D v2Conv = v2.convertTo( this->cSys );
	return this->coordinates::operator==( v2 );
}

vec3D vec3D::operator+ ( const vec3D v2 ) const{
	return this->coordinates::operator+( v2 );
}

vec3D vec3D::operator- ( const vec3D v2 ) const{
	return this->coordinates::operator-( v2 );
}

vec3D vec3D::operator- ( void ) const{
	return this->coordinates::operator-();
};

vec3D vec3D::operator* ( const double scalar ) const{
	return this->coordinates::operator*( scalar );
}

vec3D vec3D::operator/ ( const double divisor ) const{
	return this->coordinates::operator/( divisor );
}

double vec3D::operator* ( const vec3D v2 ) const{
	vec3D convVec = v2.convertTo( this->cSys );

	return x * convVec.x + y * convVec.y + z * convVec.z;
};

vec3D vec3D::operator^( const vec3D v2 ) const{
	vec3D convVec = v2.convertTo( this->cSys );

	return vec3D( vec3{ y * convVec.z - z * convVec.y, z * convVec.x - x * convVec.z, x * convVec.y - y * convVec.x }, this->cSys );
};

bool vec3D::sameSystem( const vec3D v ) const{
	return this->coordinates::sameSystem( v );
}

vec3D vec3D::convertTo( const cartCSys* const target_cSys ) const{
	coordinates tipCoords = this->coordinates::convertTo( target_cSys );
	pnt3D tipPoint{ tipCoords, target_cSys };

	vec3D convVec = tipPoint - this->cSys->OPntPrnt();

	return convVec;
};

vec3D vec3D::convertTo( const vec3D v ) const{
	return this->convertTo( v.cSys );
};

vec3D vec3D::convertTo( const line l ) const{
	return this->convertTo( l.R() );
};

vec3D vec3D::convertTo( const surf s ) const{
	return this->convertTo( s.R1() );
};

primitiveVec3 vec3D::XYZ( void ) const{
	return coordinates::XYZ();
}

primitiveVec3 vec3D::XYZ( const cartCSys* const target ) const{
	return vec3D::convertTo( target ).XYZ();
}

primitiveVec3 vec3D::XYZ( const vec3D targetV ) const{
	return vec3D::convertTo( targetV.cSys ).XYZ();;
}

primitiveVec3 vec3D::gXYZ( void ) const{
	return this->XYZ( GLOBAL_CSYS );
};

double  vec3D::gX( void ) const{ return gXYZ().x; };

double  vec3D::gY( void ) const{ return gXYZ().y; };

double  vec3D::gZ( void ) const{ return gXYZ().z; };

mathObj::MATH_ERR vec3D::updateLength( void ){
	// Reset error flag
	errno = 0;

	// New length
	len = sqrt( pow( x, 2 ) + pow( y, 2 ) + pow( z, 2 ) );

	// Check error flag from sqrt()
	if( errno != 0 ) return checkErr( MATH_ERR::GENERAL, "Error calculating square root!" );
	return MATH_ERR::OK;
}

mathObj::MATH_ERR vec3D::scale( const double scalar ){
	// Scale each component
	primitiveVec3::scale( scalar );

	// Update vec3Dtors length
	return updateLength();
}

mathObj::MATH_ERR vec3D::normalize( void ){
	mathObj::MATH_ERR tErr = MATH_ERR::OK;
	mathObj::MATH_ERR err = MATH_ERR::OK;

	if( ( tErr = primitiveVec3::normalize() ) != MATH_ERR::OK ) err = tErr;
	if( ( tErr = updateLength() ) != MATH_ERR::OK ) err = tErr;

	// Scale and return error code
	return err;
}

mathObj::MATH_ERR vec3D::addXM( const double x_ ){
	primitiveVec3::addXM( x_ );
	return updateLength();
};

mathObj::MATH_ERR vec3D::addYM( const double y_ ){
	primitiveVec3::addYM( y_ );
	return updateLength();
};

mathObj::MATH_ERR vec3D::addZM( const double z_ ){
	primitiveVec3::addZM( z_ );
	return updateLength();
};

vec3D vec3D::addX( const double x_ ) const{
	vec3D v = *this;
	v.addXM( x_ );
	return v;
};

vec3D vec3D::addY( const double y_ ) const{
	vec3D v = *this;
	v.addYM( y_ );
	return v;
};

vec3D vec3D::addZ( const double z_ ) const{
	vec3D v = *this;
	v.addZM( z_ );
	return v;
};

double vec3D::angle( const vec3D v2 ) const{
	// Check if one vec3Dtor has length zero
	if( iseqErr( len, 0 ) || iseqErr( v2.len, 0 ) ) return 0;

	// Return angle in radians
	return acos( ( ( *this ) * v2 ) / ( this->len * v2.len ) );
}

bool vec3D::isOrtho( const vec3D v2 ) const{
	// One vec3Dtor has no length?
	if( iseqErr( this->len, 0 ) || iseqErr( v2.len, 0 ) ) return false;

	// Check dot product
	return iseqErr( ( *this ) * v2, 0 );
}

mathObj::MATH_ERR vec3D::rotXM( const double sinPhi, const double cosPhi ){
	primitiveVec3::rotXM( sinPhi, cosPhi );
	return updateLength();
}

mathObj::MATH_ERR vec3D::rotYM( const double sinPhi, const double cosPhi ){
	primitiveVec3::rotYM( sinPhi, cosPhi );
	return updateLength();
}

mathObj::MATH_ERR vec3D::rotZM( const double sinPhi, const double cosPhi ){
	primitiveVec3::rotZM( sinPhi, cosPhi );
	return updateLength();
}

mathObj::MATH_ERR vec3D::rotXM( const double phi ){
	return rotXM( sin( phi ), cos( phi ) );
}

mathObj::MATH_ERR vec3D::rotYM( const double phi ){
	return rotYM( sin( phi ), cos( phi ) );
}

mathObj::MATH_ERR vec3D::rotZM( const double phi ){
	return rotZM( sin( phi ), cos( phi ) );
}

vec3D vec3D::rotX( const double phi ) const{
	vec3D v( *this );
	v.rotXM( phi );
	return v;
}

vec3D vec3D::rotY( const double phi ) const{
	vec3D v( *this );
	v.rotYM( phi );
	return v;
}

vec3D vec3D::rotZ( const double phi ) const{
	vec3D v( *this );
	v.rotZM( phi );
	return v;
}

mathObj::MATH_ERR vec3D::rotNM( const vec3D n, const double phi ){
	vec3D nCpy{ n.convertTo( *this ) };

	mathObj::MATH_ERR tErr = MATH_ERR::OK;
	mathObj::MATH_ERR err = MATH_ERR::OK;

	if( ( tErr = this->primitiveVec3::rotNM( nCpy, phi ) ) != MATH_ERR::OK ) err = tErr;
	if( ( tErr = this->updateLength() ) != MATH_ERR::OK ) err = tErr;

	return err;
}

vec3D vec3D::rotN( const vec3D n, const double phi ) const{
	vec3D v( *this );
	v.rotNM( n, phi );
	return v;
}

vec3D vec3D::projectOnXYPlane( const cartCSys* const cSys_ ) const{
	vec3D projectedVec = this->convertTo( cSys_ );
	projectedVec.addZM( -projectedVec.z );

	return projectedVec;
}

/*
	uvec3D implementation
*/

uvec3D::uvec3D( vec3D v ) : vec3D( v ){
	// Normalize vec3Dtor
	normalize();
}

uvec3D::uvec3D( const vec3 xyz_, const cartCSys* const cSys_ ) :
	uvec3D( vec3D{ xyz_, cSys_ } ){}

mathObj::MATH_ERR uvec3D::scale( [[maybe_unused]] const double scalar ){
	// Do nothing
	return MATH_ERR::OK;
};

mathObj::MATH_ERR uvec3D::addXM( const double x_ ){
	mathObj::MATH_ERR tErr = MATH_ERR::OK;
	if( ( tErr = vec3D::addXM( x_ ) ) != MATH_ERR::OK )		return tErr;
	if( ( tErr = normalize() ) != MATH_ERR::OK )			return tErr;
	return MATH_ERR::OK;
};

mathObj::MATH_ERR uvec3D::addYM( const double y_ ){
	mathObj::MATH_ERR tErr = MATH_ERR::OK;
	if( ( tErr = vec3D::addYM( y_ ) ) != MATH_ERR::OK )		return tErr;
	if( ( tErr = normalize() ) != MATH_ERR::OK )			return tErr;
	return MATH_ERR::OK;
};

mathObj::MATH_ERR uvec3D::addZM( const double z_ ){
	mathObj::MATH_ERR tErr = MATH_ERR::OK;
	if( ( tErr = vec3D::addZM( z_ ) ) != MATH_ERR::OK )		return tErr;
	if( ( tErr = normalize() ) != MATH_ERR::OK )			return tErr;
	return MATH_ERR::OK;
};



/*
	pnt3D implementation
*/

pnt3D::pnt3D( vec3D v )
	: vec3D( v ){};

string pnt3D::toStr( [[maybe_unused]] const unsigned int newLineTabulators ) const{
	string str;

	char tempCharArr[ 256 ];
	snprintf( tempCharArr, 256, "(%.6f,%.6f,%.6f)", x, y, z );

	str += tempCharArr;
	return str;
}

pnt3D pnt3D::convertTo( const cartCSys* const target_cSys ) const{
	coordinates convCoords = this->coordinates::convertTo( target_cSys );

	return pnt3D( convCoords, target_cSys );
};

pnt3D pnt3D::convertTo( const pnt3D targetP ) const{
	return pnt3D::convertTo( targetP.cSys );
}

primitiveVec3 pnt3D::XYZ( void ) const{
	return vec3D::XYZ();
}

primitiveVec3 pnt3D::XYZ( const cartCSys* const target ) const{
	return pnt3D::convertTo( target ).XYZ();
}

primitiveVec3 pnt3D::XYZ( const pnt3D targetP ) const{
	return pnt3D::XYZ( targetP.cSys );
}

primitiveVec3 pnt3D::gXYZ( void ) const{
	return this->pnt3D::convertTo( GLOBAL_CSYS ).XYZ();
}

double pnt3D::gX( void ) const{
	return this->pnt3D::gXYZ().x;
}

double pnt3D::gY( void ) const{
	return this->pnt3D::gXYZ().y;
}

double pnt3D::gZ( void ) const{
	return this->pnt3D::gXYZ().z;
}

/*
bool pnt3D::isInside( const vox v ) const{
	// Create copy of this point in voxel's coordinate system
	pnt3D p{ this->convertTo( v.O() ) };
	// O
	pnt3D O = v.O();

	// Check all components
	return	O.x <= p.x && p.x <= O.x + v.Size().x &&
		O.y <= p.y && p.y <= O.y + v.Size().y &&
		O.z <= p.z && p.z <= O.z + v.Size().z;
};*/

pnt3D pnt3D::projectOnXYPlane( const cartCSys* const cSys_ ) const{
	pnt3D projectedPnt = this->convertTo( cSys_ );
	projectedPnt.addZM( -projectedPnt.z );

	return projectedPnt;
}