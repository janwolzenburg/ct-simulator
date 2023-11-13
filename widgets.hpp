/******************************************************************
* @file   widgets.hpp
* @brief  Template implementations
*
* @author Jan Wolzenburg
* @date   April 2023
* ********************************************************************/

#include "widgets.h"


template<class C, std::enable_if_t<std::is_base_of_v< Fl_Widget, C >, bool>>
int X( const C& parent, const double start ){
	return parent.x() + static_cast<int>( start * static_cast<double>( parent.w() ) );
}

template<class C, std::enable_if_t<std::is_base_of_v< Fl_Widget, C >, bool>>
int Y( const C& parent, const double start ){
	return parent.y() + static_cast<int>( start * static_cast<double>( parent.h() ) );
}

template<class C, std::enable_if_t<std::is_base_of_v< Fl_Widget, C >, bool>>
int W( const C& parent, const double portion ){
	return static_cast<int>( portion * static_cast<double>( parent.w() ) );
}

template<class C, std::enable_if_t<std::is_base_of_v< Fl_Widget, C >, bool>>
int H( const C& parent, const double portion ){
	return static_cast<int>( portion * static_cast<double>( parent.h() ) );
}

template<class C, std::enable_if_t<std::is_base_of_v< Fl_Widget, C >, bool>>
int vOff( const C& parent ){
	return parent.y() + parent.h();
}

template<class C, std::enable_if_t<std::is_base_of_v< Fl_Widget, C >, bool>>
int hOff( const C& parent ){
	return parent.x() + parent.w();
}