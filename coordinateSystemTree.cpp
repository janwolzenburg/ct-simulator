/*********************************************************
* @file   CoordinateSystemTree.cpp
* @brief  Implementations
*
* @author Jan Wolzenburg
* @date   December 2022
* ********************************************************************/



/*********************************************************************
	Includes
 *********************************************************************/
#include "coordinateSystemTree.h"
#include "coordinateSystem.h"
#include "serialization.h"


 /*********************************************************************
	Implementations
 *********************************************************************/


CoordinateSystemTree& CoordinateSystems( void ){
	return CoordinateSystemTree::GetInstance();
}

const CoordinateSystem* GlobalSystem( void ){
	return CoordinateSystems().GetGlobal();
}

CoordinateSystem* DummySystem( void ){
	return CoordinateSystems().GetDummy();
}



/*
	CoordinateSystemTree implementation
*/

CoordinateSystemTree& CoordinateSystemTree::GetInstance(){
	static CoordinateSystemTree instance;
	return instance;
}

string CoordinateSystemTree::ToString( const unsigned int newline_tabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) newLine += '\t';

	for( size_t idx = 0; idx < system_count_; idx++ ){
		str += newLine + systems_.at( idx ).ToString( 1 );
	}

	return str;
}

CoordinateSystemTree::CoordinateSystemTree( void ) :
	system_count_( 2 ),
	systems_{	CoordinateSystem{ PrimitiveVector3{ Tuple3D{ 0, 0, 0 } }, PrimitiveVector3{ Tuple3D{ 1, 0, 0 } }, PrimitiveVector3{ Tuple3D{ 0, 1, 0 } }, PrimitiveVector3{ Tuple3D{ 0, 0, 1 } }, nullptr, "Dummy system" },
				CoordinateSystem{ PrimitiveVector3{ Tuple3D{ 0, 0, 0 } }, PrimitiveVector3{ Tuple3D{ 1, 0, 0 } }, PrimitiveVector3{ Tuple3D{ 0, 1, 0 } }, PrimitiveVector3{ Tuple3D{ 0, 0, 1 } }, nullptr, "Global system" } }
{
	systems_.at( 0 ).parent_ = &systems_.at( 1 );
}

CoordinateSystem* CoordinateSystemTree::AddSystem( const PrimitiveVector3 origin_, const PrimitiveVector3 ex_, const PrimitiveVector3 ey_, const PrimitiveVector3 ez_, const CoordinateSystem* parent_, const string name_ ){
	// Is the given parent_ valid in this tree
	if( !IsValidTreeElement( parent_ ) ){
		CheckForAndOutputError( MathError::Input, "parent is not part of tree!" );
		parent_ = &( systems_.at( 1 ) );		// Set parent_ to global
	}

	// Add new system to tree
	CoordinateSystem newSys{ origin_, ex_, ey_, ez_, parent_, name_ };
	systems_.at( system_count_++ ) = newSys;

	return &systems_.at( system_count_ - 1 );
};

CoordinateSystem* CoordinateSystemTree::AddSystem( const PrimitiveVector3 origin_, const PrimitiveVector3 ex_, const PrimitiveVector3 ey_, const PrimitiveVector3 ez_, const string name_ ){
	return AddSystem( origin_, ex_, ey_, ez_, GlobalSystem(), name_ );
}

CoordinateSystem* CoordinateSystemTree::AddSystem( const CoordinateSystem* parent_, const string name_ ){
	return AddSystem( PrimitiveVector3{ Tuple3D{ 0, 0, 0 } }, PrimitiveVector3{ Tuple3D{ 1, 0, 0 } }, PrimitiveVector3{ Tuple3D{ 0, 1, 0 } }, PrimitiveVector3{ Tuple3D{ 0, 0, 1 } }, parent_, name_ );
}

CoordinateSystem* CoordinateSystemTree::AddSystem( const string name_ ){
	return AddSystem( GetGlobal(), name_ );
}

CoordinateSystem* CoordinateSystemTree::AddSystem( const vector<char>& binary_data, vector<char>::const_iterator& it )
{

	PrimitiveVector3 origin{ binary_data, it };
	PrimitiveVector3 ex{ binary_data, it };
	PrimitiveVector3 ey{ binary_data, it };
	PrimitiveVector3 ez{ binary_data, it };

	string name = DeSerializeBuildIn<string>( string{""}, binary_data, it);

	return AddSystem( origin, ex, ey, ez, GlobalSystem(), name );

}

bool CoordinateSystemTree::IsValidTreeElement( const CoordinateSystem* const element ) const{
	for( size_t idx = 0; idx < system_count_; idx++ ){
		if( &( systems_.at( idx ) ) == element ) return true;
	}

	return false;
}
