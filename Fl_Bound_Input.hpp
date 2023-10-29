#pragma once
/*********************************************************************
* @file   Fl_Bound_Input.hpp
* @brief  Implementations
*
* @author Jan Wolzenburg
* @date   October 2023
* ********************************************************************/


/*********************************************************************
	Includes
*********************************************************************/

#include "Fl_Bound_Input.h"
#include "generel.h"
#include "generelMath.h"

template< class C, typename T>
Fl_Bound_Input<C, T>::Fl_Bound_Input( int x, int y, int w, int h, const char* label ) :
	Fl_Group{ x, y, w, h },
	input{ x, y, w, h, label },
	precision( 0 ),
	current( (T) 0 ),
	valueString( ToString( current, precision ) ),
	maxVal( (T) 100 ),
	minVal( (T) 0 ),
	constraint( NONE )
{
	Fl_Group::add( input );
	input.callback( cbFunction, (Fl_Widget*) this );
}

template< class C, typename T>
 void Fl_Bound_Input<C, T>::cbFunction( [[maybe_unused]] Fl_Widget* widget, void* p ){
	Fl_Bound_Input* parentPtr = static_cast<Fl_Bound_Input*>( p );

	parentPtr->checkBounds();
	parentPtr->Fl_Group::do_callback();
}

template< class C, typename T>
void Fl_Bound_Input<C, T>::value( const T newValue ){
	valueString = ToString( newValue, precision );
	input.value( valueString.c_str() );
	checkBounds();
}

template< class C, typename T>
void Fl_Bound_Input<C, T>::setProperties( const T min_, const T max_, const int precision_, const INPUT_CONSTRAINTS constraint_ ){
	minVal = min_;
	maxVal = max_;
	precision = precision_;
	constraint = constraint_;
	checkBounds();
}

template< class C, typename T>
void Fl_Bound_Input<C, T>::checkBounds( void ){

	current = ToNum<T>( input.value() );
	if( current < minVal ) current = minVal;
	if( current > maxVal ) current = maxVal;

	switch( constraint ){

		case ODD:
			if( IsEven( current ) ) current++;
			break;

		case EVEN:
			if( !IsEven( current ) ) current++;
			break;

		case NONE:
		default:
			break;
	}


	valueString = ToString( current, precision );
	input.value( valueString.c_str() );
}