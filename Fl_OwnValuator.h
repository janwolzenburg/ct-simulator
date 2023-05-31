#pragma once
/*********************************************************************
 * @file   OwnValuator.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   March 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include <string>
using std::string;

#include <FL/Fl_Valuator.H>
#include <FL/Fl_Counter.H>


/*********************************************************************
   Definitions
*********************************************************************/



template< class C >
class Fl_OwnValuator : public C{

	static_assert( std::is_base_of_v< Fl_Valuator, C > );


	public:

	Fl_OwnValuator( int x, int y, int w, int h, const char* label );

	static void cbFunction( Fl_Widget* widgetPtr, void* flag );

	bool ChangeFlag( void );

	double value( void );

	int value( double newVal );

	void ValueToLabel( const bool yesNo );


	private:

	void updateLabel( void );


	private:
	bool changeFlag;
	bool valueToLabel;
	string labelString;


};

#include "Fl_OwnValuator.hpp"