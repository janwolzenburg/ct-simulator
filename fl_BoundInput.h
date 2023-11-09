#pragma once
/*********************************************************************
 * @file   Fl_BoundInput.h
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
 * @brief Constraints for the bound input
*/
enum Input_Constraints{
	None,
	Even,
	Odd
};

/*!
 * @brief Class for a bound number input
 * @tparam C Class derived from Fl_Input
 * @tparam T Arithmetic type to input
*/
template< class C, typename T>
class Fl_BoundInput : public Fl_Group{

	static_assert( std::is_base_of_v< Fl_Input, C > );
	static_assert( std::is_arithmetic_v< T >);

	
	public:

	/*!
	 * @brief Constructor
	 * @param x x-position
	 * @param y y-position
	 * @param w Width
	 * @param h Height
	 * @param label Label
	*/
	Fl_BoundInput( int x, int y, int w, int h, const char* label );
	
	/*!
	 * @brief Get current value 
	 * @return Current value
	*/
	T value( void ) const{ return current_value_; }; 

	/*!
	 * @brief Set alignment for title
	 * @param alignment Alignment
	*/
	void align( Fl_Align alignment ){ input_widget_.align( alignment ); };

	/*!
	 * @brief Set new value
	 * @param newValue New value
	*/
	void value( const T newValue );

	/*!
	 * @brief Set properties of input
	 * @param min_value Minimum value
	 * @param max_value Maximum value
	 * @param precision Precision of number_of_pixel to show. Use negative numbers to round to multiples of ten
	 * @param constraint Contrains for inputted values
	*/
	void SetProperties( const T min_value, const T max_value, const int precision, const Input_Constraints constraint = None  );


	private:

	C input_widget_;				/*!<A Fl_Input derivation*/
				
	int precision_;					/*!<Precision of number_of_pixel. Negative to round to significant digits*/
	T current_value_;				/*!<Current value*/
	string value_string_;			/*!<String with formatted value*/

	T max_allowed_;					/*!<Maximum allowed value*/
	T min_allowed_;					/*!<Minimum allowed value*/
	Input_Constraints constraint_;	/*!<Constraints for the input*/

	/*!
	 * @brief Check if current value is in bounds and violating constrains
	 * @details Updates value to conform
	*/
	void CheckAndForceConstraints( void );

	/*!
	 * @brief Callback for value change
	 * @param widget Pointer to widget which triggered the callback
	 * @param p Pointer to userdata. Here a pointer to the Fl_BoundInput must be passed
	*/
	static void HanldeValueChange( Fl_Widget* widget, void* p );

};

#include "fl_BoundInput.hpp"