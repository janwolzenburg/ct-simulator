/*********************************************************
* @file   cSysTree.cpp
* @brief  Implementations
*
* @author Jan Wolzenburg
* @date   December 2022
* ********************************************************************/



/*********************************************************************
	Includes
 *********************************************************************/
#include "cSysTree.h"
#include "cartesian.h"
#include "serialization.h"


 /*********************************************************************
	Implementations
 *********************************************************************/


cSysTree& CSYS_TREE( void ){
	return cSysTree::getInstance();
}

const cartCSys* GLOBAL_CSYS( void ){
	return CSYS_TREE().getGlobal();
}

cartCSys* DUMMY_CSYS( void ){
	return CSYS_TREE().getDummy();
}



/*
	cSysTree implementation
*/

cSysTree& cSysTree::getInstance(){
	static cSysTree instance;
	return instance;
}

string cSysTree::toStr( const unsigned int newLineTabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newLineTabulators; i++ ) newLine += '\t';

	for( size_t idx = 0; idx < numSystems; idx++ ){
		str += newLine + systems.at( idx ).toStr( 1 );
	}

	return str;
}

cSysTree::cSysTree( void ) :
	numSystems( 2 ),
	systems{	cartCSys{ primitiveVec3{ Tuple3D{ 0, 0, 0 } }, primitiveVec3{ Tuple3D{ 1, 0, 0 } }, primitiveVec3{ Tuple3D{ 0, 1, 0 } }, primitiveVec3{ Tuple3D{ 0, 0, 1 } }, nullptr, "Dummy system" },
				cartCSys{ primitiveVec3{ Tuple3D{ 0, 0, 0 } }, primitiveVec3{ Tuple3D{ 1, 0, 0 } }, primitiveVec3{ Tuple3D{ 0, 1, 0 } }, primitiveVec3{ Tuple3D{ 0, 0, 1 } }, nullptr, "Global system" } }
{
	systems.at( 0 ).parent = &systems.at( 1 );
}

cartCSys* cSysTree::addCSys( const primitiveVec3 origin_, const primitiveVec3 ex_, const primitiveVec3 ey_, const primitiveVec3 ez_, const cartCSys* parent_, const string name_ ){
	// Is the given parent valid in this tree
	if( !validTreeElement( parent_ ) ){
		checkErr( MATH_ERR::INPUT, "Parent is not part of tree!" );
		parent_ = &( systems.at( 1 ) );		// Set parent to global
	}

	// Add new system to tree
	cartCSys newSys{ origin_, ex_, ey_, ez_, parent_, name_ };
	systems.at( numSystems++ ) = newSys;

	return &systems.at( numSystems - 1 );
};

cartCSys* cSysTree::addCSys( const primitiveVec3 origin_, const primitiveVec3 ex_, const primitiveVec3 ey_, const primitiveVec3 ez_, const string name_ ){
	return addCSys( origin_, ex_, ey_, ez_, GLOBAL_CSYS(), name_ );
}

cartCSys* cSysTree::addCSys( const cartCSys* parent_, const string name_ ){
	return addCSys( primitiveVec3{ Tuple3D{ 0, 0, 0 } }, primitiveVec3{ Tuple3D{ 1, 0, 0 } }, primitiveVec3{ Tuple3D{ 0, 1, 0 } }, primitiveVec3{ Tuple3D{ 0, 0, 1 } }, parent_, name_ );
}

cartCSys* cSysTree::addCSys( const string name_ ){
	return addCSys( getGlobal(), name_ );
}

cartCSys* cSysTree::addCSys( const vector<char>& binary_data, vector<char>::const_iterator& it )
{

	primitiveVec3 origin{ binary_data, it };
	primitiveVec3 ex{ binary_data, it };
	primitiveVec3 ey{ binary_data, it };
	primitiveVec3 ez{ binary_data, it };

	string name = DeSerializeBuildIn<string>( string{""}, binary_data, it);

	return addCSys( origin, ex, ey, ez, GLOBAL_CSYS(), name );

}

bool cSysTree::validTreeElement( const cartCSys* const element ) const{
	for( size_t idx = 0; idx < numSystems; idx++ ){
		if( &( systems.at( idx ) ) == element ) return true;
	}

	return false;
}
