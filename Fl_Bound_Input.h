#pragma once
/*********************************************************************
 * @file   Fl_Bound_Input.h
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
enum INPUT_CONSTRAINTS{
	NONE,
	EVEN,
	ODD
};

/*!
 * @brief Class for a bound number_of_pixel input
 * @tparam C Class derived from Fl_Input
 * @tparam T Arithmetic type to input
*/
template< class C, typename T>
class Fl_Bound_Input : public Fl_Group{

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
	Fl_Bound_Input( int x, int y, int w, int h, const char* label );

	/*!
	 * @brief Set alignment for title
	 * @param alignment Alignment
	*/
	void align( Fl_Align alignment ){ input.align( alignment ); };

	/*!
	 * @brief Set new value
	 * @param newValue New value
	*/
	void value( const T newValue );

	/*!
	 * @brief Get current value 
	 * @return Current value
	*/
	T value( void ) const{ return current; }; 

	/*!
	 * @brief Set properties_ of input
	 * @param min_ Minimum value
	 * @param max_ Maximum value
	 * @param precision_ Precision of number_of_pixel to show. Use negative numbers to round to multiples of ten
	 * @param constraint_ Contrains for inputted values
	*/
	void setProperties( const T min_, const T max_, const int precision_, const INPUT_CONSTRAINTS constraint_ = NONE  );


	private:

	/*!
	 * @brief Check if current value is in bounds and violating constrains
	 * @details Updates value to conform
	*/
	void checkBounds( void );

	/*!
	 * @brief Callback for value change
	 * @param widget Pointer to widget which triggered the callback
	 * @param p Pointer to userdata. Here a pointer to the Fl_Bound_Input must be passed
	*/
	static void cbFunction( Fl_Widget* widget, void* p );
	

	C input;				/*!<A Fl_Input derivation*/
				
	int precision;			/*!<Precision of number_of_pixel. Negative to round to significant digits*/
	T current;				/*!<Current value*/
	string valueString;		/*!<String with formatted value*/

	T maxVal;				/*!<Maximum allowed value*/
	T minVal;				/*!<Minimum allowed value*/
	INPUT_CONSTRAINTS constraint;	/*!<Constraints for the input*/

};

#include "Fl_Bound_Input.hpp"