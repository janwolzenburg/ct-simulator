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
PersistingObject<C>::PersistingObject( const C&& standard, const path file_path, const bool deactivate_saving ) :
	C( std::move( standard ) ),
	file_path_( file_path ),
	was_loaded_( false ),
	disable_saving( deactivate_saving )
{
	LoadFromFile();
};

template< class C >
PersistingObject<C>::PersistingObject( const C&& standard, const char* file_name, const bool deactivate_saving ) :
	PersistingObject<C>{ std::move( standard ), PROGRAM_STATE().GetAbsolutePath( string{ file_name } ), deactivate_saving }
{};

template< class C >
PersistingObject<C>::~PersistingObject( void ){
	if( was_loaded_ && !PROGRAM_STATE().reset_state_at_exit() && !disable_saving )
		SaveToFile();
};

template< class C >
PersistingObject<C>& PersistingObject<C>::operator=( const C& instance ){
	C::operator=( instance );
	was_loaded_ = true;
	return *this;
};


template< class C >
bool PersistingObject<C>::Load( const path file_path ){


	// does the file exist?
	if( !std::filesystem::exists( file_path ) ) return false;

	// load file
	vector<char> binaryData = std::move( ImportSerialized( file_path ) );
	vector<char>::iterator binaryDataIt = binaryData.begin();

	if( !ValidBinaryData( C::FILE_PREAMBLE, binaryData, binaryDataIt ) ) return false;
	
	C::operator=( std::move( C{binaryData, binaryDataIt} ) );
	
	was_loaded_ = true;
	return was_loaded_;
};

template< class C >
bool PersistingObject<C>::Save( const path file_path, const bool force ) const{

	if( !was_loaded_ && !force ) return false;

	vector<char> binaryData;
	SerializeBuildIn<string>( C::FILE_PREAMBLE, binaryData );
	C::Serialize( binaryData );

	return ExportSerialized( file_path, binaryData );

};

template< class C >
bool PersistingObject<C>::SaveToFile( const bool force ) const{

	return this->Save( file_path_, force );
};

