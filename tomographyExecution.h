#pragma once
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

#include "programState.h"
#include "widgets.h"
#include "backprojection.h"

class tomographyExec : public Fl_Group{

	public:

	tomographyExec( int x, int y, int w, int h ) : 
		Fl_Group( x, y, w, h ),

		tomoParameterGrp(	X( *this, .05 ), Y( *this, .05 ),								W( *this, .9 ),			H( *this, .1 ) ),

		controlGrp(			X( *this, .05 ), vOff( tomoParameterGrp ) + Y( *this, .05 ),	W( *this, .9 ),			H( *this, .1 ) ),
		radiationButton(	X( controlGrp, 0. ), Y( controlGrp, 0. ),						W( controlGrp, .3 ),	H( controlGrp, .3 ), "Record Slice"),

		radiateFlag( false )
	{
		Fl_Group::box( FL_BORDER_BOX );

		Fl_Group::add( tomoParameterGrp );
		tomoParameterGrp.box( FL_BORDER_BOX );

		Fl_Group::add( controlGrp );
		controlGrp.box( FL_BORDER_BOX );

		controlGrp.add( radiationButton );
		radiationButton.callback( button_cb, &radiateFlag );
	}


	void handleEvents( void ){

		programState& state = PROGRAM_STATE();


		if( radiateFlag ){


			Fl_Group::window()->deactivate();

			radiateFlag = false;

			state.Projections() = state.Tomography().recordSlice(state.Gantry(), state.Model(), 0.);

			//filteredProjections filteredProjections( projections, discreteFilter::ramLak );

			//reconstrucedImage reconstruction( filteredProjections );



			Fl_Group::window()->activate();
		}


	}

	private:

	// Information

	Fl_Group tomoParameterGrp;
	//Fl_Bound_Input<Fl_Float_Input, double> exposureTimeIn;

	// Control
	
	Fl_Group controlGrp;
	Fl_Button radiationButton;
	// Button Go


	// 
	// Filter
	// Simulation

	// Projections

	// Filtered projection

	// Reconstructed image


	bool radiateFlag;


 };



class tomographyView : public Fl_Window{

	

 };