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

#include "Fl_Selector.h"
#include "widgets.h"


 /*********************************************************************
	Implementations
 *********************************************************************/

Fl_Selector::Fl_Selector( int x, int y, int w, int h, const char* label ) :
	Fl_Group{ x, y, w, h },
	previous{ X( *this, 0. ),		Y( *this, 0. ),		H( *this, 1. ),		H( *this, 1. ) },
	current{ hOff( previous ),	Y( *this, 0. ),		w - 2 * h,			H( *this, 1. ) },
	next{ hOff( current ),	Y( *this, 0. ),		H( *this, 1. ),		H( *this, 1. ) }

{
	Fl_Group::add( previous );
	Fl_Group::add( current );
	Fl_Group::add( next );

	current.label( label );

	previous.label( "@<-" );
	next.label( "@->" );

	previous.callback( goPrev, (Fl_Selector*) this );
	next.callback( goNext, (Fl_Selector*) this );

	next.deactivate();
	previous.deactivate();

	currentElement = elements.cbegin();


}

void Fl_Selector::goPrev( [[maybe_unused]] Fl_Widget* widget, void* p ){

	Fl_Selector* selectorGrp = static_cast<Fl_Selector*>( p );

	if( selectorGrp->currentElement > selectorGrp->elements.cbegin() ) selectorGrp->currentElement--;


	selectorGrp->checkActivation();
	selectorGrp->current.value( selectorGrp->currentElement->c_str() );
	selectorGrp->do_callback();

}

void Fl_Selector::goNext( [[maybe_unused]] Fl_Widget* widget, void* p ){

	Fl_Selector* selectorGrp = static_cast<Fl_Selector*>( p );

	if( selectorGrp->currentElement < selectorGrp->elements.cend() - 1 ) selectorGrp->currentElement++;

	selectorGrp->checkActivation();
	selectorGrp->current.value( selectorGrp->currentElement->c_str() );
	selectorGrp->do_callback();

}


void Fl_Selector::align( Fl_Align alignment ){
	current.align( alignment );
}

void Fl_Selector::setElements( const vector<string> newElements ){
	elements = newElements;
	currentElement = elements.cbegin();

	checkActivation();
}


void Fl_Selector::value( const string newValue ){

	for( vector<string>::const_iterator it = elements.cbegin(); it < elements.cend(); it++ ){

		if( newValue == *it ){
			currentElement = it;
			current.value( currentElement->c_str() );

			checkActivation();

			break;
		}
	}
};

void Fl_Selector::checkActivation( void ){

	if( currentElement > elements.cbegin() ) previous.activate();
	else previous.deactivate();

	if( currentElement < elements.cend() - 1 ) next.activate();
	else next.deactivate();
}