#pragma once
/*********************************************************************
 * @file   progress.h
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
	 * @param parent Parent used for window placement on screen
	 * @param textSize Size of text
	 * @param numLines Amount of lines
	 * @param label Window label
	*/
	Fl_Progress_Window( const Fl_Window* const parent,  unsigned int textSize, unsigned int numLines, const char* label );

	/*!
	 * @brief Change text in a line
	 * @param lineNumber Linenumber to change. Starting at 0
	 * @param newText New line text
	*/
	void changeLineText( const unsigned int lineNumber, const string newText);

	/*!
	 * @brief Get amount of lines
	 * @return Available lines
	*/
	unsigned int NumLines( void ) const { return numLines; }


	private:

	/*!
	 * @brief Update text output
	*/
	void updateOutput( void );

	unsigned int numLines;				/*!<Amount of lines*/

	Fl_Multiline_Output textDisplay;	/*!<Widget for text output*/
	vector<string> lines;				/*!<Line texts*/
	string displayText;					/*!<String to pass to widget*/

	constexpr static int padding = 20;				/*!<Padding of text to window borders*/
	constexpr static int charactersPerLine = 30;	/*!<Approximate amount of characters per line*/
 };
