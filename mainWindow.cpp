#include "mainWindow.h"
#include "processingWindow.h"

void mainView::handleEvents( void ){

	//resetProgramStateBtn.handleEvents();
	if( unsetFlag( resetButtonPressed ) ){
		if( fl_choice( "Do you want to reset program status?\nThis will happen at the program's exit!", "Reset", "Keep state", 0 ) == 0 )
			PROGRAM_STATE().resetStateStorageAtExit();
	}

	if( unsetFlag( importSinogramFlag ) ){

		path chosenPath = PROGRAM_STATE().importSinogram();

		if( chosenPath.empty() ) return;

		vector<char> binData = importSerialized( chosenPath );
		vector<char>::const_iterator it = binData.cbegin();

		if( validBinaryData( radonTransformed::FILE_PREAMBLE, binData, it ) ){
			radonTransformed importedSinogram{ binData, it };
			PROGRAM_STATE().assignRadonTransformed( importedSinogram );

			if( PROGRAM_STATE().processingWindow != nullptr ){
				PROGRAM_STATE().processingWindow->setNewRTFlag();
			}
		}
	}


	modView.handleEvents();
	gantryBuild.handleEvents();
	tomographyExecution.handleEvents();

}