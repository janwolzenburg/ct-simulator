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
#include "cartesian.h"
#include "cSysTree.h"
#include "vec3D.h"
#include "line.h"
#include "surf.h"


 /*********************************************************************
	Implementations
 *********************************************************************/


/*
	cartCSys implementation
*/

cartCSys::cartCSys( const primitiveVec3 origin_, const primitiveVec3 ex_, const primitiveVec3 ey_, const primitiveVec3 ez_,
					const cartCSys* const parent_, const string name_ )
	: primitiveCartCSys{ origin_, ex_, ey_, ez_ },
	parent( parent_ ),
	name( name_ ){};

cartCSys::cartCSys( void )
	: cartCSys( primitiveVec3{ v3{ 0, 0, 0 } }, primitiveVec3{ v3{ 1, 0, 0 } }, primitiveVec3{ v3{ 0, 1, 0 } }, primitiveVec3{ v3{ 0, 0, 1 } }, nullptr, "Uninitialised system" ){};

string cartCSys::toStr( const unsigned int newLineTabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newLineTabulators; i++ ) newLine += '\t';

	str += newLine + name;
	str += newLine + primitiveCartCSys::toStr();
	str += newLine + "Parent cSys:" + std::format( "{:#X}", (size_t) parent );
	return str;
}


cartCSys* cartCSys::createCopy( const string newName ) const{
	// Only one global coordinate system allowed! Parent of copy will be global system
	if (this->isGlobal()) {
		return CSYS_TREE().addCSys(origin, ex, ey, ez, this, newName);
	}

	return CSYS_TREE().addCSys( origin, ex, ey, ez, parent, newName );
}

void cartCSys::copyFrom( const cartCSys* const sourceCSys ){

	origin = sourceCSys->origin;
	ex = sourceCSys->ex;
	ey = sourceCSys->ey;
	ez = sourceCSys->ez;

	parent = sourceCSys->parent;

}

cartCSys* cartCSys::addCSys( const primitiveVec3 origin_, const primitiveVec3 ex_, const primitiveVec3 ey_, const primitiveVec3 ez_, const string name_ ) const{
	return CSYS_TREE().addCSys( origin_, ex_, ey_, ez_, this, name_ );
}

vector<const cartCSys*> cartCSys::getPathFromGlobal( void ) const{
	vector<const cartCSys*> path;

	if( this->isGlobal() ) return path;

	const cartCSys* cur_cSys = this->parent;	// Start with parent of this system

	// Loop while current coordinate system is not the global system
	while( !cur_cSys->isGlobal() ){
		path.push_back( cur_cSys );
		cur_cSys = cur_cSys->parent;
	}

	// Reverse order to get path of coordinate systems traverse
	std::reverse( path.begin(), path.end() );

	return path;
}


pnt3 cartCSys::OPnt( void ) const{
	return pnt3{ v3{0, 0, 0}, this };
}

pnt3 cartCSys::OPntPrnt( void ) const{
	if( this->isGlobal() ) return pnt3{ origin, this };
	return pnt3{ origin, parent };
}

uvec3 cartCSys::ExVec( void ) const{
	return uvec3{ v3{1, 0, 0},  this };
}

uvec3 cartCSys::EyVec( void ) const{
	return uvec3{ v3{0, 1, 0},   this };
}

uvec3 cartCSys::EzVec( void ) const{
	return uvec3{ v3{0, 0, 1},  this };
}

line cartCSys::xAxis( void ) const{
	const cartCSys* parent_ptr = parent;
	if( this->isGlobal() ) parent_ptr = GLOBAL_CSYS();

	return line{ vec3{ex, parent_ptr},  pnt3{origin, parent_ptr} };
}

line cartCSys::yAxis( void ) const{
	const cartCSys* parent_ptr = parent;
	if( this->isGlobal() ) parent_ptr = GLOBAL_CSYS();

	return line{ vec3{ey, parent_ptr},  pnt3{origin, parent_ptr} };
}

line cartCSys::zAxis( void ) const{
	const cartCSys* parent_ptr = parent;
	if( this->isGlobal() ) parent_ptr = GLOBAL_CSYS();

	return line{ vec3{ez, parent_ptr},  pnt3{origin, parent_ptr} };
}

surf cartCSys::xyPlane( void ) const{
	const cartCSys* parent_ptr = parent;
	if( this->isGlobal() ) parent_ptr = GLOBAL_CSYS();

	return surf{ vec3{ex, parent_ptr}, vec3{ey, parent_ptr}, pnt3{origin, parent_ptr } };
}

surf cartCSys::yzPlane( void ) const{
	const cartCSys* parent_ptr = parent;
	if( this->isGlobal() ) parent_ptr = GLOBAL_CSYS();

	return surf{ vec3{ey, parent_ptr}, vec3{ez, parent_ptr}, pnt3{origin, parent_ptr } };
}

surf cartCSys::xzPlane( void ) const{
	const cartCSys* parent_ptr = parent;
	if( this->isGlobal() ) parent_ptr = GLOBAL_CSYS();

	return surf{ vec3{ex, parent_ptr}, vec3{ez, parent_ptr}, pnt3{origin, parent_ptr } };
}

mathObj::MATH_ERR cartCSys::translateM( const vec3 dV ){
	if( this->isGlobal() ){
		return checkErr( MATH_ERR::OPERATION, "Global coordinate system cannot be translated!" );
	}

	primitiveCartCSys::translateM( dV.XYZ( parent ) );

	return MATH_ERR::OK;
}

mathObj::MATH_ERR cartCSys::rotateM( const uvec3 n, const double phi ){
	if( this->isGlobal() ){
		return checkErr( MATH_ERR::OPERATION, "Global coordinate system cannot be rotated!" );
	}

	return primitiveCartCSys::rotateM( n.XYZ( parent ), phi );
}

mathObj::MATH_ERR cartCSys::rotateM( const line l, const double phi ){
	MATH_ERR tErr = MATH_ERR::OK;
	MATH_ERR errCode = MATH_ERR::OK;

	// Rotate coordinate system's unit vectors
	if( ( tErr = rotateM( l.R(), phi ) ) != MATH_ERR::OK ) errCode = tErr;

	// Move rotation center to origin of rotation axis
	translateM( -l.O() );

	// Rotate position vector of origin around rotation axis
	if( ( tErr = origin.rotNM( l.R().XYZ( parent ), phi ) ) != MATH_ERR::OK ) errCode = tErr;

	// Translate back
	translateM( l.O() );

	return errCode;
}

void cartCSys::setPrimitive( const primitiveCartCSys primitiveCSys ){

	this->origin = primitiveCSys.O();
	this->ex = primitiveCSys.Ex();
	this->ey = primitiveCSys.Ey();
	this->ez = primitiveCSys.Ez();

}

size_t cartCSys::serialize( vector<char>& binData ) const{

	size_t numBytes = 0;

	numBytes += OPnt().gXYZ().serialize( binData );

	numBytes += ExVec().gXYZ().serialize( binData );
	numBytes += EyVec().gXYZ().serialize( binData );
	numBytes += EzVec().gXYZ().serialize( binData );
	
	numBytes += serializeBuildIn( name, binData );

	return numBytes;

}