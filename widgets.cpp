/******************************************************************
* @file   widgets.cpp
* @brief  Implementations
*
* @author Jan Wolzenburg
* @date   March 2023
* ********************************************************************/



/*********************************************************************
  Includes
*********************************************************************/

#include "widgets.h"



/*********************************************************************
  Implementations
*********************************************************************/


void button_cb( Fl_Widget* widget, void* flag ){
	*( (bool*) flag ) = true;
}


selector::selector( int x, int y, int w, int h, const char* label ) :
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

	previous.callback( goPrev, (selector*) this );
	next.callback( goNext, (selector*) this );

	next.deactivate();
	previous.deactivate();

	currentElement = elements.cbegin();


};

void selector::goPrev( Fl_Widget* widget, void* p ){

	selector* selectorGrp = static_cast<selector*>( p );

	if( selectorGrp->currentElement > selectorGrp->elements.cbegin() ) selectorGrp->currentElement--;


	selectorGrp->checkActivation();
	selectorGrp->current.value( selectorGrp->currentElement->c_str() );
	selectorGrp->do_callback();

}


void selector::goNext( Fl_Widget* widget, void* p ){

	selector* selectorGrp = static_cast<selector*>( p );

	if( selectorGrp->currentElement < selectorGrp->elements.cend() - 1 ) selectorGrp->currentElement++;

	selectorGrp->checkActivation();
	selectorGrp->current.value( selectorGrp->currentElement->c_str() );
	selectorGrp->do_callback();

}


void selector::align( Fl_Align alignment ){
	current.align( alignment );
}

void selector::setElements( const vector<string> newElements ){
	elements = newElements;
	currentElement = elements.cbegin();

	checkActivation();
}


void selector::value( const string newValue ){

	for( vector<string>::const_iterator it = elements.cbegin(); it < elements.cend(); it++ ){

		if( newValue == *it ){
			currentElement = it;
			current.value( currentElement->c_str() );

			checkActivation();

			break;
		}
	}
};

void selector::checkActivation( void ){

	if( currentElement > elements.cbegin() ) previous.activate();
	else previous.deactivate();

	if( currentElement < elements.cend() - 1 ) next.activate();
	else next.deactivate();
}