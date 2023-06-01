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

 #include "Fl_OwnValuator.h"


/*********************************************************************
   Definitions
*********************************************************************/



template< class C >
void Fl_OwnValuator<C>::cbFunction( Fl_Widget* widgetPtr, void* flag ){
	*( (bool*) flag ) = true;

	Fl_OwnValuator* valuatorPtr = static_cast<Fl_OwnValuator*>( widgetPtr );

	if( valuatorPtr->valueToLabel ){
		valuatorPtr->updateLabel();
	}

}


template< class C >
Fl_OwnValuator<C>::Fl_OwnValuator( int x, int y, int w, int h, const char* label ) :
	C{ x, y, w, h, label },
	changeFlag( false ),
	valueToLabel( false ){
	Fl_Valuator::callback( cbFunction, &changeFlag );
}


template< class C >
bool Fl_OwnValuator<C>::ChangeFlag( void ){

	if( changeFlag ){
		changeFlag = false;
		return true;
	}

	return false;
}


template< class C >
double Fl_OwnValuator<C>::value( void ) const{
	return Fl_Valuator::value();
}


template< class C >
int Fl_OwnValuator<C>::value( double newVal ){

	bool valueReturn = Fl_Valuator::value( newVal );

	changeFlag = true;

	if( valueToLabel )
		updateLabel();

	return valueReturn;
}


template< class C >
void Fl_OwnValuator<C>::ValueToLabel( const bool yesNo ){
	valueToLabel = yesNo;
	if( yesNo ) updateLabel();
}


template< class C >
void Fl_OwnValuator<C>::updateLabel( void ){

	int significantDigits = (int) ( ceil( 1. / Fl_Valuator::step() ) - 1. );

	if( significantDigits < 0 ) significantDigits = 0;

	std::stringstream stream;

	stream << std::fixed << std::setprecision( significantDigits ) << this->value();

	labelString = stream.str();
	this->label( this->labelString.c_str() );
	this->redraw_label();

}