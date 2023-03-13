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



 /*********************************************************************
	Implementations
 *********************************************************************/


programState& PROGRAM_STATE( void ){
	return programState::getInstance();
}


programState& programState::getInstance(){
	static programState instance;
	return instance;
};


programState::programState( void ) :
	storedModelInstance{},
	storedModel{ getPath( "storedModel.model" ), storedModelInstance },

	storedModelChooserInstance{ "Choose CT model", "*.model", path{ "./" } },
	storedModelChooser{ getPath( "storedModelChooser.chooser" ), storedModelChooserInstance },

	modelSliceInstance{},
	storedModelSlice{ getPath( "modelSliceImage.image" ), modelSliceInstance }

{

}


void programState::saveState( void ) const{
	
	// Check if state storage directory erxists
	if( !std::filesystem::is_directory( stateStorage ) ) std::filesystem::create_directory( stateStorage );

	storedModel.saveObject();
	storedModelChooser.saveObject();
	storedModelSlice.saveObject();
}


void programState::loadModel( void ){

	path modelToLoad = storedModelChooserInstance.choose();

	storedModel.load( modelToLoad );

};