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


fileChooser::fileChooser( const vector<char>& binData, vector<char>::const_iterator& it ) :
	fileChooser{ "", "", path{} }
{
	this->setTitle( deSerializeBuildIn<string>( string{ "File chooser" }, binData, it ) );
	this->setFilter( deSerializeBuildIn<string>( string{ "" }, binData, it ) );
	this->setStartDirectory( deSerializeBuildIn<string>( string{ "./" }, binData, it ) );

	this->type( deSerializeBuildIn<unsigned char>( (unsigned char) (BROWSE_FILE), binData, it ) );

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


size_t fileChooser::serialize( vector<char>& binData ) const{

	size_t numBytes = 0;
	numBytes += serializeBuildIn( FILE_PREAMBLE, binData );
	numBytes += serializeBuildIn( string{ this->title() }, binData);
	numBytes += serializeBuildIn( string{ this->filter() }, binData );
	numBytes += serializeBuildIn( string{ this->directory() }, binData );
	numBytes += serializeBuildIn<unsigned char>( (unsigned char) chooserType, binData );

	return numBytes;

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

	path filePath = path{ this->filename() };
	setStartDirectory( filePath.parent_path() );
	
	return filePath;
}