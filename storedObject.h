#pragma once
/*********************************************************************
 * @file   .h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   March 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include <string>
using std::string;

#include <vector>
using std::vector;

#include "generel.h"


/*********************************************************************
   Definitions
*********************************************************************/

template< class C >
class storedObject{

	public:
	storedObject( const path filePath_, C& objectRef );

	bool load( const path filePath );

	void saveObject( const bool force = false ) const;

	inline bool Loaded( void ) const{ return loaded; };

	inline void setLoaded( void ){ loaded = true; };


	private:

	path file;
	C& object;
	bool loaded;

	void loadStored( void );

};


template< class C >
storedObject<C>::storedObject( const path filePath_, C& objectRef ) :
	file( filePath_ ),
	object( objectRef ),
	loaded( false ){
	loadStored();
};


template< class C >
bool storedObject<C>::load( const path filePath ){

	// Does the file exist?
	if( !std::filesystem::exists( filePath ) ) return false;

	// Load file
	vector<char> binaryData = std::move( importSerialized( filePath ) );
	vector<char>::iterator binaryDataIt = binaryData.begin();

	if( !validBinaryData( object.FILE_PREAMBLE, binaryData, binaryDataIt ) ) return false;

	if constexpr( std::is_same<C, model>::value )
		object = std::move( C{ binaryData, binaryDataIt } );
	else
		object = C{ binaryData, binaryDataIt };
	
	loaded = true;
	return loaded;
};


template< class C >
void storedObject<C>::saveObject( const bool force ) const{

	if( !loaded && !force ) return;

	vector<char> binaryData;
	object.serialize( binaryData );

	exportSerialized( file, binaryData );

};

template< class C >
void storedObject<C>::loadStored( void ){

	load( file );

};
