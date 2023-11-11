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
#include "test_model.h"
#include "coordinateSystemTree.h"
#include "progress.h"

#include "mainWindow.h"
#include "processingWindow.h"

/*!
 * @brief Main function
 * @param  
 * @return 
*/
int main( [[maybe_unused]] int argc, [[maybe_unused]] char** argv ){

	/*
	Model testModel = getTestModel( GlobalSystem(), 20 );

	vector<char> modelBin;
	testModel.Serialize( modelBin );
	ExportSerialized(path{ "./testModel_Artifact_20x.model" }, modelBin );

	return 0;
	*/

	

	Fl_Group::current( NULL );
	Fl_Tooltip::enable();
	Fl_Tooltip::hoverdelay( (float) 0.05 );

	mainWindow* mainWindow_ = new mainWindow{ (int) ( 1920. * 0.9 ), (int) ( 1080. * 0.9 ), "CT-Simulator" };

	mainWindow_->hide();

	Fl_Progress_Window* initialWindow = new Fl_Progress_Window{ mainWindow_, 20, 3, "Initialisation" };
	initialWindow->changeLineText( 1, "Loading program state..." );
	initialWindow->show();


	bool firstLoop = true;


	while( Fl::wait() ){
		
		if( firstLoop ){
			mainWindow_->show();
			initialWindow->hide();
			firstLoop = false;
		}
	}

	delete mainWindow_;
	delete initialWindow;

	
	return 0;
	
}
