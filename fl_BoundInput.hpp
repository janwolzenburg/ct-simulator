#pragma once
/*********************************************************************
* @file   Fl_BoundInput.hpp
* @brief  Implementations
*
* @author Jan Wolzenburg
* @date   October 2023
* ********************************************************************/


/*********************************************************************
	Includes
*********************************************************************/

#include "fl_BoundInput.h"
#include "generel.h"
#include "generelMath.h"

template< class C, typename T>
Fl_BoundInput<C, T>::Fl_BoundInput( int x, int y, int w, int h, const char* label ) :
	Fl_Group{ x, y, w, h },
	input_widget_{ x, y, w, h, label },
	precision_( 0 ),
	current_value_( static_cast<T>( 0 ) ),
	value_string_( ConvertToString( current_value_, precision_ ) ),
	max_allowed_( static_cast<T>( 100 ) ),
	min_allowed_( static_cast<T>( 0 ) ),
	constraint_( None )
{
	Fl_Group::add( input_widget_ );
	input_widget_.callback( HandleValueChange, static_cast<Fl_Widget*>( this ) );

	input_widget_.value( "0" );
}

template< class C, typename T>
 void Fl_BoundInput<C, T>::HandleValueChange( [[maybe_unused]] Fl_Widget* widget, void* p ){
	Fl_BoundInput* parentPtr = static_cast<Fl_BoundInput*>( p );

	parentPtr->CheckAndForceConstraints();
	parentPtr->Fl_Group::do_callback();
}

template< class C, typename T>
void Fl_BoundInput<C, T>::value( const T newValue ){
	value_string_ = ConvertToString( newValue, precision_ );
	input_widget_.value( value_string_.c_str() );
	CheckAndForceConstraints();
}

template< class C, typename T>
void Fl_BoundInput<C, T>::SetProperties( const T min, const T max, const int precision, const Input_Constraints constraint ){
	min_allowed_ = min;
	max_allowed_ = max;
	precision_ = precision;
	constraint_ = constraint;
	CheckAndForceConstraints();
}

template< class C, typename T>
void Fl_BoundInput<C, T>::CheckAndForceConstraints( void ){

	current_value_ = ConvertToNumber<T>( input_widget_.value() );
	if( current_value_ < min_allowed_ ) current_value_ = min_allowed_;
	if( current_value_ > max_allowed_ ) current_value_ = max_allowed_;

	switch( constraint_ ){

		case Odd:
			if( IsEven( current_value_ ) ) current_value_++;
			break;

		case Even:
			if( !IsEven( current_value_ ) ) current_value_++;
			break;

		case None:
		default:
			break;
	}


	value_string_ = ConvertToString( current_value_, precision_ );
	input_widget_.value( value_string_.c_str() );
}