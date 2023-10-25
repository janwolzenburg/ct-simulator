#pragma once
/*********************************************************************
 * @file   widgets.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   March 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include <FL/Fl_Box.H>

#include <FL/Fl_Group.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Tooltip.H>

#include "Fl_GridImage.h"
#include "Fl_OwnValuator.h"
#include "Fl_Plot.h"




 /*********************************************************************
	Definitions
 *********************************************************************/

void button_cb( Fl_Widget* widget, void* flag );

template<class C>
int X( const C& parent, const double start );

template<class C>
int Y( const C& parent, const double start );

template<class C>
int W( const C& parent, const double portion );

template<class C>
int H( const C& parent, const double portion );

template<class C>
int vOff( const C& parent );

template<class C>
int hOff( const C& parent );

#include "widgets.hpp"