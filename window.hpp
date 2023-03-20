/******************************************************************
* @file   window.cpp
* @brief  Implementations
*
* @author Jan Wolzenburg
* @date   March 2023
* ********************************************************************/



/*********************************************************************
  Includes
*********************************************************************/

#include "window.h"



/*********************************************************************
  Implementations
*********************************************************************/



template<class W>
W& window::add( const relPosition position, Fl_Group& parent, const char* label ){

	static_assert( std::is_base_of_v< Fl_Widget, W > );

	int x = parent.x() + (int) ( (double) parent.w() * position.X() );
	int y = parent.y() + (int) ( (double) parent.h() * position.Y() );

	int w = (int) ( (double) parent.w() * position.W() );
	int h = (int) ( (double) parent.h() * position.H() );

	W* newWidget = new W{ x, y, w, h, label };
	widgets.push_back( newWidget );

	parent.add( newWidget );

	return *newWidget;
}


template<class W>
W& window::add( const relPosition position, Fl_Group& parent ){
	return add<W>( position, parent, "" );
}


template<class W>
W& window::add( const relPosition position, const char* label ){
	return add<W>( position, *this, label );
}


template<class W>
W& window::add( const relPosition position ){
	return add<W>( position, *this );
}