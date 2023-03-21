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
#include "cSysTree.h"



 /*********************************************************************
	Implementations
 *********************************************************************/


#ifdef WIN32
	const path programState::stateStorage{ ".\\stateStorage\\" };
#else
	const path programState::stateStorage{ "./stateStorage/" };
#endif // WIN32

const string slicePlane::FILE_PREAMBLE{ "VIEWPLANE_FILE_PREAMBLE" };



programState& PROGRAM_STATE( void ){
	return programState::getInstance();
}


programState& programState::getInstance(){
	static programState instance;
	return instance;
}


programState::programState( void ) :
	modelInstance{},
	storedModel{ getPath( "storedModel.model" ), modelInstance },

	modelChooserInstance{ "Choose CT model", "*.model", path{ "./" } },
	storedModelChooser{ getPath( "storedModelChooser.chooser" ), modelChooserInstance }

{

}


programState::~programState( void ) {
	
	createStorageDir();

	storedModel.saveObject();
	storedModelChooser.saveObject( true );
	
}


void programState::createStorageDir( void ){

	// Check if state storage directory erxists
	if( !std::filesystem::is_directory( stateStorage ) ) std::filesystem::create_directory( stateStorage );

}


bool programState::loadModel( void ){

	path modelToLoad = modelChooserInstance.choose();

	if( !storedModel.load( modelToLoad ) ) return false;

	return true;

}


