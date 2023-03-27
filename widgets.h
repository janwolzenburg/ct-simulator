#pragma once
/*********************************************************************
 * @file   widgets.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   March 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include <FL/Fl_Box.H>

#include <FL/Fl_Group.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>

#include "Fl_GreyImage.h"
#include "Own_Valuator.h"




 /*********************************************************************
	Definitions
 *********************************************************************/

void button_cb( Fl_Widget* widget, void* flag );


template<class C>
int X( const C& parent, const double start ){
	static_assert( std::is_base_of_v< Fl_Widget, C > );
	return parent.x() + (int) ( start * (double) parent.w() );
}

template<class C>
int Y( const C& parent, const double start ){
	static_assert( std::is_base_of_v< Fl_Widget, C > );
	return parent.y() + (int) ( start * (double) parent.h() );
}

template<class C>
int W( const C& parent, const double portion ){
	static_assert( std::is_base_of_v< Fl_Widget, C > );
	return (int) ( portion * (double) parent.w() );
}

template<class C>
int H( const C& parent, const double portion ){
	static_assert( std::is_base_of_v< Fl_Widget, C > );
	return (int) ( portion * (double) parent.h() );
}

template<class C>
int hOff( const C& parent ){
	static_assert( std::is_base_of_v< Fl_Widget, C > );
	return parent.y() + parent.h();
}

template<class C>
int vOff( const C& parent ){
	static_assert( std::is_base_of_v< Fl_Widget, C > );
	return parent.x() + parent.w();
}


class selector : public Fl_Group{

	public:
	selector( int x, int y, int w, int h, const char* label ) : 
		Fl_Group{ x, y, w, h },
		previous{	X( *this, 0.),		Y( *this, 0. ),		H( *this, 1. ),		H( *this, 1. ) },
		current{	vOff( previous ),	Y( *this, 0. ),		w - 2 * h,			H( *this, 1. ) },
		next{		vOff( current ),	Y( *this, 0. ),		H( *this, 1. ),		H( *this, 1. ) }

	{
		Fl_Group::add( previous );
		Fl_Group::add( current );
		Fl_Group::add( next );

		current.label( label );

		previous.label("@<-");
		next.label( "@->" );

		previous.callback( goPrev, (selector*) this );
		next.callback( goNext, (selector*) this );

		next.deactivate();
		previous.deactivate();

		currentElement = elements.cbegin();


	};

	void align( Fl_Align alignment ){
		current.align( alignment );
	}

	void setElements( const vector<string> newElements ){
		elements = newElements;
		currentElement = elements.cbegin();
	
		checkActivation();
	}


	inline void value( const string newValue ){

		for( vector<string>::const_iterator it = elements.cbegin(); it < elements.cend(); it++ ){

			if( newValue == *it ){
				currentElement = it;
				current.value( currentElement->c_str() );

				checkActivation();

				break;
			}
		}
	};

	inline string value( void ) const{

		return *currentElement;	

	};


	private:

	static void goPrev( Fl_Widget* widget, void* p ){
		
		selector* selectorGrp = static_cast<selector*>( p );

		if( selectorGrp->currentElement > selectorGrp->elements.cbegin() ) selectorGrp->currentElement--;
		

		selectorGrp->checkActivation();
		selectorGrp->current.value( selectorGrp->currentElement->c_str() );
		selectorGrp->do_callback();

	}


	static void goNext( Fl_Widget* widget,  void* p ){

		selector* selectorGrp = static_cast<selector*>( p );

		if( selectorGrp->currentElement < selectorGrp->elements.cend() - 1 ) selectorGrp->currentElement++;

		selectorGrp->checkActivation();
		selectorGrp->current.value( selectorGrp->currentElement->c_str() );
		selectorGrp->do_callback();

	}

	void checkActivation( void ){

		if( currentElement > elements.cbegin() ) previous.activate();
		else previous.deactivate();

		if( currentElement < elements.cend() - 1 ) next.activate();
		else next.deactivate();
	}

	private:
	Fl_Button previous;
	Fl_Output current;
	Fl_Button next;

	//Fl_Callback* callback_;
	//void* user_data_;

	vector<string> elements;
	vector<string>::const_iterator currentElement;

};