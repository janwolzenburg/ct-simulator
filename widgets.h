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

 #include<FL/Fl_Widget.H>
 #include<type_traits>

 #include "generel.h"

 /*********************************************************************
	Definitions
 *********************************************************************/

void button_cb( Fl_Widget* widget, void* flag );

template<class C, std::enable_if_t<std::is_base_of_v< Fl_Widget, C >, bool> = false>
int X( const C& parent, const double start );

template<class C, std::enable_if_t<std::is_base_of_v< Fl_Widget, C >, bool> = false>
int Y( const C& parent, const double start );

template<class C, std::enable_if_t<std::is_base_of_v< Fl_Widget, C >, bool> = false>
int W( const C& parent, const double portion );

template<class C, std::enable_if_t<std::is_base_of_v< Fl_Widget, C >, bool> = false>
int H( const C& parent, const double portion );

template<class C, std::enable_if_t<std::is_base_of_v< Fl_Widget, C >, bool> = false>
int vOff( const C& parent );

template<class C, std::enable_if_t<std::is_base_of_v< Fl_Widget, C >, bool> = false>
int hOff( const C& parent );


/*!
 * @brief Storage of x and y pixel coordinates
*/
struct PixelCoordinates{

	PixelCoordinates( const int x, const int y ) : 
		x( x ), y( y ){};

	PixelCoordinates( const size_t u_x, const size_t u_y ) : 
		PixelCoordinates{ static_cast<int>( u_x ), static_cast<int>( u_y ) }{};

	PixelCoordinates( const Index2D index ) :
		PixelCoordinates{ index.x, index.y }{};

	int x;
	int y;
};

#include "widgets.hpp"