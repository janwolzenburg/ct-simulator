/******************************************************************
* @file   storedObjects.hpp
* @brief  Template implementations
*
* @author Jan Wolzenburg
* @date   April 2023
* ********************************************************************/

#include "persistingObject.h"
#include "serialization.h"


template< class C >
PersistingObject<C>::PersistingObject( const path file_path, C& objectRef ) :
	file_path_( file_path ),
	object_reference_( objectRef ),
	was_loaded_( false ){
	LoadFromFile();
}

template< class C >
bool PersistingObject<C>::Load( const path file_path ){

	// Does the file exist?
	if( !std::filesystem::exists( file_path ) ) return false;

	// Load file
	vector<char> binaryData = std::move( ImportSerialized( file_path ) );
	vector<char>::iterator binaryDataIt = binaryData.begin();

	if( !ValidBinaryData( object_reference_.FILE_PREAMBLE, binaryData, binaryDataIt ) ) return false;
	
	object_reference_ = std::move( C{ binaryData, binaryDataIt } );
	
	was_loaded_ = true;
	return was_loaded_;
}

template< class C >
void PersistingObject<C>::Save( const bool force ) const{

	if( !was_loaded_ && !force ) return;

	vector<char> binaryData;
	object_reference_.Serialize( binaryData );

	ExportSerialized( file_path_, binaryData );

}

template< class C >
void PersistingObject<C>::LoadFromFile( void ){

	Load( file_path_ );

}