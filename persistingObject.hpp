/******************************************************************
* @file   persistingObject.hpp
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
	vector<char> binary_data = ImportSerialized( file_path );
	vector<char>::const_iterator current_byte = binary_data.begin();

	if( !IsValidBinaryData( C::FILE_PREAMBLE, binary_data, std::ref( current_byte ) ) ) return false;
	
	C::operator=( std::move( C{ binary_data, std::ref( current_byte ) } ) );
	
	was_loaded_ = true;
	return was_loaded_;
};

template< class C >
bool PersistingObject<C>::Save( const path file_path, const bool force ) const{

	if( !was_loaded_ && !force ) return false;

	vector<char> binary_data;
	SerializeBuildIn<string>( C::FILE_PREAMBLE, binary_data );
	C::Serialize( binary_data );

	return ExportSerialized( file_path, binary_data );

};

template< class C >
bool PersistingObject<C>::SaveToFile( const bool force ) const{

	return this->Save( file_path_, force );
};

