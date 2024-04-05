#pragma once
/*********************************************************************
 * @file   fl_BoundInput.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   October 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include <type_traits>
#include <string>
using std::string;
#include <FL/Fl_Group.H>


 /*********************************************************************
	Definitions
 *********************************************************************/

/*!
 * @brief constraints for the bound input
*/
enum Input_Constraints{
	None,
	Even,
	Odd
};

/*!
 * @brief class for a bound number input
 * @tparam C class derived from Fl_Input
 * @tparam T arithmetic type to input
*/
template< class C, typename T>
class Fl_BoundInput : public Fl_Group{

	static_assert( std::is_base_of_v< Fl_Input, C > );
	static_assert( std::is_arithmetic_v< T >);

	
	public:

	/*!
	 * @brief constructor
	 * @param x x-position
	 * @param y y-position
	 * @param w width
	 * @param h height
	 * @param label label
	*/
	Fl_BoundInput( int x, int y, int w, int h, const char* label );
	
	/*!
	 * @brief get current value 
	 * @return current value
	*/
	T value( void ) const{ return current_value_; }; 

	/*!
	 * @brief set alignment for title
	 * @param alignment alignment
	*/
	void align( Fl_Align alignment ){ input_widget_.align( alignment ); };

	/*!
	 * @brief set new value
	 * @param new_value new value
	*/
	void value( const T new_value );

	/*!
	 * @brief set properties of input
	 * @param min_value minimum value
	 * @param max_value maximum value
	 * @param precision precision of number_of_pixel to show. Use negative numbers to round to multiples of ten
	 * @param constraint contrains for inputted values
	*/
	void SetProperties( const T min_value, const T max_value, const int precision, const Input_Constraints constraint = None  );


	private:

	C input_widget_;				/*!< a Fl_Input derivation*/
				
	int precision_;					/*!< precision of number_of_pixel. Negative to round to significant digits*/
	T current_value_;				/*!< current value*/
	string value_string_;		/*!< string with formatted value*/

	T max_allowed_;					/*!< maximum allowed value*/
	T min_allowed_;					/*!< minimum allowed value*/
	Input_Constraints constraint_;	/*!< constraints for the input*/

	/*!
	 * @brief check if current value is in bounds and violating constrains
	 * @details updates value to conform
	*/
	void CheckAndForceConstraints( void );

	/*!
	 * @brief callback for value change
	 * @param widget pointer to widget which triggered the callback
	 * @param user_data pointer to userdata. here a pointer to the Fl_BoundInput must be passed
	*/
	static void HandleValueChange( Fl_Widget* widget, void* user_data );

};

#include "fl_BoundInput.hpp"