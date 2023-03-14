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


programState& PROGRAM_STATE( void ){
	return programState::getInstance();
}

cartCSys* viewSystem = CSYS_TREE().addCSys( "View system" );
const surfLim programState::viewPlane{	uvec3{ v3{ 1, 0, 0 }, viewSystem },
										uvec3{ v3{ 0, 1, 0 }, viewSystem },
										pnt3{  v3{0, 0, 0}, viewSystem },
										-300, 300, -300, 300
										};

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
	storedModelChooser.saveObject( true );
	storedModelSlice.saveObject();
}


bool programState::loadModel( void ){

	path modelToLoad = storedModelChooserInstance.choose();

	return storedModel.load( modelToLoad );

}


bool programState::sliceModel( void ){

	if( !ModelLoaded() ) return false;

	grid modelSliceGrid = storedModelInstance.getSlice( viewPlane, 1. );
	modelSliceInstance = greyImage{ modelSliceGrid };
	storedModelSlice.setLoaded();

	return true;
}