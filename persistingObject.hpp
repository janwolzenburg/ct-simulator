/******************************************************************
* @file   storedObjects.hpp
* @brief  Template implementations
*
* @author Jan Wolzenburg
* @date   April 2023
* ********************************************************************/

#include "persistingObject.h"
#include "serialization.h"

/*
template< class C >
PersistingObject<C>::PersistingObject( const path file_path, C& objectRef ) :
	file_path_( file_path ),
	object_reference_( objectRef ),
	was_loaded_( false ){
	LoadFromFile();
}*/

template< class C >
bool PersistingObject<C>::LoadAndSetPath( const path file_path ){
	
	file_path_ = file_path;
	return LoadFromFile();

}


template< class C >
bool PersistingObject<C>::Load( const path file_path ){


	// Does the file exist?
	if( !std::filesystem::exists( file_path ) ) return false;

	// Load file
	vector<char> binaryData = std::move( ImportSerialized( file_path ) );
	vector<char>::iterator binaryDataIt = binaryData.begin();

	if( !ValidBinaryData( C::FILE_PREAMBLE, binaryData, binaryDataIt ) ) return false;
	
	C::operator=( std::move( C{binaryData, binaryDataIt} ) );
	
	was_loaded_ = true;
	return was_loaded_;
}

template< class C >
bool PersistingObject<C>::Save( const bool force ) const{

	if( !was_loaded_ && !force ) return false;

	vector<char> binaryData;
	C::Serialize( binaryData );

	return ExportSerialized( file_path_, binaryData );

}

template< class C >
bool PersistingObject<C>::LoadFromFile( void ){

	return Load( file_path_ );

}