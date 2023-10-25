#pragma once

#include <type_traits>
#include <string>
using std::string;
#include <FL/Fl_Group.H>


enum INPUT_CONSTRAINTS{
	NONE,
	EVEN,
	ODD
};

template< class C, typename T>
class Fl_Bound_Input : public Fl_Group{

	static_assert( std::is_base_of_v< Fl_Input, C > );
	static_assert( std::is_arithmetic_v< T >);

	
	public:

	Fl_Bound_Input( int x, int y, int w, int h, const char* label );

	void align( Fl_Align alignment );

	void value( const T newValue );

	T value( void ) const;

	void setProperties( const T min_, const T max_, const int precision_, const INPUT_CONSTRAINTS constraint_ = NONE  );

	void checkBounds( void );

	static void cbFunction( Fl_Widget* widget, void* p );
	

	private:

	C input;

	int precision;
	T current;
	string valueString;

	T maxVal;
	T minVal;
	INPUT_CONSTRAINTS constraint;

};

#include "Fl_Bound_Input.hpp"