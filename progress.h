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

class Fl_Progress_Window : public Fl_Window{

	public:
	Fl_Progress_Window( const Fl_Window* const parent,  unsigned int textSize, unsigned int numLines, const char* label ) :
		Fl_Window( 20 * textSize, static_cast<int>( Fmin( numLines, (unsigned int) 1 ) * textSize ), label ),
		numLines( Fmin( numLines, (unsigned int ) 1 ) ),
		textDisplay( 0, 0, Fl_Window::w(), Fl_Window::h() ),
		lines( numLines ),
		displayText()
	{
	
		Fl_Window::add( textDisplay );
		Fl_Window::position( parent->x() + parent->w() / 2 - Fl_Window::w() / 2,
							 parent->y() + parent->h() / 2 - Fl_Window::h() / 2 );

	
		Fl_Window::show();

	}


	void changeLineText( const unsigned int lineNumber, const string newText){ 
		
		if( lineNumber >= numLines ) return;
		
		lines.at( lineNumber ) = newText;

		updateOutput();
	}

	unsigned int NumLines( void ) const { return numLines; }

	private:

	void updateOutput( void ){

		displayText.clear();

		for( auto currentLine_It = lines.cbegin(); currentLine_It < lines.cend(); currentLine_It++ ){

			displayText += *currentLine_It;

			if( lines.cend() - currentLine_It > 1 ) displayText += "\n";

		}

		textDisplay.value( displayText.c_str() );

		textDisplay.redraw();

	}


	private:

	unsigned int numLines;

	Fl_Multiline_Output textDisplay;
	vector<string> lines;
	string displayText;


 };
