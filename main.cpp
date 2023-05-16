/*********************************************************************
 * @file   main.cpp
 * @brief  Main file
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/

#include <iostream>
using std::cerr;  using std::endl; using std::cout;

#include <type_traits>

#include "programState.h"
#include "gui.h"
#include "test_model.h"
#include "cSysTree.h"

#include "mainWindow.h"

/*!
 * @brief Main function
 * @param  
 * @return 
*/
int main( int argc, char** argv ){

	//model testModel = getTestModel( GLOBAL_CSYS(), 20 );

	//vector<char> modelBin;
	//testModel.serialize( modelBin );
	//exportSerialized(path{ "./testModel_20x.model" }, modelBin );

	//return 0;

	programState& state = PROGRAM_STATE();

	Fl_Group::current( NULL );
	Fl_Tooltip::enable();
	Fl_Tooltip::hoverdelay( (float) 0.05 );

	mainView* mainWindow = new mainView( (int) ( 1920. * 0.9 ), (int) ( 1080. * 0.9 ), "CT-Simulator" );

	//Fl_Window* mainWindow = new Fl_Window( (int) ( 1920.*0.9 ), (int) ( 1080.*0.9 ), "CT-Simulator" );
	//mainWindow->resizable( mainWindow );

	//modelView* modView = new modelView(				X( *mainWindow, 0. ),	Y( *mainWindow, 0. ),			W( *mainWindow, 0.3 ),	H( *mainWindow, 1 ) );
	//mainWindow->add( modView );
	//
	//gantryEdition gantryBuild{			hOff( *modView ) + X( *mainWindow, .025 ),	Y( *mainWindow, 0. ),	W( *mainWindow, 0.25 ),	H( *mainWindow, 1. ) };
	//mainWindow->add( gantryBuild );
	//
	//tomographyExec tomographyExecution( hOff( gantryBuild ) + X( *mainWindow, .025 ), Y( *mainWindow, 0. ),	W( *mainWindow, 0.4 ), H( *mainWindow, 1. ) );
	//mainWindow->add( tomographyExecution );


	processingView* procView = new processingView( 1280, 720, "Processing" );
	procView->hide();


	while( Fl::wait() ){
		
		mainWindow->handleEvents();
	}

	delete mainWindow;


	return 0;
}
