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

	
	model testModel = getTestModel( GLOBAL_CSYS(), 20 );

	vector<char> modelBin;
	testModel.serialize( modelBin );
	exportSerialized(path{ "./testModel_Artifact_20x.model" }, modelBin );

	return 0;
	

	
	programState& state = PROGRAM_STATE();

	Fl_Group::current( NULL );
	Fl_Tooltip::enable();
	Fl_Tooltip::hoverdelay( (float) 0.05 );

	mainView* mainWindow = new mainView( (int) ( 1920. * 0.9 ), (int) ( 1080. * 0.9 ), "CT-Simulator" );
	processingView* procView = new processingView( (int) ( 1920. * 0.9 ), (int) ( 1080. * 0.9 ), "Processing" );

	state.registerMainWindow( mainWindow );
	state.registerProcessingWindow( procView );


	procView->hide();


	if( state.RadonTransformedLoaded() ){
		procView->setNewRTFlag();
	}

	while( Fl::wait() ){
		
		mainWindow->handleEvents();
		procView->handleEvents();

	}

	delete mainWindow;
	delete procView;

	return 0;
	
}
