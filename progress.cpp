/******************************************************************
* @file   progress.cpp
* @brief  Implementations
*
* @author Jan Wolzenburg
* @date   October 2023
* ********************************************************************/

#include "progress.h"



Fl_Progress_Window::Fl_Progress_Window( const Fl_Window* const parent,  unsigned int textSize, unsigned int numLines, const char* label ) :
	Fl_Window( charactersPerLine * textSize + 2*padding, static_cast<int>( Fmin( numLines, (unsigned int) 1 ) * textSize ) + 2*padding, label ),
	numLines( Fmin( numLines, (unsigned int ) 1 ) ),
	textDisplay( padding, padding, Fl_Window::w() - 2*padding, Fl_Window::h()-2*padding ),
	lines( numLines ),
	displayText()
{
	
	Fl_Window::add( textDisplay );
	Fl_Window::position( parent->x() + parent->w() / 2 - Fl_Window::w() / 2,
						parent->y() + parent->h() / 2 - Fl_Window::h() / 2 );

	Fl_Window::show();
}

void Fl_Progress_Window::changeLineText( const unsigned int lineNumber, const string newText){ 
	if( lineNumber >= numLines ) return;
	lines.at( lineNumber ) = newText;
	updateOutput();
}


void Fl_Progress_Window::updateOutput( void ){

	displayText.clear();

	for( auto currentLine_It = lines.cbegin(); currentLine_It < lines.cend(); currentLine_It++ ){

		displayText += *currentLine_It;

		if( lines.cend() - currentLine_It > 1 ) displayText += "\n";

	}

	textDisplay.value( displayText.c_str() );
	textDisplay.redraw();
}