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
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Tooltip.H>

#include "Fl_GreyImage.h"
#include "Own_Valuator.h"
#include "Fl_plots.h"




 /*********************************************************************
	Definitions
 *********************************************************************/

void button_cb( Fl_Widget* widget, void* flag );

template<class C>
int X( const C& parent, const double start );

template<class C>
int Y( const C& parent, const double start );

template<class C>
int W( const C& parent, const double portion );

template<class C>
int H( const C& parent, const double portion );

template<class C>
int vOff( const C& parent );

template<class C>
int hOff( const C& parent );


class selector : public Fl_Group{

	public:

	selector( int x, int y, int w, int h, const char* label );

	void align( Fl_Align alignment );

	void setElements( const vector<string> newElements );

	void value( const string newValue );

	inline string value( void ) const{ return *currentElement;	};


	private:

	static void goPrev( Fl_Widget* widget, void* p );

	static void goNext( Fl_Widget* widget,  void* p );

	void checkActivation( void );


	private:

	Fl_Button previous;
	Fl_Output current;
	Fl_Button next;


	vector<string> elements;
	vector<string>::const_iterator currentElement;

};


enum INPUT_CONSTRAINTS{
	NONE,
	EVEN,
	ODD
};

template< class C, typename T>
class Fl_Bound_Input : public Fl_Group{


	static_assert( std::is_base_of_v< Fl_Input, C > );
	static_assert( std::is_arithmetic_v< T >);

	public:


	Fl_Bound_Input( int x, int y, int w, int h, const char* label );

	void align( Fl_Align alignment );

	void value( const T newValue );

	T value( void ) const;

	void setProperties( const T min_, const T max_, const int precision_, const INPUT_CONSTRAINTS constraint_ = NONE  );

	void checkBounds( void );

	static void cbFunction( Fl_Widget* widget, void* p );
	

	public:

	C input;

	int precision;
	T current;
	string valueString;

	T maxVal;
	T minVal;
	INPUT_CONSTRAINTS constraint;

};


#include "widgets.hpp"