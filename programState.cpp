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
	
	modelSliceInstance{},
	modelViewPara{},
	tomographyInstance{},
	tomographyParamerters{},
	currentProjections{},
	currentProcessingParameters{},
	currentFilteredProjections{},
	currentReconstrucedImage{},
	mainWindow_( nullptr ),
	processingWindow_( nullptr ), 

	resetStateAtExit( false ),

	modelInstance{},
	storedModel{ getPath( "storedModel.model" ), modelInstance },
	modelChooserInstance{ "Choose CT model", "*.model", path{ "./" } },
	storedModelChooser{ getPath( "storedModelChooser.txt" ), modelChooserInstance },
	storedViewParameter( programState::getPath( "storedViewParameter.txt" ), modelViewPara ),

	xRayTubeParameter{},
	storedXRayTubeParameter{ programState::getPath( "storedTubeParameter.txt" ), xRayTubeParameter },
	radonParameter{},
	storedRadonParameter{ programState::getPath( "storedRadonParameter.txt" ), radonParameter },
	detectorParameter{},
	storedDetectorParameter{ programState::getPath( "storedDetectorParameter.txt" ), detectorParameter },
	gantryInstance{ CoordinateSystems().AddSystem( "Gantry system"), xRayTubeParameter, radonParameter, detectorParameter },
	
	storedTomographyParamerter{ programState::getPath( "storedTomograpyParameter.txt" ), tomographyParamerters },
	storedProjections{ programState::getPath( "storedProjections.txt" ), currentProjections },
	storedProcessingParameters{ programState::getPath( "storedProcessingParameters.txt" ), currentProcessingParameters },
	
	importChooserInstance{ "Import Sinogram", "*.sinogram", path{ "./" } },
	storedImportChooser{ getPath( "storedImportChooser.txt" ), importChooserInstance },
	exportChooserInstance{ "Export Sinogram", "*.sinogram", path{ "./" }, Fl_Native_File_Chooser::Type::BROWSE_SAVE_FILE },
	storedExportChooser{ getPath( "storedExportChooser.txt" ), exportChooserInstance }

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
	storedViewParameter.saveObject();

	storedXRayTubeParameter.saveObject();
	storedRadonParameter.saveObject();
	storedDetectorParameter.saveObject();

	storedTomographyParamerter.saveObject();
	storedProjections.saveObject();
	storedProcessingParameters.saveObject( );
	storedImportChooser.saveObject();
	storedExportChooser.saveObject();
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

void programState::activateAll( void ){
	if( mainWindow_ != nullptr )
		mainWindow_->activate();

	if( processingWindow_ != nullptr )
		processingWindow_->activate();
}

void programState::deactivateAll( void ){
	if( mainWindow_ != nullptr )
		mainWindow_->deactivate();

	if( processingWindow_ != nullptr )
		processingWindow_->deactivate();
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
}

string programState::modelDescription( void ) const{

	string modelDataString;

	modelDataString.clear();
	modelDataString += "Name: \t" + modelInstance.Name() + '\n';
	modelDataString += "Voxel: \t\t\t" + ToString( modelInstance.NumVox().x ) + " x " + ToString( modelInstance.NumVox().y ) + " x " + ToString( modelInstance.NumVox().z ) + "\n";
	modelDataString += "Voxel Größe: \t" + ToString( modelInstance.VoxSize().x, 2 ) + " x " + ToString( modelInstance.VoxSize().y, 2 ) + " x " + ToString( modelInstance.VoxSize().z, 2 ) + "  mm^3\n";
	modelDataString += "Model Größe: \t" + ToString( modelInstance.ModSize().x ) + " x " + ToString( modelInstance.ModSize().y ) + " x " + ToString( modelInstance.ModSize().z ) + "  mm^3";

	return modelDataString;
}

bool programState::moveModel( double& targetXRot, double& targetYRot, double& targetZTrans ){

	const slicePlane backupPlane = modelViewPara.plane; 
	slicePlane& planeInstance =  modelViewPara.plane;

	const PrimitiveCoordinateSystem backupCSys = modelInstance.CSys()->GetPrimitive();

	if( targetXRot != planeInstance.rotationAngleX ){

		const double rotationAngle = targetXRot - planeInstance.rotationAngleX;
		planeInstance.rotationAngleX = targetXRot;

		const line axis{ planeInstance.surface.R1(), planeInstance.surface.O() };

		modelInstance.CSys()->Rotate( axis, rotationAngle / 360. * 2. * PI );
	}

	if( targetYRot != planeInstance.rotationAngleY ){

		const double rotationAngle = targetYRot - planeInstance.rotationAngleY;
		planeInstance.rotationAngleY = targetYRot;

		const line axis{ planeInstance.surface.R2(), planeInstance.surface.O() };

		modelInstance.CSys()->Rotate( axis, rotationAngle / 360. * 2. * PI );
	}

	if( targetZTrans != planeInstance.positionZ ){

		const double translation = targetZTrans - planeInstance.positionZ;
		planeInstance.positionZ = targetZTrans;

		modelInstance.CSys()->Translate( ( (Vector3D) planeInstance.surface.Normal() ) * translation );
	}

	// Return if succeeded
	if( sliceModel() ) return true;
	
	// Revert changes
	planeInstance = backupPlane;
	modelInstance.CSys()->SetPrimitive( backupCSys );

	targetXRot = planeInstance.rotationAngleX;
	targetYRot = planeInstance.rotationAngleY;
	targetZTrans = planeInstance.positionZ;


	return false;
}

bool programState::sliceModel( void ){

	storedViewParameter.setLoaded();
	storedTomographyParamerter.setLoaded();

	grid<voxData> tempSlice = modelInstance.getSlice(  modelViewPara.plane.surface, 1. );
	
	if( tempSlice.Size().c == 0 || tempSlice.Size().r == 0 )
		return false;

	modelSliceInstance = tempSlice;

	return true;
}

void programState::centerModel( void ){

	// Center model
	Tuple3D center = PrimitiveVector3{ modelInstance.ModSize() } / -2.;

	modelInstance.CSys()->SetPrimitive( PrimitiveCoordinateSystem{ center, Tuple3D{1,0,0}, Tuple3D{0,1,0}, Tuple3D{0,0,1} } );
}

void programState::resetModel( void ){

	// Reset plane
	 modelViewPara.plane.rotationAngleX = 0.;
	 modelViewPara.plane.rotationAngleY = 0.;
	 modelViewPara.plane.positionZ = 0.;

	centerModel();
}

bool programState::loadModel( void ){

	path modelToLoad = modelChooserInstance.choose();
	storedModelChooser.setLoaded();

	if( !storedModel.load( modelToLoad ) ) return false;

	return true;
}

void programState::assignRadonTransformed( const radonTransformed rt ){ 
	currentProjections = rt; 
	storedProjections.setLoaded(); 
	storedProcessingParameters.setLoaded(); 
	if( processingWindow_ != nullptr ){
		processingWindow_->setNewRTFlag();
	}
}

void programState::setUpdateInformationFlag( void ){
	if( mainWindow_ != nullptr )
		mainWindow_->tomographyExecution.updateInformation();
}

void programState::exportSinogram( void ){
	if( storedProjections.Loaded() ){

		path exportPath = exportChooserInstance.choose();
		storedExportChooser.setLoaded();
		if( exportPath.empty() ) return;

		if( exportPath.extension() != "sinogram" )
			exportPath += ".sinogram";

		vector<char> binary_data;
		currentProjections.Serialize( binary_data );

		ExportSerialized( exportPath, binary_data );

	}
}

path programState::importSinogram( void ){
	storedImportChooser.setLoaded();
	return  importChooserInstance.choose();
}