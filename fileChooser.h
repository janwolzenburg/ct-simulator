#pragma once
/*********************************************************************
 * @file   fileChooser.h
 * @brief  A custom file chooser based on FLTK native file chooser class
 *
 * @author Jan Wolzenburg
 * @date   March 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

 #include <string>
 using std::string;

 #include <filesystem>
 using std::filesystem::path;

#include "FL/Fl_Native_File_Chooser.H"



 /*********************************************************************
	Definitions
 *********************************************************************/

/*!
 * @brief Custom class for choosing a file
*/
class fileChooser : private Fl_Native_File_Chooser{

	public:

	/*!
	 * @brief Constructor
	 * @param windowTitle Title of window 
	 * @param fileFilter String with filter
	 * @param defaultDirectory Directory to start
	*/
	fileChooser( const string windowTitle, const string fileFilter, const path defaultDirectory = path{  } );

	/*!
	 * @brief Set the start directory
	 * @param directory Directory to start
	*/
	void setStartDirectory( const path directory );

	/*!
	 * @brief Choose a file
	 * @return Path to chosen file
	*/
	path choose( void );
		 
 };