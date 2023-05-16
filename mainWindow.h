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

#include "programState.h"

#include "modelView.h"
#include "gantryCreation.h"
#include "tomographyExecution.h"


class mainView : public Fl_Window{

	public:

	mainView( int w, int h, const char* label ) :
		Fl_Window( w, h, label ),

		modView( X( *this, 0. ), Y( *this, 0. ), W( *this, 0.3 ), H( *this, 1 ) ),
		gantryBuild( hOff( modView ) + X( *this, .025 ), Y( *this, 0. ), W( *this, 0.25 ), H( *this, 1. ) ),
		tomographyExecution( hOff( gantryBuild ) + X( *this, .025 ), Y( *this, 0. ), W( *this, 0.4 ), H( *this, 1. ) )
	{
		Fl_Window::resizable( *this );

		Fl_Window::add( modView );
		Fl_Window::add( gantryBuild );
		Fl_Window::add( tomographyExecution );

		Fl_Window::show();


		if( PROGRAM_STATE().ModelLoaded() ){
			modView.setUpdateFlag();
		}

		gantryBuild.setUpdateFlag();

	}


	void handleEvents( void ){

		modView.handleEvents();
		gantryBuild.handleEvents();
		tomographyExecution.handleEvents();

	}


	public:

	modelView modView;
	gantryEdition gantryBuild;
	tomographyExec tomographyExecution;


};