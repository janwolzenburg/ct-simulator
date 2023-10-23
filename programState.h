#pragma once
/*********************************************************************
 * @file   programState.h
 * @brief  
 *
 * @author Jan Wolzenburg
 * @date   March 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

 #include <string>
 using std::string;

 #include <vector>
 using std::vector;

 #include "programState.fwd.h"

 #include "model.h"
 #include "gantry.h"

 #include "fileChooser.h"
 #include "slicePlane.h"
 #include "storedObject.h"
 #include "monoImage.h"
 #include "tomography.h"
 #include "backprojection.h"
 #include "mainWindow.fwd.h"
 #include "processingWindow.fwd.h"
 #include "processingParameters.h"
 #include "modelViewParameter.h"



 /*********************************************************************
	Definitions
 *********************************************************************/


 programState& PROGRAM_STATE( void );


class programState{

	public:

	static const path stateStorage;
	static programState& getInstance();
	

	slicePlane planeInstance;
	grid<voxData> modelSliceInstance;
	modelViewParameter modelViewPara;
	tomography tomographyInstance;
	tomographyParameter tomographyParamerters;
	radonTransformed currentProjections;
	filteredProjections currentFilteredProjections;
	reconstrucedImage currentReconstrucedImage;
	processingParameter currentProcessingParameters;

	
	~programState( void );


	/*********************************************** Storage ***************************************/
	/***********************************************************************************************/


	void createStorageDir( void );

	void deleteStorageDir( void );

	void resetStateStorageAtExit( void ){ resetStateAtExit = true; };
	
	path getPath( const string filename ){ return stateStorage / filename; };


	/*********************************************** GUI *******************************************/
	/***********************************************************************************************/

	void registerMainWindow( mainView* const ptr ) { mainWindow = ptr; };

	void registerProcessingWindow( processingView* const ptr ){ processingWindow = ptr; };

	mainView* MainWindow( void ) const{ return mainWindow; };

	processingView* ProcessingWindow( void ) const{ return processingWindow; };
			
	void activateAll( void );

	void deactivateAll( void );


	
	const model& Model( void ){ return modelInstance; };

	bool ModelLoaded( void ) const{ return storedModel.Loaded(); };

	bool loadModel( void );

	string modelDescription( void ) const;

	bool moveModel( double& targetXRot, double& targetYRot, double& targetZTrans );

	bool sliceModel( void );

	void centerModel( void );

	void resetModel( void );


	
	const gantry& Gantry( void ){ return gantryInstance; };

	void buildGantry( const tubeParameter tubeParameter_,
					  const detectorRadonParameter radonParameter, const detectorIndipendentParameter indipendentParameter );


					  
	const tube& Tube( void ) const{ return gantryInstance.Tube(); };

	const tubeParameter& TubeParameter( void ) const{ return xRayTubeParameter; };

	const detectorRadonParameter& RadonParameter( void ) const{ return radonParameter; };

	const detectorIndipendentParameter& DetectorParameter( void ) const{ return detectorParameter; };

	const detectorPhysicalParameter DetectorPhysicalParameter( void ) const{ return gantryInstance.Detector().getPhysicalParameters(); };



	bool RadonTransformedLoaded( void ) const{ return storedProjections.Loaded();  };

	void assignRadonTransformed( const radonTransformed rt ){ currentProjections = rt; storedProjections.setLoaded(); storedProcessingParameters.setLoaded(); };
	
	void setUpdateInformationFlag( void ) const;

	void exportSinogram( void );

	path importSinogram( void );







	private:

	programState( void );

	programState( const programState& ) = delete;

	programState& operator=( const programState& ) = delete;



	mainView* mainWindow = nullptr;
	processingView* processingWindow = nullptr;

	bool resetStateAtExit;

	model modelInstance;
	storedObject<model> storedModel;

	fileChooser modelChooserInstance;
	storedObject<fileChooser> storedModelChooser;

	storedObject<slicePlane> storedPlane;

	storedObject<modelViewParameter> storedModelParameter;


	// Voltage, Current, material
	tubeParameter xRayTubeParameter;
	storedObject<tubeParameter> storedXRayTubeParameter;

	// Numberpoints (2D), Distance range
	detectorRadonParameter radonParameter;
	storedObject<detectorRadonParameter> storedRadonParameter;

	// Pixel arc radius, pixel size col direction, Structere, structure max angle
	detectorIndipendentParameter detectorParameter;
	storedObject<detectorIndipendentParameter> storedDetectorParameter;

	gantry gantryInstance;

	storedObject<tomographyParameter> storedTomographyParamerter;

	fileChooser exportChooserInstance;
	storedObject<fileChooser> storedExportChooser;

	fileChooser importChooserInstance;
	storedObject<fileChooser> storedImportChooser;

	storedObject<radonTransformed> storedProjections;

	storedObject<processingParameter> storedProcessingParameters;


};

