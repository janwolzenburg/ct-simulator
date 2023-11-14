#pragma once
/*********************************************************************
* @file   Fl_Selector.cpp
* @brief  Implementations
*
* @author Jan Wolzenburg
* @date   October 2023
* ********************************************************************/


/*********************************************************************
	Includes
 *********************************************************************/

#include "fl_Selector.h"
#include "widgets.h"


 /*********************************************************************
	Implementations
 *********************************************************************/

Fl_Selector::Fl_Selector( int x, int y, int w, int h, const char* label ) :
	Fl_Group{ x, y, w, h },
	previous_button_{ X( *this, 0. ),		Y( *this, 0. ),		H( *this, 1. ),		H( *this, 1. ) },
	current_text_{ hOff( previous_button_ ),	Y( *this, 0. ),		w - 2 * h,			H( *this, 1. ) },
	next_button_{ hOff( current_text_ ),	Y( *this, 0. ),		H( *this, 1. ),		H( *this, 1. ) }

{
	Fl_Group::add( previous_button_ );
	Fl_Group::add( current_text_ );
	Fl_Group::add( next_button_ );

	current_text_.label( label );
	current_text_.align( FL_ALIGN_TOP );

	previous_button_.label( "@<-" );
	next_button_.label( "@->" );

	previous_button_.callback( SelectPrevious, (Fl_Selector*) this );
	next_button_.callback( SelectNext, (Fl_Selector*) this );

	next_button_.deactivate();
	previous_button_.deactivate();

	current_element_ = elements_.cbegin();


}

void Fl_Selector::SelectPrevious( [[maybe_unused]] Fl_Widget* widget, void* p ){

	Fl_Selector* selectorGrp = static_cast<Fl_Selector*>( p );

	if( selectorGrp->current_element_ > selectorGrp->elements_.cbegin() ) selectorGrp->current_element_--;


	selectorGrp->CheckButtonActivation();
	selectorGrp->current_text_.value( selectorGrp->current_element_->c_str() );
	selectorGrp->do_callback();

}

void Fl_Selector::SelectNext( [[maybe_unused]] Fl_Widget* widget, void* p ){

	Fl_Selector* selectorGrp = static_cast<Fl_Selector*>( p );

	if( selectorGrp->current_element_ < selectorGrp->elements_.cend() - 1 ) selectorGrp->current_element_++;

	selectorGrp->CheckButtonActivation();
	selectorGrp->current_text_.value( selectorGrp->current_element_->c_str() );
	selectorGrp->do_callback();

}

void Fl_Selector::AssignElements( const vector<string> newElements ){
	elements_ = newElements;
	current_element_ = elements_.cbegin();

	CheckButtonActivation();
	current_text_.value( current_element_->c_str() );

}

void Fl_Selector::SetCurrentElement( const string newValue ){

	for( vector<string>::const_iterator it = elements_.cbegin(); it < elements_.cend(); it++ ){

		if( newValue == *it ){
			current_element_ = it;
			current_text_.value( current_element_->c_str() );

			CheckButtonActivation();

			break;
		}
	}
}

void Fl_Selector::CheckButtonActivation( void ){

	if( current_element_ > elements_.cbegin() ) previous_button_.activate();
	else previous_button_.deactivate();

	if( current_element_ < elements_.cend() - 1 ) next_button_.activate();
	else next_button_.deactivate();
}