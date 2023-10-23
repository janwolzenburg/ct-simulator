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
#include "mainWindow.h"
#include "processingWindow.h"


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
	
	resetStateAtExit( false ),

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

	importChooserInstance{ "Import Sinogram", "*.sinogram", path{ "./" } },
	storedImportChooser{ getPath( "storedImportChooser.txt" ), importChooserInstance },

	exportChooserInstance{ "Export Sinogram", "*.sinogram", path{ "./" }, Fl_Native_File_Chooser::Type::BROWSE_SAVE_FILE },
	storedExportChooser{ getPath( "storedExportChooser.txt" ), exportChooserInstance },

	tomographyParamerters{},
	storedTomographyParamerter{ programState::getPath( "storedTomograpyParameter.txt" ), tomographyParamerters },

	currentProjections{},
	storedProjections{ programState::getPath( "storedProjections.txt" ), currentProjections },

	currentProcessingParameters{},
	storedProcessingParameters{ programState::getPath( "storedProcessingParameters.txt" ), currentProcessingParameters },

	currentFilteredProjections{}

{

	createStorageDir();

}


programState::~programState( void ) {
	
	if( resetStateAtExit ){
		
		deleteStorageDir();

		return;
	}
	createStorageDir();

	storedModel.saveObject();
	storedModelChooser.saveObject();
	storedPlane.saveObject();
	storedModelParameter.saveObject();

	storedXRayTubeParameter.saveObject();
	storedRadonParameter.saveObject();
	storedDetectorParameter.saveObject();

	storedTomographyParamerter.saveObject();
	storedProjections.saveObject();
	storedProcessingParameters.saveObject( );
	storedImportChooser.saveObject();
	storedExportChooser.saveObject();
}

void programState::activateAll( void ){
	mainWindow->activate();
	processingWindow->activate();
}


void programState::deactivateAll( void ){
	mainWindow->deactivate();
	processingWindow->deactivate();
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

void programState::buildGantry( const tubeParameter tubeParameter_,
				  const detectorRadonParameter radonParameter_, const detectorIndipendentParameter indipendentParameter ){

	xRayTubeParameter = tubeParameter_;
	radonParameter = radonParameter_;
	detectorParameter = indipendentParameter;

	gantryInstance.reset();
	gantry newGantry{ gantryInstance.CSys(), tubeParameter_, radonParameter, indipendentParameter };

	storedXRayTubeParameter.setLoaded();
	storedRadonParameter.setLoaded();
	storedDetectorParameter.setLoaded();
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

bool programState::moveModel( double& targetXRot, double& targetYRot, double& targetZTrans ){

	const slicePlane backupPlane = planeInstance; 
	const primitiveCartCSys backupCSys = modelInstance.CSys()->getPrimitive();

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


	// Return if succeeded
	if( sliceModel() ) return true;
	

	// Revert changes
	planeInstance = backupPlane;
	modelInstance.CSys()->setPrimitive( backupCSys );

	targetXRot = planeInstance.rotationAngleX;
	targetYRot = planeInstance.rotationAngleY;
	targetZTrans = planeInstance.positionZ;


	return false;

}

bool programState::sliceModel( void ){

	storedPlane.setLoaded();
	storedModelParameter.setLoaded();
	storedTomographyParamerter.setLoaded();

	grid<voxData> tempSlice = modelInstance.getSlice( planeInstance.surface, 1. );
	
	if( tempSlice.Size().col == 0 || tempSlice.Size().row == 0 )
		return false;

	modelSliceInstance = tempSlice;

	return true;

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


void programState::setUpdateInformationFlag( void ) const{
	if( mainWindow != nullptr )
		mainWindow->tomographyExecution.updateInformation();
}

bool programState::loadModel( void ){

	path modelToLoad = modelChooserInstance.choose();
	storedModelChooser.setLoaded();

	if( !storedModel.load( modelToLoad ) ) return false;


	return true;

}



void programState::exportSinogram( void ){
	if( storedProjections.Loaded() ){

		path exportPath = exportChooserInstance.choose();
		storedExportChooser.setLoaded();
		if( exportPath.empty() ) return;


		if( exportPath.extension() != "sinogram" )
			exportPath += ".sinogram";

		vector<char> binData;
		currentProjections.serialize( binData );

		exportSerialized( exportPath, binData );

	}
}

path programState::importSinogram( void ){
	storedImportChooser.setLoaded();
	return  importChooserInstance.choose();

}