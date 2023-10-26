/******************************************************************
* @file   storedObjects.hpp
* @brief  Template implementations
*
* @author Jan Wolzenburg
* @date   April 2023
* ********************************************************************/

#include "storedObject.h"
#include "serialization.h"


template< class C >
storedObject<C>::storedObject( const path file_path_, C& objectRef ) :
	file( file_path_ ),
	object( objectRef ),
	loaded( false ){
	loadStored();
}

template< class C >
bool storedObject<C>::load( const path file_path ){

	// Does the file exist?
	if( !std::filesystem::exists( file_path ) ) return false;

	// Load file
	vector<char> binaryData = std::move( ImportSerialized( file_path ) );
	vector<char>::iterator binaryDataIt = binaryData.begin();

	if( !ValidBinaryData( object.FILE_PREAMBLE, binaryData, binaryDataIt ) ) return false;

	if constexpr( std::is_same_v<C, model> )
		object = std::move( C{ binaryData, binaryDataIt } );
	else
		object = C{ binaryData, binaryDataIt };
	
	loaded = true;
	return loaded;
}

template< class C >
void storedObject<C>::saveObject( const bool force ) const{

	if( !loaded && !force ) return;

	vector<char> binaryData;
	object.Serialize( binaryData );

	ExportSerialized( file, binaryData );

}

template< class C >
void storedObject<C>::loadStored( void ){

	load( file );

}