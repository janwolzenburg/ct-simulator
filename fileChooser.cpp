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


const string fileChooser::FILE_PREAMBLE{ "FILE_CHOOSER_FILE_PREAMBLE     " };


fileChooser::fileChooser( const string windowTitle, const string fileFilter, const path defaultDirectory ) : 
	titleString( windowTitle ),
	filterString( fileFilter ),
	startDirectory( defaultDirectory )
{

	this->type( Fl_Native_File_Chooser::BROWSE_FILE );
	
	this->title( windowTitle.c_str() );
	this->filter( fileFilter.c_str() );

	this->directory( defaultDirectory.string().c_str() );

}


fileChooser::fileChooser( const vector<char>& binData, vector<char>::const_iterator& it ) :
	fileChooser{ "", "" }
{

	this->type( Fl_Native_File_Chooser::BROWSE_FILE );

	this->setTitle( deSerializeBuildIn<string>( string{ "File chooser" }, binData, it ) );
	this->setFilter( deSerializeBuildIn<string>( string{ "" }, binData, it ) );
	this->setStartDirectory( deSerializeBuildIn<string>( string{ "./" }, binData, it ) );

}

fileChooser::fileChooser( const fileChooser& fC ) : 
	fileChooser{ fC.titleString, fC.filterString, fC.startDirectory }
{

}

fileChooser& fileChooser::operator=( const fileChooser& fC ){

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