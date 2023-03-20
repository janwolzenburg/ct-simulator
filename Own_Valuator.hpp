#pragma once
/*********************************************************************
 * @file   OwnValuator.hpp
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   March 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

 #include <iomanip>
 #include <sstream>

 #include "Own_Valuator.h"


/*********************************************************************
   Definitions
*********************************************************************/



template< class C >
void Own_Valuator<C>::cbFunction( Fl_Widget* widgetPtr, void* flag ){
	*( (bool*) flag ) = true;

	Own_Valuator* valuatorPtr = static_cast<Own_Valuator*>( widgetPtr );

	if( valuatorPtr->valueToLabel ){
		valuatorPtr->updateLabel();
	}

}


template< class C >
Own_Valuator<C>::Own_Valuator( int x, int y, int w, int h, const char* label ) :
	C{ x, y, w, h, label },
	changeFlag( false ),
	valueToLabel( false ){
	Fl_Valuator::callback( cbFunction, &changeFlag );
}


template< class C >
bool Own_Valuator<C>::ChangeFlag( void ){

	if( changeFlag ){
		changeFlag = false;
		return true;
	}

	return false;
}


template< class C >
double Own_Valuator<C>::value( void ){
	return Fl_Valuator::value();
}


template< class C >
int Own_Valuator<C>::value( double newVal ){

	bool valueReturn = Fl_Valuator::value( newVal );

	if( valueToLabel )
		updateLabel();

	return valueReturn;
}


template< class C >
void Own_Valuator<C>::ValueToLabel( const bool yesNo ){
	valueToLabel = yesNo;
	if( yesNo ) updateLabel();
}


template< class C >
void Own_Valuator<C>::updateLabel( void ){

	int significantDigits = (int) ( ceil( 1. / Fl_Valuator::step() ) - 1. );

	if( significantDigits < 0 ) significantDigits = 0;

	std::stringstream stream;

	stream << std::fixed << std::setprecision( significantDigits ) << this->value();

	labelString = stream.str();
	this->label( this->labelString.c_str() );
	this->redraw_label();

}