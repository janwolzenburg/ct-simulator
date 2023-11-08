/*********************************************************
* @file   programState.cpp
* @brief  Implementations
*
* @author Jan Wolzenburg
* @date   March 2023
* ********************************************************************/


/*********************************************************************
	Includes
 *********************************************************************/

#include "programState.h"
#include "coordinateSystemTree.h"
#include "mainWindow.h"
#include "processingWindow.h"
#include "serialization.h"
#include "gantryCreation.h"

 /*********************************************************************
	Implementations
 *********************************************************************/


#ifdef WIN32
	const path programState::stateStorage{ ".\\stateStorage\\" };
#else
	const path programState::stateStorage{ "./stateStorage/" };
#endif // WIN32


programState& PROGRAM_STATE( void ){
	return programState::getInstance();
}


programState& programState::getInstance(){
	static programState instance;
	return instance;
}

programState::programState( void ) :
	


	mainWindow_( nullptr ),

	resetStateAtExit( false ),
	

	importChooserInstance{ "Import Sinogram", "*.sinogram", path{ "./" } },
	storedImportChooser{ getPath( "storedImportChooser.txt" ), importChooserInstance }

{
	createStorageDir();
}

programState::~programState( void ) {
	
	if( resetStateAtExit ){
		deleteStorageDir();
		return;
	}
	



	storedImportChooser.Save();
}


void programState::createStorageDir( void ){
	// Check if state storage directory erxists
	if( !std::filesystem::is_directory( stateStorage ) ) std::filesystem::create_directory( stateStorage );
}

void programState::deleteStorageDir( void ){

	// Check if state storage directory erxists
	if( !std::filesystem::is_directory( stateStorage ) ) return;

	// Remove all content
	for( const auto& file : std::filesystem::directory_iterator( stateStorage ) )
		std::filesystem::remove_all( file.path() );
}


const Model& programState::model( void ) const{ return mainWindow_->modView.model(); };

gantryEdition& programState::gantryCreation( void ){ 
	return mainWindow_->gantryBuild; 
};



void programState::setUpdateInformationFlag( void ){
	if( mainWindow_ != nullptr )
		mainWindow_->tomographyExecution.updateInformation();
}



path programState::importSinogram( void ){
	storedImportChooser.SetAsLoaded();
	return  importChooserInstance.ChooseFile();
}