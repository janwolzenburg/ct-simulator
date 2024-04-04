#pragma once
/*********************************************************************
 * @file   widgets.h
 *
 * @author Jan Wolzenburg
 * @date   March 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

 #include<FL/Fl_Widget.H>
 #include<type_traits>

 #include "generel.h"


 /*********************************************************************
	Definitions
 *********************************************************************/

/*!
 * @brief calculate pixel offset in x-direction based on parent postion and relative position
 * @tparam C type of widget
 * @param parent the parent widget
 * @param start relative position
 * @return absolute pixel position
 */
template<class C, std::enable_if_t<std::is_base_of_v< Fl_Widget, C >, bool> = false>
int X( const C& parent, const double start );

/*!
 * @brief calculate pixel offset in y-direction based on parent postion and relative position
 * @tparam C type of widget
 * @param parent the parent widget
 * @param start relative position
 * @return absolute pixel position
 */
template<class C, std::enable_if_t<std::is_base_of_v< Fl_Widget, C >, bool> = false>
int Y( const C& parent, const double start );

/*!
 * @brief calculate width in based on parent width and relative portion
 * @tparam C type of widget
 * @param parent the parent widget
 * @param portion the desired portion
 * @return absolute width
 */
template<class C, std::enable_if_t<std::is_base_of_v< Fl_Widget, C >, bool> = false>
int W( const C& parent, const double portion );

/*!
 * @brief calculate height based on parent height and relative portion
 * @tparam C type of widget
 * @param parent the parent widget
 * @param portion the desired portion
 * @return absolute height
 */
template<class C, std::enable_if_t<std::is_base_of_v< Fl_Widget, C >, bool> = false>
int H( const C& parent, const double portion );

/*!
 * @brief get absolute vertical position based on a parent
 * @tparam C type of widget
 * @param parent the parent
 * @return vertical pixel position directly under the parent
 */
template<class C, std::enable_if_t<std::is_base_of_v< Fl_Widget, C >, bool> = false>
int vOff( const C& parent );

/*!
 * @brief get absolute horizontal position based on a parent
 * @tparam C type of widget
 * @param parent the parent
 * @return horizontal pixel position directly right to the parent
 */
template<class C, std::enable_if_t<std::is_base_of_v< Fl_Widget, C >, bool> = false>
int hOff( const C& parent );


/*!
 * @brief storage of x and y pixel coordinates
*/
struct PixelCoordinates{

	/*!
	 * @brief constructor
	 * @param x x-coordinate
	 * @param y y-coordinate
	 */
	PixelCoordinates( const int x, const int y ) : 
		x( x ), y( y ){};

	/*!
	 * @brief constructor
	 * @param x x-coordinate
	 * @param y y-coordinate
	 */
	PixelCoordinates( const size_t u_x, const size_t u_y ) : 
		PixelCoordinates{ static_cast<int>( u_x ), static_cast<int>( u_y ) }{};

	/*!
	 * @brief constructor
	 * @param indices indices
	 */
	PixelCoordinates( const Index2D indices ) :
		PixelCoordinates{ indices.x, indices.y }{};

	int x;	/*!< x position*/
	int y;	/*!< y position*/
};

#include "widgets.hpp"