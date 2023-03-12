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

#include "fileChooser.h"
#include "FL/Fl_Native_File_Chooser.H"



/*********************************************************************
  Implementations
*********************************************************************/



fileChooser::fileChooser( const string windowTitle, const string fileFilter, const path defaultDirectory )
{

	this->type( Fl_Native_File_Chooser::BROWSE_FILE );
	
	this->title( windowTitle.c_str() );
	this->filter( fileFilter.c_str() );

	this->directory( defaultDirectory.string().c_str() );

}

void fileChooser::setStartDirectory( const path directory ){
	this->directory( directory.string().c_str() );
}

path fileChooser::choose( void ){

	if( this->show() != 0 ) return path{};

	path filePath = path{ this->filename() };
	setStartDirectory( filePath.parent_path() );
	
	return filePath;

}