/******************************************************************
* @file   storedObjects.hpp
* @brief  Template implementations
*
* @author Jan Wolzenburg
* @date   April 2023
* ********************************************************************/

#include "storedObject.h"

template< class C >
storedObject<C>::storedObject( const path filePath_, C& objectRef ) :
	file( filePath_ ),
	object( objectRef ),
	loaded( false ){
	loadStored();
}

template< class C >
bool storedObject<C>::load( const path filePath ){

	// Does the file exist?
	if( !std::filesystem::exists( filePath ) ) return false;

	// Load file
	vector<char> binaryData = std::move( importSerialized( filePath ) );
	vector<char>::iterator binaryDataIt = binaryData.begin();

	if( !validBinaryData( object.FILE_PREAMBLE, binaryData, binaryDataIt ) ) return false;

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
	object.serialize( binaryData );

	exportSerialized( file, binaryData );

}

template< class C >
void storedObject<C>::loadStored( void ){

	load( file );

}