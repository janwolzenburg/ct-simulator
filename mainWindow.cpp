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

	menu{					X( *this, 0. ),							Y( *this, 0.001 ),		W( *this, 1. ),		H( *this, 0.034 ) },
	importSinogramBtn{		X( menu, .7 ),							Y( menu, 0 ),		W( menu, .15 ),		H( menu, 1. ), "Import Sinogram" },
	resetProgramStateBtn{	X( menu, .9 ),							Y( menu, 0. ),		W( menu, .1 ),		H( menu, 1. ), "Reset program" },

	modView{				X( *this, 0. ),							Y( *this, 0.04 ),	W( *this, 0.45 ),	H( *this, .95 ), *this },
	gantryBuild{			hOff( modView ) + X( *this, .025 ),		Y( *this, 0.04 ),	W( *this, 0.25 ),	H( *this, .95 ), *this },
	tomographyExecution{	hOff( gantryBuild ) + X( *this, .025 ), Y( *this, 0.04 ),	W( *this, 0.25 ),	H( *this, .95 ), *this },
	
	importChooserInstance{ "Import Sinogram", "*.sinogram", path{ "./" } },
	storedImportChooser{ PROGRAM_STATE().getPath( "storedImportChooser.txt" ), importChooserInstance },

	setresetAtExitCB{ *this, &mainWindow::resetAtExit },
	importSGCB{ *this, &mainWindow::importSinogram }
	
{
	Fl_Window::resizable( *this );
	Fl_Window::add( menu );
	Fl_Window::add( modView );
	Fl_Window::add( gantryBuild );
	Fl_Window::add( tomographyExecution );

	menu.add( importSinogramBtn );
	importSinogramBtn.callback( CallbackFunction<mainWindow>::Fl_Callback, &importSGCB);

	menu.add( resetProgramStateBtn );
	resetProgramStateBtn.callback( CallbackFunction<mainWindow>::Fl_Callback, &setresetAtExitCB );

	gantryBuild.deactivate();
	tomographyExecution.deactivate();

	if( modView.IsModelLoaded() ){
		gantryBuild.activate();
		tomographyExecution.activate();
		tomographyExecution.updateInformation( gantryBuild.projections_properties(), gantryBuild.gantry().detector().properties(), gantryBuild.gantry().tube() );
	}

	Fl_Window::show();


}


mainWindow::~mainWindow( void ){
	storedImportChooser.Save();
}

void mainWindow::resetAtExit( void ){

	if( fl_choice( "Do you want to reset program status?\nThis will happen at the program's exit!", "Reset", "Keep state", 0 ) == 0 )
		PROGRAM_STATE().resetStateStorageAtExit();
}


void mainWindow::importSinogram( void ){

	storedImportChooser.SetAsLoaded();
	path sgPath =  importChooserInstance.ChooseFile();

	if( sgPath.empty() ) return;

	vector<char> binary_data = ImportSerialized( sgPath );
	vector<char>::const_iterator it = binary_data.cbegin();

	if( ValidBinaryData( Projections::FILE_PREAMBLE, binary_data, it ) ){
		Projections importedSinogram{ binary_data, it };
		tomographyExecution.AssignProjections( importedSinogram );
	}
	
}