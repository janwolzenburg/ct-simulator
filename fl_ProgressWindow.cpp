/******************************************************************
* @file   fl_ProgressWindow.cpp
*
* @author Jan Wolzenburg
* @date   October 2023
* ********************************************************************/

#include "fl_ProgressWindow.h"



Fl_Progress_Window::Fl_Progress_Window( const Fl_Window* const parent,  unsigned int text_size, unsigned int number_of_lines, const char* label ) :
	Fl_Window( number_of_character_per_line_ * text_size + 2*padding_, static_cast<int>( ForceToMin( number_of_lines, (unsigned int) 1 ) * text_size + 25 ) + 2*padding_, label ),
	number_of_lines_( ForceToMin( number_of_lines, (unsigned int ) 1 ) ),
	text_output_( padding_, padding_, Fl_Window::w() - 2*padding_, Fl_Window::h()-2*padding_ ),
	line_texts_( number_of_lines ),
	continuous_text_()
{
	
	Fl_Window::add( text_output_ );
	Fl_Window::position( parent->x() + parent->w() / 2 - Fl_Window::w() / 2,
						parent->y() + parent->h() / 2 - Fl_Window::h() / 2 );

	Fl_Window::show();

	text_output_.textsize( text_size );

}

void Fl_Progress_Window::ChangeLineText( const unsigned int line_number, const string line_text){ 
	if( line_number >= number_of_lines_ ) return;
	line_texts_.at( line_number ) = line_text;
	UpdateOutput();
}


void Fl_Progress_Window::UpdateOutput( void ){

	continuous_text_.clear();

	for( auto current_line = line_texts_.cbegin(); current_line < line_texts_.cend(); current_line++ ){

		continuous_text_ += *current_line;

		if( line_texts_.cend() - current_line > 1 ) continuous_text_ += "\n";

	}

	text_output_.value( continuous_text_.c_str() );
	text_output_.redraw();
}