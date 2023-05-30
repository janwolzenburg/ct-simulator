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
#include "processingWindow.h"


#include "programState.h"
#include "widgets.h"
#include "backprojection.h"

class tomographyExec : public Fl_Group{

	public:

	tomographyExec( int x, int y, int w, int h ) : 
		Fl_Group( x, y, w, h ),

		tomoParameterGrp(	X( *this, .05 ), Y( *this, .05 ),								W( *this, .9 ),			H( *this, .1 ) ),
		exposureTimeIn(		X( tomoParameterGrp, 0. ), Y( tomoParameterGrp, .2 ), W( tomoParameterGrp, .2 ), H( tomoParameterGrp, .25 ), "Exposure time"),
		scatteringOnOff(	X( tomoParameterGrp, .5), Y( tomoParameterGrp, .2 ), W( tomoParameterGrp, .2 ), H( tomoParameterGrp, .25 ), "Scattering"),

		controlGrp(			X( *this, .05 ), vOff( tomoParameterGrp ) + Y( *this, .05 ),	W( *this, .9 ),			H( *this, .1 ) ),
		radiationButton(	X( controlGrp, 0. ), Y( controlGrp, 0. ),						W( controlGrp, .3 ),	H( controlGrp, .3 ), "Record Slice"),

		radiateFlag( false ),
		updateFlag( false )
	{
		Fl_Group::box( FL_BORDER_BOX );

		Fl_Group::add( tomoParameterGrp );
		tomoParameterGrp.box( FL_BORDER_BOX );

		tomoParameterGrp.add( exposureTimeIn );
		tomoParameterGrp.add( scatteringOnOff );

		exposureTimeIn.align( FL_ALIGN_TOP );

		exposureTimeIn.setProperties( 0.001, 10., 3 );
		exposureTimeIn.value( PROGRAM_STATE().TomographyParameter().ExposureTime() );
		scatteringOnOff.value( PROGRAM_STATE().TomographyParameter().Scattering() );

		exposureTimeIn.callback( button_cb, &updateFlag );
		scatteringOnOff.callback( button_cb, &updateFlag );

		Fl_Group::add( controlGrp );
		controlGrp.box( FL_BORDER_BOX );

		controlGrp.add( radiationButton );
		radiationButton.callback( button_cb, &radiateFlag );
	
		this->deactivate();
	}

	~tomographyExec( void ){
		
	}


	void handleEvents( void ){

		programState& state = PROGRAM_STATE();

		if( state.ModelLoaded() && !this->active() ) this->activate();
		else if( !state.ModelLoaded() ) this->deactivate();

		if( radiateFlag ){


			Fl_Group::window()->deactivate();

			radiateFlag = false;

			state.Tomography() = tomography( state.TomographyParameter() );
			state.assignRadonTransformed( state.Tomography().recordSlice( state.Gantry(), state.Model(), 0 ) );

			if( state.ProcessingWindow() != nullptr ){
				state.ProcessingWindow()->setNewRTFlag();
			}

			Fl_Group::window()->activate();
		}

		if( updateFlag ){
			updateFlag = false;

			state.TomographyParameter() = tomographyParameter( exposureTimeIn.value(), (bool) scatteringOnOff.value() );

		}

	}

	private:

	// Information

	Fl_Group tomoParameterGrp;
	Fl_Bound_Input<Fl_Float_Input, double> exposureTimeIn;
	Fl_Toggle_Button scatteringOnOff;

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
	bool updateFlag;

 };


