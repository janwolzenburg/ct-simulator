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
 * @brief class for a single single selector
*/
class Fl_Selector : public Fl_Group{

	public:

	/*!
	 * @brief constructor
	 * @param x x-position
	 * @param y y-position
	 * @param w Width
	 * @param h Height
	 * @param label Label
	*/
	Fl_Selector( int x, int y, int w, int h, const char* label = 0L);

	/*!
	 * @brief get selected item
	 * @return item
	*/
	string current_element( void ) const{ return *current_element_; };

	/*!
	 * @brief set selectable elements
	 * @param elements Elements
	*/
	void AssignElements( const vector<string> elements );

	/*!
	 * @brief set new value of selector
	 * @param element Selected string
	*/
	void SetCurrentElement( const string element );


	private:

	Fl_Button previous_button_;		/*!< button to select previous element*/
	Fl_Output current_text_;		/*!< text outpur for current element*/
	Fl_Button next_button_;			/*!< button to select next element*/

	vector<string> elements_;		/*!< all elements*/
	vector<string>::const_iterator current_element_;	/*!< current element*/


	/*!
	 * @brief goto previous selectable option
	 * @detail Is static because this function is registered as a callback
	 * @param button_widget Pointer to button widget
	 * @param fl_selector Pointer to userdata. Here a pointer to the Fl_Selector must be passed
	*/
	static void SelectPrevious( Fl_Widget* button_widget, void* fl_selector );

	/*!
	 * @brief goto next selectable option
	 * @detail Is static because this function is registered as a callback
	 * @param button_widget Pointer to button widget
	 * @param fl_selector Pointer to userdata. Here a pointer to the Fl_Selector must be passed
	*/
	static void SelectNext( Fl_Widget* button_widget,  void* fl_selector );

	/*!
	 * @brief check for activation of buttons
	*/
	void CheckButtonActivation( void );
};