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

const string valuatorStatus::FILE_PREAMBLE{ "VALUATORSTATUS_FILE_PREAMBLE" };


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

	modelSliceInstance{},

	valStatusInstance{},
	storedValStatus{ getPath("storedValuatorStatus.valStatus"), valStatusInstance}

{

}


void programState::saveState( void ) const{
	
	// Check if state storage directory erxists
	if( !std::filesystem::is_directory( stateStorage ) ) std::filesystem::create_directory( stateStorage );

	storedModel.saveObject();
	storedModelChooser.saveObject( true );
	storedViewPlane.saveObject( true );
	storedValStatus.saveObject( true );
}


bool programState::loadModel( void ){

	path modelToLoad = modelChooserInstance.choose();

	if( !storedModel.load( modelToLoad ) ) return false;

	centerModel();

	return true;

}


void programState::centerModel( void ){

	// Center model
	v3 center = primitiveVec3{ modelInstance.ModSize() } / -2.;

	modelInstance.CSys()->setPrimitive( primitiveCartCSys{ center, v3{1,0,0}, v3{0,1,0}, v3{0,0,1} } );

}


bool programState::sliceModel( void ){

	if( !ModelLoaded() ) return false;

	grid modelSliceGrid = modelInstance.getSlice( viewPlaneInstance.surface, 1. );
	modelSliceInstance = greyImage{ modelSliceGrid };

	return true;
}


bool programState::rotateViewX( const double angleDeg ){

	double rotationAngle = angleDeg - viewPlaneInstance.rotationAngleX;
	viewPlaneInstance.rotationAngleX = angleDeg;

	line axis = line{ viewPlaneInstance.surface.R1(), viewPlaneInstance.surface.O() };
	modelInstance.CSys()->rotateM( axis, rotationAngle / 360. * 2. * PI );

	return sliceModel();
}

bool programState::rotateViewY( const double angleDeg ){

	double rotationAngle = angleDeg - viewPlaneInstance.rotationAngleX;
	viewPlaneInstance.rotationAngleX = angleDeg;

	line axis = line{ viewPlaneInstance.surface.R2(), viewPlaneInstance.surface.O() };

	modelInstance.CSys()->rotateM( axis, rotationAngle / 360. * 2. * PI );
	return sliceModel();
}


valuatorStatus& programState::getValStatus( void ){
	return valStatusInstance;
}


path programState::getPath( const string filename ) const{

	return stateStorage / filename;

}