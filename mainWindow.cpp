/*********************************************************
* @file   programState.cpp
* @brief  Implementations
*
* @author Jan Wolzenburg
* @date   October 2023
* ********************************************************************/


/*********************************************************************
	Includes
 *********************************************************************/
 
#include "mainWindow.h"
#include "FL/Fl_ask.H"
#include "widgets.h"
#include "serialization.h"


 /*********************************************************************
	Implementations
 *********************************************************************/

mainWindow::mainWindow( int w, int h, const char* label ) :
	Fl_Window{ w, h, label },

	menu{					X( *this, 0. ),							Y( *this, 0. ),		W( *this, 1. ),		H( *this, 0.035 ) },

	importSinogramBtn{		X( menu, .7 ),							Y( menu, 0 ),		W( menu, .15 ),		H( menu, 1. ), "Import Sinogram" },
	importSinogramFlag( false ),

	resetProgramStateBtn{	X( menu, .9 ),							Y( menu, 0. ),		W( menu, .1 ),		H( menu, 1. ), "Reset program" },
	resetButtonPressed( false ),
	modView{				X( *this, 0. ),							Y( *this, 0.04 ),	W( *this, 0.3 ),	H( *this, .95 ) },
	gantryBuild{			hOff( modView ) + X( *this, .025 ),		Y( *this, 0.04 ),	W( *this, 0.25 ),	H( *this, .95 ) },
	tomographyExecution{	hOff( gantryBuild ) + X( *this, .025 ), Y( *this, 0.04 ),	W( *this, 0.4 ),	H( *this, .95 ) }
{
	Fl_Window::resizable( *this );

	Fl_Window::add( menu );

	menu.add( importSinogramBtn );
	importSinogramBtn.callback( button_cb, &importSinogramFlag );

	menu.add( resetProgramStateBtn );
	resetProgramStateBtn.callback( button_cb, &resetButtonPressed );

	Fl_Window::add( modView );
	Fl_Window::add( gantryBuild );
	Fl_Window::add( tomographyExecution );

	Fl_Window::show();


	if( modView.ModelLoaded() ){
		modView.setUpdateFlag();
	}

	gantryBuild.setUpdateFlag();

}

void mainWindow::deactivate( void ){
	Fl_Window::deactivate();
	menu.deactivate();
	modView.deactivate();
	gantryBuild.deactivate();
	tomographyExecution.deactivate();
}

void mainWindow::activate( void ){
	Fl_Window::activate();
	menu.activate();
	modView.activate();
	gantryBuild.activate();
	tomographyExecution.activate();
}

void mainWindow::handleEvents( void ){

	

	if( UnsetFlag( resetButtonPressed ) ){
		if( fl_choice( "Do you want to reset program status?\nThis will happen at the program's exit!", "Reset", "Keep state", 0 ) == 0 )
			PROGRAM_STATE().resetStateStorageAtExit();
	}

	if( UnsetFlag( importSinogramFlag ) ){

		path chosenPath = PROGRAM_STATE().importSinogram();

		if( chosenPath.empty() ) return;

		vector<char> binary_data = ImportSerialized( chosenPath );
		vector<char>::const_iterator it = binary_data.cbegin();

		if( ValidBinaryData( Projections::FILE_PREAMBLE, binary_data, it ) ){
			Projections importedSinogram{ binary_data, it };
			PROGRAM_STATE().assignRadonTransformed( importedSinogram );
		}
	}

	
	if( modView.ModelLoaded() && !tomographyExecution.active() )
		tomographyExecution.activate();
	else if( !modView.ModelLoaded() )
		tomographyExecution.deactivate();


	modView.handleEvents();
	gantryBuild.handleEvents();
	tomographyExecution.handleEvents();

}