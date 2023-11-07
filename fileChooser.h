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
class FileChooser : private Fl_Native_File_Chooser{

	public:

	static const string FILE_PREAMBLE; /*!<String to prepend to file when storing as file*/

	/*!
	 * @brief Constructor
	 * @param window_title Title of window 
	 * @param file_filter String with filter
	 * @param start_directory Directory to start
	 * @param type File chooser type
	*/
	FileChooser( const string window_title, const string file_filter, const path start_directory, const Fl_Native_File_Chooser::Type type = Fl_Native_File_Chooser::Type::BROWSE_FILE );

	/*!
	 * @brief Copy constructor
	*/
	FileChooser( const FileChooser& file_chooser );

	/*!
	 * @brief Copy assignment
	*/
	FileChooser& operator=( const FileChooser& file_chooser );

	/*!
	 * @brief Constructor from serialized data
	 * @param binary_data Reference to vector with binary data
	 * @param current_byte Iterator to start of data in vector
	*/
	FileChooser( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	/*!
	 * @brief Serialize this object
	 * @param binary_data Reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;

	/*!
	 * @brief Set new window title
	 * @param newTitle New title
	*/
	void SetTitle( const string new_title );

	/*!
	 * @brief Set new filter
	 * @param newFilter New filter
	*/
	void SetFilter( const string new_filter );

	/*!
	 * @brief Set the start directory
	 * @param directory Directory to start
	*/
	void SetStartDirectory( const path directory );

	/*!
	 * @brief Choose a file
	 * @return Path to chosen file
	*/
	path ChooseFile( void );
	

	private:
	
	string title_string_;				/*!<Title string*/
	string filter_string_;				/*!<Filter string*/
	path start_directory_;				/*!<Start directory*/
	Fl_Native_File_Chooser::Type type_;	/*!<Type of file chooser*/
 };