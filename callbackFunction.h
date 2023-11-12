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
 * @brief Class to store information about an object instance and a pointer to a member function
 * @tparam C Class
*/
template<typename C>
class CallbackFunction{

	public:

	/*!
	 * @brief Constructor
	 * @param class_instance Reference to class instance 
	 * @param member_function Pointer to member function
	*/
	CallbackFunction( C& class_instance, void ( C::*member_function )( void )  );

	/*!
	 * @brief Execute member function
	*/
	void Execute( void );

	/*!
	 * @brief Function to pass to FLTK callback() function
	 * @param widget Pointer to widget
	 * @param callback Pointer to CallbackFunction instance
	*/
	static void Fl_Callback( Fl_Widget* widget, void* callback );

	private:

	C& instance_;							/*!< Reference to class instance*/
	void (C::*member_function_)( void );	/*!< Pointer to member function*/

};

#include "callbackFunction.hpp"