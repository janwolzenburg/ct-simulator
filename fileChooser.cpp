/*********************************************************************
 * @file   fileChooser.cpp
 * @brief  Implementations
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


FileChooser::FileChooser( const string windowTitle, const string fileFilter, const path defaultDirectory, Fl_Native_File_Chooser::Type type ) :
	title_string_( windowTitle ),
	filter_string_( fileFilter ),
	start_directory_( defaultDirectory ),
	type_( type )
{

	this->Fl_Native_File_Chooser::type( type_ );
	
	this->title( windowTitle.c_str() );
	this->filter( fileFilter.c_str() );

	this->directory( defaultDirectory.string().c_str() );

	this->options( SAVEAS_CONFIRM );

}


FileChooser::FileChooser( const vector<char>& binary_data, vector<char>::const_iterator& it ) :
	FileChooser{ "", "", path{} }
{
	this->SetTitle( DeSerializeBuildIn<string>( string{ "File chooser" }, binary_data, it ) );
	this->SetFilter( DeSerializeBuildIn<string>( string{ "" }, binary_data, it ) );
	this->SetStartDirectory( DeSerializeBuildIn<string>( string{ "./" }, binary_data, it ) );

	this->Fl_Native_File_Chooser::type( DeSerializeBuildIn<unsigned char>( (unsigned char) (BROWSE_FILE), binary_data, it ) );

	this->options( SAVEAS_CONFIRM );

}

FileChooser::FileChooser( const FileChooser& fC ) : 
	FileChooser{ fC.title_string_, fC.filter_string_, fC.start_directory_, fC.type_ }
{}

FileChooser& FileChooser::operator=( const FileChooser& fC ){


	this->Fl_Native_File_Chooser::type( fC.type_ );
	this->SetTitle( fC.title_string_ );
	this->SetFilter( fC.filter_string_ );
	this->SetStartDirectory( fC.start_directory_ );
	

	return *this;
}


size_t FileChooser::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;
	num_bytes += SerializeBuildIn( FILE_PREAMBLE, binary_data );
	num_bytes += SerializeBuildIn( string{ this->title() }, binary_data);
	num_bytes += SerializeBuildIn( string{ this->filter() }, binary_data );
	num_bytes += SerializeBuildIn( string{ this->directory() }, binary_data );
	num_bytes += SerializeBuildIn<unsigned char>( (unsigned char) type_, binary_data );

	return num_bytes;

}

void FileChooser::SetTitle( const string newTitle ){
	title_string_ = newTitle;
	this->title( title_string_.c_str() );
}

void FileChooser::SetFilter( const string newFilter ){
	filter_string_ = newFilter;
	this->filter( filter_string_.c_str() );
}

void FileChooser::SetStartDirectory( const path newDirectory ){
	start_directory_ = newDirectory;
	this->directory( start_directory_.string().c_str() );
}

path FileChooser::ChooseFile( void ){

	if( this->show() != 0 ) return path{};

	path file_path = path{ this->filename() };
	SetStartDirectory( file_path.parent_path() );
	
	return file_path;
}