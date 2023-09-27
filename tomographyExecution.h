#pragma once

/*********************************************************************
 * @file   tomographyExecution.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   April 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include <FL/Fl_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Toggle_Button.H>

#include "programState.h"
#include "widgets.h"

class tomographyExec : public Fl_Group{

	public:

	tomographyExec( int x, int y, int w, int h );

	~tomographyExec( void );

	void handleEvents( void );

	private:

	// Information

	Fl_Box title;

	Fl_Group tomoParameterGrp;
	Fl_Box parameterTitle;
	Fl_Bound_Input<Fl_Float_Input, double> exposureTimeIn;
	Fl_Bound_Input<Fl_Float_Input, double> rayStepSizeIn;
	
	Fl_Counter radiationLoopsIn;
	Fl_Bound_Input<Fl_Float_Input, double> scatterPropabilityIn;
	Fl_Toggle_Button scatteringOnOff;

	Fl_Multiline_Output information;	// Energy, frames

	Fl_Group controlGrp;
	Fl_Button radiationButton;




	bool radiateFlag;
	bool updateFlag;

 };


