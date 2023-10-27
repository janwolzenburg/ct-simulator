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

	static const string FILE_PREAMBLE;


	public:

	/*!
	 * @brief Constructor
	 * @param windowTitle Title of window 
	 * @param fileFilter String with filter
	 * @param defaultDirectory Directory to start
	 * @param type_ File chooser type
	*/
	fileChooser( const string windowTitle, const string fileFilter, const path defaultDirectory, const Fl_Native_File_Chooser::Type type_ = Fl_Native_File_Chooser::Type::BROWSE_FILE );

	/*!
	 * @brief Constructor from serialized data_
	 * @param binary_data Reference to vector with binary data_
	 * @param it Iterator to start of data_ in vector
	*/
	fileChooser( const vector<char>& binary_data, vector<char>::const_iterator& it );

	/*!
	 * @brief Copy constructor
	 * @param fC instance to copy from
	*/
	fileChooser( const fileChooser& fC );

	/*!
	 * @brief Assignment operator
	 * @param fC 
	 * @return 
	*/
	fileChooser& operator=( const fileChooser& fC );

	/*!
	 * @brief Serialize this object
	 * @param binary_data Reference to vector where data_ will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;

	/*!
	 * @brief Set new window title
	 * @param newTitle New title
	*/
	void setTitle( const string newTitle );

	/*!
	 * @brief Set new filter
	 * @param newFilter New filter
	*/
	void setFilter( const string newFilter );

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
	

	private:
	
	string titleString;		/*!<Title string*/
	string filterString;	/*!<Filter string*/
	path startDirectory;	/*!<Start directory*/
	Fl_Native_File_Chooser::Type chooserType;	/*!<Type of file chooser*/
 };