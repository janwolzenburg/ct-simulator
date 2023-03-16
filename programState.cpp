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
	storedModelChooser{ getPath( "storedModelChooser.chooser" ), modelChooserInstance },

	viewPlaneInstance{},
	storedViewPlane{ getPath( "storedViewPlane.sliceplane" ), viewPlaneInstance },

	modelSliceInstance{}

{

}


void programState::saveState( void ) const{
	
	// Check if state storage directory erxists
	if( !std::filesystem::is_directory( stateStorage ) ) std::filesystem::create_directory( stateStorage );

	storedModel.saveObject();
	storedModelChooser.saveObject( true );
	storedViewPlane.saveObject( true );
}


bool programState::loadModel( void ){

	path modelToLoad = modelChooserInstance.choose();

	return storedModel.load( modelToLoad );

}


bool programState::sliceModel( void ){

	if( !ModelLoaded() ) return false;

	grid modelSliceGrid = modelInstance.getSlice( viewPlaneInstance.surface, 1. );
	modelSliceInstance = greyImage{ modelSliceGrid };

	return true;
}