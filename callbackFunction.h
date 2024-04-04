#pragma once
/*********************************************************************
 * @file   callbackFunction.h
 *
 * @author Jan Wolzenburg
 * @date   November 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

 #include<FL/Fl_Widget.H>


 /*********************************************************************
	Definitions
 *********************************************************************/


 /*!
 * @brief class to store information about an object instance and a pointer to a member function
 * @tparam C class
*/
template<typename C>
class CallbackFunction{

	public:

	/*!
	 * @brief constructor
	 * @param class_instance reference to class instance 
	 * @param member_function pointer to member function
	*/
	CallbackFunction( C& class_instance, void ( C::*member_function )( void ) );

	/*!
	 * @brief execute member function
	*/
	void Execute( void );

	/*!
	 * @brief function to pass to FLTK callback() function
	 * @param widget pointer to widget
	 * @param callback pointer to CallbackFunction instance
	*/
	static void Fl_Callback( Fl_Widget* widget, void* callback );

	private:

	C& instance_;													/*!< reference to class instance*/
	void (C::*member_function_)( void );	/*!< pointer to member function*/

};

#include "callbackFunction.hpp"