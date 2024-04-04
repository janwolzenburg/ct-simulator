#pragma once
/*********************************************************************
 * @file   callbackFunction.h
 * @brief
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
	 * @param class_instance Reference to class instance 
	 * @param member_function Pointer to member function
	*/
	CallbackFunction( C& class_instance, void ( C::*member_function )( void ) );

	/*!
	 * @brief execute member function
	*/
	void Execute( void );

	/*!
	 * @brief function to pass to FLTK callback() function
	 * @param widget Pointer to widget
	 * @param callback Pointer to CallbackFunction instance
	*/
	static void Fl_Callback( Fl_Widget* widget, void* callback );

	private:

	C& instance_;							/*!< reference to class instance*/
	void (C::*member_function_)( void );	/*!< pointer to member function*/

};

#include "callbackFunction.hpp"