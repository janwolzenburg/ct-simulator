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
 #include "FL/fl_draw.H"


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
int Fl_OwnValuator<C>::value( double newVal ){

	int valueReturn = Fl_Valuator::value( newVal );

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

	int significantDigits = (int) ceil(  - log10( Fl_Valuator::step() ) );
	if( significantDigits < 0 ) significantDigits = 0;

	std::stringstream stream;
	stream << std::fixed << std::setprecision( significantDigits ) << this->value();

	string labelString = stream.str();

	
	int oldW, oldH;
	this->measure_label( oldW, oldH );

	int newW, newH, i = 0;
	do{
		labelString = " " + labelString;
		fl_measure( labelString.c_str(), newW, newH, 1 );

	}while( newW < oldW && i++ < 30 );


	this->copy_label( labelString.c_str() );
	this->redraw_label();

}