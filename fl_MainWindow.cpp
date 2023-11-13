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
 
#include "fl_MainWindow.h"
#include "FL/Fl_ask.H"
#include "widgets.h"
#include "serialization.h"


 /*********************************************************************
	Implementations
 *********************************************************************/

Fl_MainWindow::Fl_MainWindow( int w, int h, const char* label ) :
	Fl_Window{ w, h, label },

	menu_group_{					X( *this, 0. ),							Y( *this, 0.001 ),		W( *this, 1. ),		H( *this, 0.034 ) },
	import_projections_button_{		X( menu_group_, .7 ),							Y( menu_group_, 0 ),		W( menu_group_, .15 ),		H( menu_group_, 1. ), "Import Sinogram" },
	reset_program_state_at_exit_button_{	X( menu_group_, .9 ),							Y( menu_group_, 0. ),		W( menu_group_, .1 ),		H( menu_group_, 1. ), "Reset program" },

	model_view_{				X( *this, 0. ),							Y( *this, 0.04 ),	W( *this, 0.4 ),	H( *this, .95 ), *this },
	gantry_creation_{			hOff( model_view_ ) + X( *this, .025 ),		Y( *this, 0.04 ),	W( *this, 0.3 ),	H( *this, .95 ), *this },
	tomography_execution_{	hOff( gantry_creation_ ) + X( *this, .025 ), Y( *this, 0.04 ),	W( *this, 0.25 ),	H( *this, .95 ), *this },
	
	import_projections_file_chooser_{ FileChooser{ "Import Sinogram", "*.sinogram", path{ "./" } }, "import.chooser", false },
	
	reset_program_state_callback_{ *this, &Fl_MainWindow::SetResetAtExit },
	import_projections_callback_{ *this, &Fl_MainWindow::ImportProjections }
	
{

	Fl_Window::resizable( *this );
	Fl_Window::add( menu_group_ );
	Fl_Window::add( model_view_ );
	Fl_Window::add( gantry_creation_ );
	Fl_Window::add( tomography_execution_ );

	menu_group_.add( import_projections_button_ );
	import_projections_button_.callback( CallbackFunction<Fl_MainWindow>::Fl_Callback, &import_projections_callback_);

	menu_group_.add( reset_program_state_at_exit_button_ );
	reset_program_state_at_exit_button_.callback( CallbackFunction<Fl_MainWindow>::Fl_Callback, &reset_program_state_callback_ );

	gantry_creation_.deactivate();
	tomography_execution_.deactivate();

	if( model_view_.IsModelLoaded() ){
		gantry_creation_.activate();
		tomography_execution_.activate();
		tomography_execution_.UpdateInformation( gantry_creation_.projections_properties(), gantry_creation_.gantry().detector().properties(), gantry_creation_.gantry().tube() );
	}

	Fl_Window::show();


}


void Fl_MainWindow::SetResetAtExit( void ){

	if( fl_choice( "Do you want to reset program status?\nThis will happen at the program's exit!", "Reset", "Keep state", 0 ) == 0 )
		PROGRAM_STATE().resetStateStorageAtExit();
}


void Fl_MainWindow::ImportProjections( void ){

	path sgPath =  import_projections_file_chooser_.ChooseFile();
	import_projections_file_chooser_.SetAsLoaded();

	if( sgPath.empty() ) return;
	
	PersistingObject<Projections> tempory_projections{ Projections{}, sgPath, true };

	tomography_execution_.AssignProjections( static_cast<Projections>( tempory_projections ) );
	
}