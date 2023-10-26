#pragma once
/*********************************************************************
 * @file   Fl_OwnValuator.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   March 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include <FL/Fl_Valuator.H>
#include <FL/Fl_Counter.H>
#include "generel.h"


/*********************************************************************
   Definitions
*********************************************************************/

/*!
 * @brief Class for a valuator with custom number to string conversion
 * @details Able to display the current valuator value in the label
 * @tparam C Class derived from Fl_Valuator
*/
template< class C >
class Fl_OwnValuator : public C{

	static_assert( std::is_base_of_v< Fl_Valuator, C > );

	public:

	/*!
	 * @brief Constructor
	 * @param x x-position
	 * @param y y-position
	 * @param w Width
	 * @param h Height
	 * @param label Label
	*/
	Fl_OwnValuator( int x, int y, int w, int h, const char* label );

	/*!
	 * @brief Check if value changed and reset the flag
	 * @return True, when value changed
	*/
	bool ChangeFlag( void ){ return unsetFlag( changeFlag ); };

	/*!
	 * @brief Get the current value
	 * @return Value
	*/
	double value( void ) const{ return Fl_Valuator::value(); };

	/*!
	 * @brief Set new value
	 * @param newVal New value
	 * @return Return value from value() as stated in FLTK documentation
	*/
	int value( double newVal );

	/*!
	 * @brief Activate or deactivate the conversion of the value to the label
	 * @param yesNo true or false
	*/
	void ValueToLabel( const bool yesNo );


	private:

	/*!
	 * @brief Update the label to current value
	*/
	void updateLabel( void );
	
	/*!
	 * @brief Callback for value change
	 * @param widget Pointer to widget that triggered the callback
	 * @param p Pointer to a boolean, which will be set to true
	*/
	static void cbFunction( Fl_Widget* widgetPtr, void* flag );

	private:
	
	bool changeFlag;	/*!<Flag to track a value change*/
	bool valueToLabel;	/*!<Flag to activate the value to label conversion*/

};

#include "Fl_OwnValuator.hpp"