/*********************************************************************
 * @file   main.cpp
 * @brief  Main file
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/

 #include "generel.h"

#ifndef VERIFY

#include <memory>
using std::unique_ptr;

#include "programState.h"
#include "fl_ProgressWindow.h"
#include "fl_MainWindow.h"


int main( [[maybe_unused]] int argc, [[maybe_unused]] char** argv ){

	// Create instance of program state
	volatile ProgramState& program_state = PROGRAM_STATE();

	Fl_Group::current( NULL );
	Fl_Tooltip::enable();
	Fl_Tooltip::hoverdelay( (float) 0.05 );

	unique_ptr<Fl_MainWindow> mainWindow_ = std::make_unique<Fl_MainWindow>( static_cast<int>( 1920. * 0.9 ), static_cast<int>( 1080. * 0.9 ), "CT-Simulator" );

	mainWindow_->hide();

	unique_ptr<Fl_Progress_Window> initialWindow = std::make_unique<Fl_Progress_Window>( mainWindow_.get(), 20, 3, "Initialisation");
	initialWindow->ChangeLineText( 1, "Loading program state..." );
	initialWindow->show();


	bool firstLoop = true;
	while( Fl::wait() ){
		
		if( firstLoop ){
			mainWindow_->show();
			initialWindow->hide();
			firstLoop = false;
		}
	}

	return 0;
	
}

#else

#include "verifyDevice.h"
#include "verifyTransmission.h"
#include "verifyprocessing.h"

int main( void ){

	BuildTestDetector();
	TestRadonCoordinates();
	TestSpectrum();
	TestBeamCreation();

	VerifyTransmission();
	VerifyHardening();
	//VerifyScattering();

	VerifyFilteredprojections();

	return 0;

}

#endif