#pragma once
/*********************************************************************
 * @file   window.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   March 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

 #include <vector>
 using std::vector;

#include <FL/Fl_Window.H>

#include "positions.h"



 /*********************************************************************
	Definitions
 *********************************************************************/

/*!
 * @brief Class for a window
*/
class window : private Fl_Window{

	public:

	/*!
	 * @brief Constructor
	 * @param w Width
	 * @param h Height
	*/
	window( int w, int h );

	/*!
	 * @brief Destructor
	*/
	~window( void );

	/*!
	 * @brief Get reference to Fl_Window base
	 * @param  
	 * @return 
	*/
	Fl_Window& getFl( void ){ return (Fl_Window&) *this; };

	/*!
	 * @brief Add widget
	 * @tparam W Widget type
	 * @param position Relative postion in widget
	 * @param parent_ Parent of widget
	 * @param label Label of widget
	 * @return Reference to added widget
	*/
	template<class W>
	W& add( const relPosition position, Fl_Group& parent, const char* label );

	/*!
	 * @brief Add widget
	 * @tparam W Widget type
	 * @param position Relative postion in widget
	 * @param parent_ Parent of widget
	 * @return Reference to added widget
	*/
	template<class W>
	W& add( const relPosition position, Fl_Group& parent );

	/*!
	 * @brief Add widget to window
	 * @tparam W Widget type
	 * @param position Relative postion in widget
	 * @param label Label of widget
	 * @return Reference to added widget
	*/
	template<class W>
	W& add( const relPosition position, const char* label );

	/*!
	 * @brief Add widget to window
	 * @tparam W Widget type
	 * @param position Relative postion in widget
	 * @return Reference to added widget
	*/
	template<class W>
	W& add( const relPosition position );


	private:

	vector<Fl_Widget*> widgets;		/*!<Vector with pointers to widgets in window*/

};

#include "window.hpp"