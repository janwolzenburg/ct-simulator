/*********************************************************************
 * @file   fileChooser.cpp
 *
 * @author Jan Wolzenburg
 * @date   March 2023
 *********************************************************************/



 /*********************************************************************
   Includes
*********************************************************************/

#include <vector>
using std::vector;

#include "generel.h"
#include "serialization.h"
#include "fileChooser.h"
#include "FL/Fl_Native_File_Chooser.H"



/*********************************************************************
  Implementations
*********************************************************************/


const string FileChooser::FILE_PREAMBLE{ "Ver01_FILE_CHOOSER_FILE_PREAMBLE" };


FileChooser::FileChooser( const string window_title, const string file_filter, const path start_directory, Fl_Native_File_Chooser::Type type ) :
	title_string_( window_title ),
	filter_string_( file_filter ),
	start_directory_( start_directory ),
	type_( type )
{

	this->Fl_Native_File_Chooser::type( type_ );
	
	this->title( window_title.c_str() );
	this->filter( file_filter.c_str() );

	this->directory( start_directory.string().c_str() );

	this->options( SAVEAS_CONFIRM );

}


FileChooser::FileChooser( const vector<char>& binary_data, vector<char>::const_iterator& current_byte ) :
	FileChooser{ "", "", path{} }
{
	this->SetTitle( DeSerializeBuildIn<string>( string{ "File chooser" }, binary_data, current_byte ) );
	this->SetFilter( DeSerializeBuildIn<string>( string{ "" }, binary_data, current_byte ) );
	this->SetStartDirectory( DeSerializeBuildIn<string>( string{ "./" }, binary_data, current_byte ) );

	this->Fl_Native_File_Chooser::type( DeSerializeBuildIn<unsigned char>( (unsigned char) (BROWSE_FILE), binary_data, current_byte ) );

	this->options( SAVEAS_CONFIRM );

}

FileChooser::FileChooser( const FileChooser& file_chooser ) : 
	FileChooser{ file_chooser.title_string_, file_chooser.filter_string_, file_chooser.start_directory_, file_chooser.type_ }
{}

FileChooser& FileChooser::operator=( const FileChooser& file_chooser ){


	this->Fl_Native_File_Chooser::type( file_chooser.type_ );
	this->SetTitle( file_chooser.title_string_ );
	this->SetFilter( file_chooser.filter_string_ );
	this->SetStartDirectory( file_chooser.start_directory_ );
	

	return *this;
}


size_t FileChooser::Serialize( vector<char>& binary_data ) const{

	size_t number_of_bytes = 0;
	number_of_bytes += SerializeBuildIn<string>( string{ this->title() }, binary_data);
	number_of_bytes += SerializeBuildIn<string>( string{ this->filter() }, binary_data );
	number_of_bytes += SerializeBuildIn<string>( string{ this->directory() }, binary_data );
	number_of_bytes += SerializeBuildIn<unsigned char>( (unsigned char) type_, binary_data );

	return number_of_bytes;

}

void FileChooser::SetTitle( const string new_title ){
	title_string_ = new_title;
	this->title( title_string_.c_str() );
}

void FileChooser::SetFilter( const string new_filter ){
	filter_string_ = new_filter;
	this->filter( filter_string_.c_str() );
}

void FileChooser::SetStartDirectory( const path new_directory ){
	start_directory_ = new_directory;
	this->directory( start_directory_.string().c_str() );
}

path FileChooser::ChooseFile( void ){

	if( this->show() != 0 ) return path{};

	path file_path = path{ this->filename() };
	SetStartDirectory( file_path.parent_path() );
	
	return file_path;
}