/******************************************************************
* @file   progress.cpp
* @brief  Implementations
*
* @author Jan Wolzenburg
* @date   October 2023
* ********************************************************************/

#include "fl_ProgressWindow.h"



Fl_Progress_Window::Fl_Progress_Window( const Fl_Window* const parent,  unsigned int textSize, unsigned int numLines, const char* label ) :
	Fl_Window( number_of_character_per_line_ * textSize + 2*padding_, static_cast<int>( ForceToMin( numLines, (unsigned int) 1 ) * textSize + 25 ) + 2*padding_, label ),
	number_of_lines_( ForceToMin( numLines, (unsigned int ) 1 ) ),
	text_output_( padding_, padding_, Fl_Window::w() - 2*padding_, Fl_Window::h()-2*padding_ ),
	line_texts_( numLines ),
	continuous_text_()
{
	
	Fl_Window::add( text_output_ );
	Fl_Window::position( parent->x() + parent->w() / 2 - Fl_Window::w() / 2,
						parent->y() + parent->h() / 2 - Fl_Window::h() / 2 );

	Fl_Window::show();

	text_output_.textsize( textSize );

}

void Fl_Progress_Window::ChangeLineText( const unsigned int lineNumber, const string newText){ 
	if( lineNumber >= number_of_lines_ ) return;
	line_texts_.at( lineNumber ) = newText;
	UpdateOutput();
}


void Fl_Progress_Window::UpdateOutput( void ){

	continuous_text_.clear();

	for( auto currentLine_It = line_texts_.cbegin(); currentLine_It < line_texts_.cend(); currentLine_It++ ){

		continuous_text_ += *currentLine_It;

		if( line_texts_.cend() - currentLine_It > 1 ) continuous_text_ += "\n";

	}

	text_output_.value( continuous_text_.c_str() );
	text_output_.redraw();
}