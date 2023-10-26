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


const string fileChooser::FILE_PREAMBLE{ "Ver01_FILE_CHOOSER_FILE_PREAMBLE" };


fileChooser::fileChooser( const string windowTitle, const string fileFilter, const path defaultDirectory, Fl_Native_File_Chooser::Type type_ ) :
	titleString( windowTitle ),
	filterString( fileFilter ),
	startDirectory( defaultDirectory ),
	chooserType( type_ )
{

	this->type( chooserType );
	
	this->title( windowTitle.c_str() );
	this->filter( fileFilter.c_str() );

	this->directory( defaultDirectory.string().c_str() );

	this->options( SAVEAS_CONFIRM );

}


fileChooser::fileChooser( const vector<char>& binary_data, vector<char>::const_iterator& it ) :
	fileChooser{ "", "", path{} }
{
	this->setTitle( DeSerializeBuildIn<string>( string{ "File chooser" }, binary_data, it ) );
	this->setFilter( DeSerializeBuildIn<string>( string{ "" }, binary_data, it ) );
	this->setStartDirectory( DeSerializeBuildIn<string>( string{ "./" }, binary_data, it ) );

	this->type( DeSerializeBuildIn<unsigned char>( (unsigned char) (BROWSE_FILE), binary_data, it ) );

	this->options( SAVEAS_CONFIRM );

}

fileChooser::fileChooser( const fileChooser& fC ) : 
	fileChooser{ fC.titleString, fC.filterString, fC.startDirectory, fC.chooserType }
{}

fileChooser& fileChooser::operator=( const fileChooser& fC ){


	this->type( fC.chooserType );
	this->setTitle( fC.titleString );
	this->setFilter( fC.filterString );
	this->setStartDirectory( fC.startDirectory );
	

	return *this;
}


size_t fileChooser::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;
	num_bytes += SerializeBuildIn( FILE_PREAMBLE, binary_data );
	num_bytes += SerializeBuildIn( string{ this->title() }, binary_data);
	num_bytes += SerializeBuildIn( string{ this->filter() }, binary_data );
	num_bytes += SerializeBuildIn( string{ this->directory() }, binary_data );
	num_bytes += SerializeBuildIn<unsigned char>( (unsigned char) chooserType, binary_data );

	return num_bytes;

}

void fileChooser::setTitle( const string newTitle ){
	titleString = newTitle;
	this->title( titleString.c_str() );
}

void fileChooser::setFilter( const string newFilter ){
	filterString = newFilter;
	this->filter( filterString.c_str() );
}

void fileChooser::setStartDirectory( const path newDirectory ){
	startDirectory = newDirectory;
	this->directory( startDirectory.string().c_str() );
}

path fileChooser::choose( void ){

	if( this->show() != 0 ) return path{};

	path file_path = path{ this->filename() };
	setStartDirectory( file_path.parent_path() );
	
	return file_path;
}