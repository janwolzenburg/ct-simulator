/******************************************************************
* @file   storedObjects.hpp
* @brief  Template implementations
*
* @author Jan Wolzenburg
* @date   April 2023
* ********************************************************************/

#include "persistingObject.h"
#include "programState.h"
#include "serialization.h"


template< class C >
PersistingObject<C>::PersistingObject( const C&& standard, const path file_path ) :
	C( std::move( standard ) ),
	file_path_( file_path ),
	was_loaded_( false )
{
	LoadFromFile();
};

template< class C >
PersistingObject<C>::PersistingObject( const C&& standard, const char* file_name ) :
	PersistingObject<C>{ std::move( standard ), PROGRAM_STATE().getPath( string{ file_name } ) }
{};

template< class C >
PersistingObject<C>::~PersistingObject( void ){
	if( was_loaded_ && !PROGRAM_STATE().ResetStateAtExit() )
		Save();
};

template< class C >
PersistingObject<C>& PersistingObject<C>::operator=( const C& instance ){
	C::operator=( instance );
	was_loaded_ = true;
	return *this;
};


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
};

template< class C >
bool PersistingObject<C>::Save( const bool force ) const{

	if( !was_loaded_ && !force ) return false;

	vector<char> binaryData;
	C::Serialize( binaryData );

	return ExportSerialized( file_path_, binaryData );

};