/*********************************************************
* @file   coordinateSystemTree.cpp
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


CoordinateSystemTree& GetCoordinateSystemTree( void ){
	return CoordinateSystemTree::GetInstance();
}

const CoordinateSystem* GetGlobalSystem( void ){
	return GetCoordinateSystemTree().GetGlobal();
}

CoordinateSystem* GetDummySystem( void ){
	return GetCoordinateSystemTree().GetDummy();
}



/*
	CoordinateSystemTree implementation
*/

CoordinateSystemTree& CoordinateSystemTree::GetInstance(){
	static CoordinateSystemTree instance;
	return instance;
}

string CoordinateSystemTree::ConvertToString( const unsigned int newline_tabulators ) const{
	string new_string;
	string new_line = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) new_line += '\t';

	for( size_t system_index = 0; system_index < system_count_; system_index++ ){
		new_string += new_line + systems_.at( system_index ).ConvertToString( 1 );
	}

	return new_string;
}

CoordinateSystemTree::CoordinateSystemTree( void ) :
	system_count_( 2 ),
	systems_{	CoordinateSystem{ Primitivevector3{ Tuple3D{ 0, 0, 0 } }, Primitivevector3{ Tuple3D{ 1, 0, 0 } }, Primitivevector3{ Tuple3D{ 0, 1, 0 } }, Primitivevector3{ Tuple3D{ 0, 0, 1 } }, nullptr, "Dummy system" },
				CoordinateSystem{ Primitivevector3{ Tuple3D{ 0, 0, 0 } }, Primitivevector3{ Tuple3D{ 1, 0, 0 } }, Primitivevector3{ Tuple3D{ 0, 1, 0 } }, Primitivevector3{ Tuple3D{ 0, 0, 1 } }, nullptr, "Global system" } }
{
	systems_.at( 0 ).parent_ = &systems_.at( 1 );
}

CoordinateSystem* CoordinateSystemTree::AddSystem( const Primitivevector3 origin, const Primitivevector3 ex, const Primitivevector3 ey, const Primitivevector3 ez, const CoordinateSystem* parent, const string name ){
	
	// is the given parent valid in this tree
	if( !IsValidTreeElement( parent ) ){
		CheckForAndOutputError( MathError::Input, "parent is not part of tree!" );
		parent = &( systems_.at( 1 ) );		// set parent to global
	}

	// add new system to tree
	CoordinateSystem new_system{ origin, ex, ey, ez, parent, name };
	systems_.at( system_count_++ ) = new_system;

	return &systems_.at( system_count_ - 1 );
};

CoordinateSystem* CoordinateSystemTree::AddSystem( const Primitivevector3 origin, const Primitivevector3 ex, const Primitivevector3 ey, const Primitivevector3 ez, const string name ){
	return AddSystem( origin, ex, ey, ez, GetGlobalSystem(), name );
}

CoordinateSystem* CoordinateSystemTree::AddSystem( const CoordinateSystem* parent, const string name ){
	return AddSystem( Primitivevector3{ Tuple3D{ 0, 0, 0 } }, Primitivevector3{ Tuple3D{ 1, 0, 0 } }, Primitivevector3{ Tuple3D{ 0, 1, 0 } }, Primitivevector3{ Tuple3D{ 0, 0, 1 } }, parent, name );
}

CoordinateSystem* CoordinateSystemTree::AddSystem( const string name ){
	return AddSystem( GetGlobal(), name );
}

CoordinateSystem* CoordinateSystemTree::AddSystem( const vector<char>& binary_data, vector<char>::const_iterator& current_byte )
{

	Primitivevector3 origin{ binary_data, current_byte };
	Primitivevector3 ex{ binary_data, current_byte };
	Primitivevector3 ey{ binary_data, current_byte };
	Primitivevector3 ez{ binary_data, current_byte };

	string name = DeSerializeBuildIn<string>( string{""}, binary_data, current_byte);

	return AddSystem( origin, ex, ey, ez, GetGlobalSystem(), name );

}

bool CoordinateSystemTree::IsValidTreeElement( const CoordinateSystem* const element ) const{
	for( size_t system_index = 0; system_index < system_count_; system_index++ ){
		if( &( systems_.at( system_index ) ) == element ) return true;
	}

	return false;
}
