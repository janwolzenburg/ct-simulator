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
	

	tomographyInstance{},
	tomographyParamerters{},
	currentProjections{},
	currentProcessingParameters{},
	currentFilteredProjections{},
	currentReconstrucedImage{},
	mainWindow_( nullptr ),
	processingWindow_( nullptr ), 

	resetStateAtExit( false ),

	xRayTubeParameter{},
	storedXRayTubeParameter{ programState::getPath( "storedTubeParameter.txt" ), xRayTubeParameter },
	radonParameter{},
	storedRadonParameter{ programState::getPath( "storedRadonParameter.txt" ), radonParameter },
	physical_detector_properties_{},
	storedDetectorParameter{ programState::getPath( "storedDetectorParameter.txt" ), physical_detector_properties_ },
	gantryInstance{ CoordinateSystems().AddSystem( "Gantry system"), xRayTubeParameter, radonParameter, physical_detector_properties_ },
	
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
	



	storedXRayTubeParameter.Save();
	storedRadonParameter.Save();
	storedDetectorParameter.Save();

	storedTomographyParamerter.Save();
	storedProjections.Save();
	storedProcessingParameters.Save( );
	storedImportChooser.Save();
	storedExportChooser.Save();
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


const Model& programState::model( void ) const{ return mainWindow_->modView.model(); };

void programState::buildGantry( const XRayTubeProperties tubeParameter_,
				  const ProjectionsProperties radonParameter_, const PhysicalDetectorProperties indipendentParameter ){

	xRayTubeParameter = tubeParameter_;
	radonParameter = radonParameter_;
	physical_detector_properties_ = indipendentParameter;

	gantryInstance.UpdateTubeAndDetectorProperties( xRayTubeParameter, radonParameter, physical_detector_properties_ );

	storedXRayTubeParameter.SetAsLoaded();
	storedRadonParameter.SetAsLoaded();
	storedDetectorParameter.SetAsLoaded();
}


void programState::assignRadonTransformed( const Projections rt ){ 
	currentProjections = rt; 
	storedProjections.SetAsLoaded(); 
	storedProcessingParameters.SetAsLoaded(); 
	if( processingWindow_ != nullptr ){
		processingWindow_->setNewRTFlag();
	}
}

void programState::setUpdateInformationFlag( void ){
	if( mainWindow_ != nullptr )
		mainWindow_->tomographyExecution.updateInformation();
}

void programState::exportSinogram( void ){
	if( storedProjections.was_loaded() ){

		path exportPath = exportChooserInstance.choose();
		storedExportChooser.SetAsLoaded();
		if( exportPath.empty() ) return;

		if( exportPath.extension() != ".sinogram" )
			exportPath += ".sinogram";

		vector<char> binary_data;
		currentProjections.Serialize( binary_data );

		ExportSerialized( exportPath, binary_data );

	}
}

path programState::importSinogram( void ){
	storedImportChooser.SetAsLoaded();
	return  importChooserInstance.choose();
}