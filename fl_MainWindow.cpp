/*********************************************************
* @file   fl_MainWindow.cpp
*
* @author Jan Wolzenburg
* @date   October 2023
* ********************************************************************/


/*********************************************************************
	Includes
 *********************************************************************/
 
#include "fl_MainWindow.h"
#include "FL/fl_ask.H"
#include "widgets.h"
#include "serialization.h"


 /*********************************************************************
	Implementations
 *********************************************************************/

Fl_MainWindow::Fl_MainWindow( int w, int h, const char* label ) :
	Fl_Window{ w, h, label },

	menu_group_{													X( *this, 0. ),				Y( *this, 0.005 ),			W( *this, 1. ),						H( *this, 0.034 ) },
	model_creator_button_{								X( menu_group_, .1 ),	Y( menu_group_, 0 ),		W( menu_group_, .15 ),		H( menu_group_, 1. ), "Create model" },
	import_projections_button_{						X( menu_group_, .7 ),	Y( menu_group_, 0 ),		W( menu_group_, .15 ),		H( menu_group_, 1. ), "Import Sinogram" },
	reset_program_state_at_exit_button_{	X( menu_group_, .9 ),	Y( menu_group_, 0. ),		W( menu_group_, .1 ),			H( menu_group_, 1. ), "Reset program" },
	author_name_{													w - 170,							h - 20,									160,												20,										""},
	git_repository_{											20,										h - 22,									260,											20,										"https://github.com/janwolzenburg/ct-simulator"},

	model_view_{							X( *this, 0. ),																Y( *this, 0.045 ),	W( *this, 0.4 ),	H( *this, .95 ), *this },
	gantry_creation_{					hOff( model_view_ ) + X( *this, .025 ),				Y( *this, 0.045 ),	W( *this, 0.3 ),	H( *this, .95 ), *this },
	tomography_execution_{		hOff( gantry_creation_ ) + X( *this, .025 ),	Y( *this, 0.045 ),	W( *this, 0.25 ),	H( *this, .95 ), *this },
	
	import_projections_file_chooser_{ FileChooser{ "Import Sinogram", "*.projections", path{ "./" } }, "import.chooser", false },
	
	reset_program_state_callback_{ *this, &Fl_MainWindow::SetResetAtExit },
	import_projections_callback_{ *this, &Fl_MainWindow::ImportProjections },
	create_model_callback_{ *this, &Fl_MainWindow::CreateModel }
	
{

	Fl_Window::resizable( *this );
	Fl_Window::add( menu_group_ );
	Fl_Window::add( model_view_ );
	Fl_Window::add( gantry_creation_ );
	Fl_Window::add( tomography_execution_ );
	Fl_Window::add( author_name_ );
	Fl_Window::add( git_repository_ );

	menu_group_.add( import_projections_button_ );
	import_projections_button_.callback( CallbackFunction<Fl_MainWindow>::Fl_Callback, &import_projections_callback_);

	menu_group_.add( reset_program_state_at_exit_button_ );
	reset_program_state_at_exit_button_.callback( CallbackFunction<Fl_MainWindow>::Fl_Callback, &reset_program_state_callback_ );
	
	menu_group_.add( model_creator_button_ );
	model_creator_button_.callback( CallbackFunction<Fl_MainWindow>::Fl_Callback, &create_model_callback_ );

	author_name_.align( FL_ALIGN_CENTER );
	author_name_.labelsize( FL_NORMAL_SIZE );
	author_name_.copy_label( string{ "Copyright: Jan Wolzenburg" }.c_str() );

	git_repository_.align( FL_ALIGN_CENTER );
	git_repository_.labelsize( FL_NORMAL_SIZE );

	gantry_creation_.deactivate();
	tomography_execution_.deactivate();

	if( model_view_.IsModelLoaded() ){
		gantry_creation_.activate();
		tomography_execution_.activate();
		tomography_execution_.UpdateInformation( gantry_creation_.projections_properties(), gantry_creation_.gantry().tube() );
	}

	Fl_Window::show();


}


void Fl_MainWindow::SetResetAtExit( void ){

	if( fl_choice( "Do you want to reset program status?\nThis will happen at the program's exit!", "Reset", "Keep state", 0 ) == 0 )
		PROGRAM_STATE().SetResetStateAtExit();
}


void Fl_MainWindow::ImportProjections( void ){

	path projections_path =  import_projections_file_chooser_.ChooseFile();
	import_projections_file_chooser_.SetAsLoaded();

	if( projections_path.empty() ) return;
	
	PersistingObject<Projections> tempory_projections{ Projections{}, projections_path, true };

	tomography_execution_.AssignProjections( static_cast<Projections>( tempory_projections ) );
	
}

void Fl_MainWindow::CreateModel( void ){

	std::unique_ptr<Fl_ModelCreator> ptr = std::make_unique<Fl_ModelCreator>(  static_cast<int>( 1440 ), static_cast<int>( 720 ), "Model Creator" );
	creator_windows_.push_back( std::move( ptr ) );
	creator_windows_.back()->show();

}