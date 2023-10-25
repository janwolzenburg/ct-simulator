#pragma once
/*********************************************************************
 * @file   Fl_Selector.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   October 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include <string>
using std::string;
#include <vector>
using std::vector;
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>


 /*********************************************************************
	Definitions
 *********************************************************************/

class Fl_Selector : public Fl_Group{

	public:


	Fl_Selector( int x, int y, int w, int h, const char* label );

	void align( Fl_Align alignment );

	void setElements( const vector<string> newElements );

	void value( const string newValue );

	string value( void ) const{ return *currentElement;	};


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