/******************************************************************
* @file   callbackFunction.hpp
* @brief  Template implementations
*
* @author Jan Wolzenburg
* @date   November 2023
* ********************************************************************/

#include "callbackFunction.h"

template<class C>
CallbackFunction<C>::CallbackFunction( C& class_instance, void (C::*member_function)( void )  ) :
	instance_( class_instance ), 
	member_function_( member_function )
{}


template<class C>
void CallbackFunction<C>::Execute( void ){
	if( member_function_ == nullptr ) return;
	(instance_.*member_function_)();
}


template<class C>
static void HandleCallback( Fl_Widget* widget, void* callback ){
	CallbackFunction<C>* cb = static_cast<CallbackFunction<C>*>( callback );
	cb->Execute();
}