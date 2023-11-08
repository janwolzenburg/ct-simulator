#pragma once
/*********************************************************************
 * @file   modelCreator.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   November 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include "FL/Fl_Window.H"
#include "FL/Fl_Group.H"
#include "FL/Fl_Button.H"

#include "programState.h"
#include "model.h"



 /*********************************************************************
	Definitions
 *********************************************************************/

/*!
 * @brief Class for a window that can build models
*/
class ModelCreator : public Fl_Window{

	public:

	/*!
	 * @brief Constructor
	 * @param w Width of window
	 * @param h Height of window
	 * @param label Window title
	*/
	ModelCreator( int w, int h, const char* label );



}