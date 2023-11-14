#pragma once
/*********************************************************************
 * @file   fl_ProgressWindow.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   June 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/


 #include <string>
 using std::string;

#include <vector>
 using std::vector;

 #include <FL/Fl_Window.H>
 #include <FL/Fl_Multiline_Output.H>

 #include "generelMath.h"

/*!
 * @brief Class for a window with text output to show progress
*/
class Fl_Progress_Window : public Fl_Window{

	public:

	/*!
	 * @brief Constructor
	 * @param parent_ Parent used for window placement on screen
	 * @param textSize Size of text
	 * @param numLines Amount of lines
	 * @param label Window label
	*/
	Fl_Progress_Window( const Fl_Window* const parent,  unsigned int textSize, unsigned int numLines, const char* label );

	/*!
	 * @brief Change text in a Line
	 * @param lineNumber Linenumber to change. Starting at 0
	 * @param newText New Line text
	*/
	void ChangeLineText( const unsigned int lineNumber, const string newText);

	/*!
	 * @brief Get amount of lines
	 * @return Available lines
	*/
	unsigned int number_of_lines( void ) const { return number_of_lines_; }


	private:

	constexpr static int padding_ = 20;							/*!< Padding of text to window borders*/
	constexpr static int number_of_character_per_line_ = 30;	/*!< Approximate amount of characters per Line*/

	unsigned int number_of_lines_;				/*!< Amount of lines*/

	Fl_Multiline_Output text_output_;	/*!< Widget for text output*/
	vector<string> line_texts_;			/*!< Line texts*/
	string continuous_text_;			/*!< String to pass to widget*/

 
	/*!
	 * @brief Update text output
	*/
	void UpdateOutput( void );
 
 };
