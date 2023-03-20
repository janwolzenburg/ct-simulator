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


window::window( int w, int h ) :
	Fl_Window{ w, h }{
	Fl_Window::resizable( *this );
}


window::~window( void ){

	using WiPtr_It = vector<Fl_Widget*>::reverse_iterator;

	for( WiPtr_It it = widgets.rbegin(); it < widgets.rend(); it++ ){
		delete* it;
	}

	widgets.clear();

}
