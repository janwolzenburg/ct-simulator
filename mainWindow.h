#pragma once
/*********************************************************************
 * @file   processingWindow.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   May 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include "FL/Fl_Window.H"
#include "FL/Fl_Button.H"
#include "FL/fl_ask.H"

#include "programState.h"

#include "modelView.h"
#include "gantryCreation.h"
#include "tomographyExecution.h"
//#include "confirmButton.h"

class mainView : public Fl_Window{

	public:

	mainView( int w, int h, const char* label ) :
		Fl_Window( w, h, label ),

		menu( X( *this, 0. ), Y( *this, 0. ), W( *this, 1. ), H( *this, 0.035 ) ),

		importSinogramBtn(	X( menu, .7 ), Y( menu, 0 ), W( menu, .15 ), H( menu, 1. ), "Import Sinogram" ),
		importSinogramFlag( false ),

		resetProgramStateBtn{ X( menu, .9 ), Y( menu, 0. ), W( menu, .1 ), H( menu, 1. ), "Reset program" },
		resetButtonPressed( false ),
		modView( X( *this, 0. ), Y( *this, 0.04 ), W( *this, 0.3 ), H( *this, .95 ) ),
		gantryBuild( hOff( modView ) + X( *this, .025 ), Y( *this, 0.04 ), W( *this, 0.25 ), H( *this, .95 ) ),
		tomographyExecution( hOff( gantryBuild ) + X( *this, .025 ), Y( *this, 0.04 ), W( *this, 0.4 ), H( *this, .95 ) )
	{
		Fl_Window::resizable( *this );

		Fl_Window::add( menu );

		menu.add( importSinogramBtn );
		importSinogramBtn.callback( button_cb, &importSinogramFlag );

		menu.add( resetProgramStateBtn );
		resetProgramStateBtn.callback( button_cb, &resetButtonPressed );

		//resetProgramStateBtn.setTitle( "Confirm program reset" );
		//resetProgramStateBtn.setWindowText( "Do you want to reset program status?\nThis will happen at the program's exit!" );
		//resetProgramStateBtn.setButtonTexts( "Reset", "Don't reset" );

		Fl_Window::add( modView );
		Fl_Window::add( gantryBuild );
		Fl_Window::add( tomographyExecution );

		Fl_Window::show();


		if( PROGRAM_STATE().ModelLoaded() ){
			modView.setUpdateFlag();
		}

		gantryBuild.setUpdateFlag();

	}

	void deactivate( void ){
		Fl_Window::deactivate();
		menu.deactivate();
		modView.deactivate();
		gantryBuild.deactivate();
		tomographyExecution.deactivate();
	}

	void activate( void ){
		Fl_Window::activate();
		menu.activate();
		modView.activate();
		gantryBuild.activate();
		tomographyExecution.activate();
	}



	void handleEvents( void );


	public:

	Fl_Group menu;
	Fl_Button importSinogramBtn;
	bool importSinogramFlag;
	Fl_Button resetProgramStateBtn;
	bool resetButtonPressed;
	//Fl_Confirm_Button resetProgramStateBtn;
	modelView modView;
	gantryEdition gantryBuild;
	tomographyExec tomographyExecution;


};