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
	storedModelChooser{ getPath( "storedModelChooser.txt" ), modelChooserInstance },

	planeInstance{},
	storedPlane{ programState::getPath( "storedViewPlane.txt" ), planeInstance },

	modelViewPara(),
	storedModelParameter( programState::getPath( "storedModelContrast.txt" ), modelViewPara ),

	xRayTubeParameter{},
	storedXRayTubeParameter{ programState::getPath( "storedTubeParameter.txt" ), xRayTubeParameter },

	radonParameter{},
	storedRadonParameter{ programState::getPath( "storedRadonParameter.txt" ), radonParameter },

	detectorParameter{},
	storedDetectorParameter{ programState::getPath( "storedDetectorParameter.txt" ), detectorParameter },

	gantryInstance{ CSYS_TREE().addCSys( "Gantry system"), xRayTubeParameter, radonParameter, detectorParameter },

	tomographyParamerter(),
	storedTomographyParamerter( programState::getPath( "storedTomograpyParameter.txt" ), tomographyParamerter ),

	currentProjections(),
	storedProjections( programState::getPath( "storedProjections.txt" ), currentProjections ),

	currentProcessingParameters(),
	storedProcessingParameters( programState::getPath( "storedProcessingParameters.txt" ), currentProcessingParameters ),

	currentFilteredProjections()

{

	createStorageDir();

}


programState::~programState( void ) {
	
	createStorageDir();

	storedModel.saveObject();
	storedModelChooser.saveObject( true );
	storedPlane.saveObject( true );
	storedModelParameter.saveObject( true );
	storedXRayTubeParameter.saveObject( true );
	storedRadonParameter.saveObject( true );
	storedDetectorParameter.saveObject( true );
	storedTomographyParamerter.saveObject( true );
	storedProjections.saveObject();
	storedProcessingParameters.saveObject( true );
}


void programState::createStorageDir( void ){

	// Check if state storage directory erxists
	if( !std::filesystem::is_directory( stateStorage ) ) std::filesystem::create_directory( stateStorage );

}

void programState::buildGantry( const tubeParameter tubeParameter_,
				  const detectorRadonParameter radonParameter_, const detectorIndipendentParameter indipendentParameter ){

	xRayTubeParameter = tubeParameter_;
	radonParameter = radonParameter_;
	detectorParameter = indipendentParameter;

	gantryInstance.reset();
	gantry newGantry{ gantryInstance.CSys(), tubeParameter_, radonParameter, indipendentParameter };

	gantryInstance = newGantry;

};

string programState::modelDescription( void ) const{

	string modelDataString;

	modelDataString.clear();
	modelDataString += "Name: \t" + modelInstance.Name() + '\n';
	modelDataString += "Voxel: \t\t\t" + toString( modelInstance.NumVox().x ) + " x " + toString( modelInstance.NumVox().y ) + " x " + toString( modelInstance.NumVox().z ) + "\n";
	modelDataString += "Voxel Größe: \t" + toString( modelInstance.VoxSize().x, 2 ) + " x " + toString( modelInstance.VoxSize().y, 2 ) + " x " + toString( modelInstance.VoxSize().z, 2 ) + "  mm^3\n";
	modelDataString += "Model Größe: \t" + toString( modelInstance.ModSize().x ) + " x " + toString( modelInstance.ModSize().y ) + " x " + toString( modelInstance.ModSize().z ) + "  mm^3";

	return modelDataString;
}

void programState::moveModel( const double targetXRot, const double targetYRot, const double targetZTrans ){


	if( targetXRot != planeInstance.rotationAngleX ){

		const double rotationAngle = targetXRot - planeInstance.rotationAngleX;
		planeInstance.rotationAngleX = targetXRot;

		const line axis{ planeInstance.surface.R1(), planeInstance.surface.O() };

		modelInstance.CSys()->rotateM( axis, rotationAngle / 360. * 2. * PI );
	}

	if( targetYRot != planeInstance.rotationAngleY ){

		const double rotationAngle = targetYRot - planeInstance.rotationAngleY;
		planeInstance.rotationAngleY = targetYRot;

		const line axis{ planeInstance.surface.R2(), planeInstance.surface.O() };

		modelInstance.CSys()->rotateM( axis, rotationAngle / 360. * 2. * PI );
	}

	if( targetZTrans != planeInstance.positionZ ){

		const double translation = targetZTrans - planeInstance.positionZ;
		planeInstance.positionZ = targetZTrans;

		modelInstance.CSys()->translateM( ( (vec3) planeInstance.surface.Normal() ) * translation );
	}


	sliceModel();

}

void programState::sliceModel( void ){

	modelSliceInstance = modelInstance.getSlice( planeInstance.surface, 1. );

}

void programState::centerModel( void ){

	// Center model
	v3 center = primitiveVec3{ modelInstance.ModSize() } / -2.;

	modelInstance.CSys()->setPrimitive( primitiveCartCSys{ center, v3{1,0,0}, v3{0,1,0}, v3{0,0,1} } );


}

void programState::resetModel( void ){

	// Reset plane
	planeInstance.rotationAngleX = 0.;
	planeInstance.rotationAngleY = 0.;
	planeInstance.positionZ = 0.;

	centerModel();


}

bool programState::loadModel( void ){

	path modelToLoad = modelChooserInstance.choose();

	if( !storedModel.load( modelToLoad ) ) return false;

	return true;

}


