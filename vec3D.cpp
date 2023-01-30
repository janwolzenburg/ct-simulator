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
#include "cSysTree.h"
#include "coordinates.h"
#include "vec3D.h"
#include "line.h"
#include "surf.h"



/*********************************************************************
   Implementations
*********************************************************************/


/*
	vec3D implementation
*/

vec3::vec3( const coordinates coords ) :
	coordinates( coords ){
	updateLength();
}

vec3::vec3( const v3 xyz_, const cartCSys* const cSys_ ) :
	vec3( coordinates{ xyz_, cSys_ } )
{}

vec3::vec3( void ) : vec3( coordinates{} ) 
{}

string vec3::toStr( [[maybe_unused]] const unsigned int newLineTabulators ) const{
	string str;
	char tempCharArr[ 256 ];
	snprintf( tempCharArr, 256, "(%.6f,%.6f,%.6f) l=%.6f", X(), Y(), Z(), Length() );

	str += tempCharArr;
	return str;
}

bool vec3::operator== ( const vec3 v2 ) const{
	//vec3D v2Conv = v2.convertTo( this->cSys );
	return this->coordinates::operator==( v2 );
}

vec3 vec3::operator+ ( const vec3 v2 ) const{
	return this->coordinates::operator+( v2 );
}

vec3 vec3::operator- ( const vec3 v2 ) const{
	return this->coordinates::operator-( v2 );
}

vec3 vec3::operator- ( void ) const{
	return this->coordinates::operator-();
};

vec3 vec3::operator* ( const double scalar ) const{
	return this->coordinates::operator*( scalar );
}

vec3 vec3::operator/ ( const double divisor ) const{
	return this->coordinates::operator/( divisor );
}

double vec3::operator* ( const vec3 v2 ) const{
	vec3 convVec = v2.convertTo( this->cSys );

	return x * convVec.x + y * convVec.y + z * convVec.z;
};

vec3 vec3::operator^( const vec3 v2 ) const{
	vec3 convVec = v2.convertTo( this->cSys );

	return vec3( v3{ y * convVec.z - z * convVec.y, z * convVec.x - x * convVec.z, x * convVec.y - y * convVec.x }, this->cSys );
};

bool vec3::sameSystem( const vec3 v ) const{
	return this->coordinates::sameSystem( v );
}

vec3 vec3::convertTo( const cartCSys* const target_cSys ) const{
	coordinates tipCoords = this->coordinates::convertTo( target_cSys );
	pnt3 tipPoint{ tipCoords, target_cSys };

	vec3 convVec = tipPoint - this->cSys->OPntPrnt();

	return convVec;
};

vec3 vec3::convertTo( const vec3 v ) const{
	return this->convertTo( v.cSys );
};

vec3 vec3::convertTo( const line l ) const{
	return this->convertTo( l.R() );
};

vec3 vec3::convertTo( const surf s ) const{
	return this->convertTo( s.R1() );
};

primitiveVec3 vec3::XYZ( void ) const{
	return v3{ x, y, z};
} 

primitiveVec3 vec3::XYZ( const cartCSys* const target ) const{
	return vec3::convertTo( target ).XYZ();
}

primitiveVec3 vec3::XYZ( const vec3 targetV ) const{
	return vec3::convertTo( targetV.cSys ).XYZ();;
}

primitiveVec3 vec3::gXYZ( void ) const{
	return this->XYZ( GLOBAL_CSYS() );
};

double  vec3::gX( void ) const{ return gXYZ().x; };

double  vec3::gY( void ) const{ return gXYZ().y; };

double  vec3::gZ( void ) const{ return gXYZ().z; };

mathObj::MATH_ERR vec3::updateLength( void ){
	// Reset error flag
	errno = 0;

	// New length
	len = sqrt( pow( x, 2 ) + pow( y, 2 ) + pow( z, 2 ) );

	// Check error flag from sqrt()
	if( errno != 0 ) return checkErr( MATH_ERR::GENERAL, "Error calculating square root!" );
	return MATH_ERR::OK;
}

mathObj::MATH_ERR vec3::scale( const double scalar ){
	// Scale each component
	primitiveVec3::scale( scalar );

	// Update vec3Dtors length
	return updateLength();
}

mathObj::MATH_ERR vec3::normalize( void ){
	mathObj::MATH_ERR tErr = MATH_ERR::OK;
	mathObj::MATH_ERR err = MATH_ERR::OK;

	if( ( tErr = primitiveVec3::normalize() ) != MATH_ERR::OK ) err = tErr;
	if( ( tErr = updateLength() ) != MATH_ERR::OK ) err = tErr;

	// Scale and return error code
	return err;
}

mathObj::MATH_ERR vec3::addXM( const double x_ ){
	primitiveVec3::addXM( x_ );
	return updateLength();
};

mathObj::MATH_ERR vec3::addYM( const double y_ ){
	primitiveVec3::addYM( y_ );
	return updateLength();
};

mathObj::MATH_ERR vec3::addZM( const double z_ ){
	primitiveVec3::addZM( z_ );
	return updateLength();
};

vec3 vec3::addX( const double x_ ) const{
	vec3 v = *this;
	v.addXM( x_ );
	return v;
};

vec3 vec3::addY( const double y_ ) const{
	vec3 v = *this;
	v.addYM( y_ );
	return v;
};

vec3 vec3::addZ( const double z_ ) const{
	vec3 v = *this;
	v.addZM( z_ );
	return v;
};

double vec3::angle( const vec3 v2 ) const{
	// Check if one vec3Dtor has length zero
	if( iseqErr( len, 0 ) || iseqErr( v2.len, 0 ) ) return 0;

	// Return angle in radians
	return acos( ( ( *this ) * v2 ) / ( this->len * v2.len ) );
}

bool vec3::isOrtho( const vec3 v2 ) const{
	// One vec3Dtor has no length?
	if( iseqErr( this->len, 0 ) || iseqErr( v2.len, 0 ) ) return false;

	// Check dot product
	return iseqErr( ( *this ) * v2, 0 );
}

mathObj::MATH_ERR vec3::rotXM( const double sinPhi, const double cosPhi ){
	primitiveVec3::rotXM( sinPhi, cosPhi );
	return updateLength();
}

mathObj::MATH_ERR vec3::rotYM( const double sinPhi, const double cosPhi ){
	primitiveVec3::rotYM( sinPhi, cosPhi );
	return updateLength();
}

mathObj::MATH_ERR vec3::rotZM( const double sinPhi, const double cosPhi ){
	primitiveVec3::rotZM( sinPhi, cosPhi );
	return updateLength();
}

mathObj::MATH_ERR vec3::rotXM( const double phi ){
	return rotXM( sin( phi ), cos( phi ) );
}

mathObj::MATH_ERR vec3::rotYM( const double phi ){
	return rotYM( sin( phi ), cos( phi ) );
}

mathObj::MATH_ERR vec3::rotZM( const double phi ){
	return rotZM( sin( phi ), cos( phi ) );
}

vec3 vec3::rotX( const double phi ) const{
	vec3 v( *this );
	v.rotXM( phi );
	return v;
}

vec3 vec3::rotY( const double phi ) const{
	vec3 v( *this );
	v.rotYM( phi );
	return v;
}

vec3 vec3::rotZ( const double phi ) const{
	vec3 v( *this );
	v.rotZM( phi );
	return v;
}

mathObj::MATH_ERR vec3::rotNM( const vec3 n, const double phi ){
	vec3 nCpy{ n.convertTo( *this ) };

	mathObj::MATH_ERR tErr = MATH_ERR::OK;
	mathObj::MATH_ERR err = MATH_ERR::OK;

	if( ( tErr = this->primitiveVec3::rotNM( nCpy, phi ) ) != MATH_ERR::OK ) err = tErr;
	if( ( tErr = this->updateLength() ) != MATH_ERR::OK ) err = tErr;

	return err;
}

vec3 vec3::rotN( const vec3 n, const double phi ) const{
	vec3 v( *this );
	v.rotNM( n, phi );
	return v;
}

vec3 vec3::projectOnXYPlane( const cartCSys* const cSys_ ) const{
	vec3 projectedVec = this->convertTo( cSys_ );
	projectedVec.addZM( -projectedVec.z );

	return projectedVec;
}


vec3 vec3::negateX( void ) const{
	return vec3{ v3{ -this->x, this->y, this->z }, this->cSys };
}


/*
	uvec3D implementation
*/

uvec3::uvec3( vec3 v ) : vec3( v ){
	// Normalize vec3Dtor
	normalize();
}

uvec3::uvec3( const v3 xyz_, const cartCSys* const cSys_ ) :
	uvec3( vec3{ xyz_, cSys_ } ){}

mathObj::MATH_ERR uvec3::scale( [[maybe_unused]] const double scalar ){
	// Do nothing
	return MATH_ERR::OK;
};

mathObj::MATH_ERR uvec3::addXM( const double x_ ){
	mathObj::MATH_ERR tErr = MATH_ERR::OK;
	if( ( tErr = vec3::addXM( x_ ) ) != MATH_ERR::OK )		return tErr;
	if( ( tErr = normalize() ) != MATH_ERR::OK )			return tErr;
	return MATH_ERR::OK;
};

mathObj::MATH_ERR uvec3::addYM( const double y_ ){
	mathObj::MATH_ERR tErr = MATH_ERR::OK;
	if( ( tErr = vec3::addYM( y_ ) ) != MATH_ERR::OK )		return tErr;
	if( ( tErr = normalize() ) != MATH_ERR::OK )			return tErr;
	return MATH_ERR::OK;
};

mathObj::MATH_ERR uvec3::addZM( const double z_ ){
	mathObj::MATH_ERR tErr = MATH_ERR::OK;
	if( ( tErr = vec3::addZM( z_ ) ) != MATH_ERR::OK )		return tErr;
	if( ( tErr = normalize() ) != MATH_ERR::OK )			return tErr;
	return MATH_ERR::OK;
};



/*
	pnt3D implementation
*/

pnt3::pnt3( vec3 v )
	: vec3( v ){};

string pnt3::toStr( [[maybe_unused]] const unsigned int newLineTabulators ) const{
	string str;

	char tempCharArr[ 256 ];
	snprintf( tempCharArr, 256, "(%.6f,%.6f,%.6f)", x, y, z );

	str += tempCharArr;
	return str;
}

pnt3 pnt3::convertTo( const cartCSys* const target_cSys ) const{
	coordinates convCoords = this->coordinates::convertTo( target_cSys );

	return pnt3( convCoords, target_cSys );
};

pnt3 pnt3::convertTo( const pnt3 targetP ) const{
	return pnt3::convertTo( targetP.cSys );
}

primitiveVec3 pnt3::XYZ( void ) const{
	return vec3::XYZ();
}

primitiveVec3 pnt3::XYZ( const cartCSys* const target ) const{
	return pnt3::convertTo( target ).XYZ();
}

primitiveVec3 pnt3::XYZ( const pnt3 targetP ) const{
	return pnt3::XYZ( targetP.cSys );
}

primitiveVec3 pnt3::gXYZ( void ) const{
	return this->pnt3::convertTo( GLOBAL_CSYS() ).XYZ();
}

double pnt3::gX( void ) const{
	return this->pnt3::gXYZ().x;
}

double pnt3::gY( void ) const{
	return this->pnt3::gXYZ().y;
}

double pnt3::gZ( void ) const{
	return this->pnt3::gXYZ().z;
}

pnt3 pnt3::projectOnXYPlane( const cartCSys* const cSys_ ) const{
	pnt3 projectedPnt = this->convertTo( cSys_ );
	projectedPnt.addZM( -projectedPnt.z );

	return projectedPnt;
}