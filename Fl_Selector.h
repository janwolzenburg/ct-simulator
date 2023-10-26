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

/*!
 * @brief Class for a single single selector
*/
class Fl_Selector : public Fl_Group{

	public:

	/*!
	 * @brief Constructor
	 * @param x x-position
	 * @param y y-position
	 * @param w Width
	 * @param h Height
	 * @param label Label
	*/
	Fl_Selector( int x, int y, int w, int h, const char* label );

	/*!
	 * @brief Set alignment for title
	 * @param alignment Alignment
	*/
	void align( Fl_Align alignment ){ current.align( alignment ); };

	/*!
	 * @brief Set selectable elements
	 * @param newElements Elements
	*/
	void setElements( const vector<string> newElements );

	/*!
	 * @brief Set new value of selector
	 * @param newValue Selected string
	*/
	void value( const string newValue );

	/*!
	 * @brief Get selected item
	 * @return Item
	*/
	string value( void ) const{ return *currentElement;	};


	private:

	/*!
	 * @brief Goto previous selectable option
	 * @detail Is static because this function is registered as a callback
	 * @param widget Pointer to widget
	 * @param p Pointer to userdata. Here a pointer to the Fl_Selector must be passed
	*/
	static void goPrev( Fl_Widget* widget, void* p );

	/*!
	 * @brief Goto next selectable option
	 * @detail Is static because this function is registered as a callback
	 * @param widget Pointer to widget
	 * @param p Pointer to userdata. Here a pointer to the Fl_Selector must be passed
	*/
	static void goNext( Fl_Widget* widget,  void* p );

	/*!
	 * @brief Check for activation of buttons
	*/
	void checkActivation( void );


	Fl_Button previous;		/*!<Button to select previous element*/
	Fl_Output current;		/*!<Text outpur for current element*/
	Fl_Button next;			/*!<Button to select next element*/

	vector<string> elements;	/*!<All elements*/
	vector<string>::const_iterator currentElement;	/*!<Current element*/

};