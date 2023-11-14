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
void Fl_OwnValuator<C>::HandleValueChange( Fl_Widget* widgetPtr, void* flag ){
	*( (bool*) flag ) = true;

	Fl_OwnValuator* valuatorPtr = static_cast<Fl_OwnValuator*>( widgetPtr );

	if( valuatorPtr->enable_label_ ){
		valuatorPtr->UpdateLabel();
	}
}

template< class C >
Fl_OwnValuator<C>::Fl_OwnValuator( int x, int y, int w, int h, const char* label ) :
	C{ x, y, w, h, label },
	value_changed_( false ),
	enable_label_( false ){
	Fl_Valuator::callback( HandleValueChange, &value_changed_ );
}

template< class C >
int Fl_OwnValuator<C>::value( double newVal ){

	int valueReturn = Fl_Valuator::value( newVal );

	value_changed_ = true;

	if( enable_label_ )
		UpdateLabel();

	return valueReturn;
}

template< class C >
void Fl_OwnValuator<C>::ValueToLabel( const bool yesNo ){
	enable_label_ = yesNo;
	if( yesNo ) UpdateLabel();
}

template< class C >
void Fl_OwnValuator<C>::UpdateLabel( void ){

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