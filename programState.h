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


	public:

	static programState& getInstance();

	~programState( void );


	bool ModelLoaded( void ) const{ return storedModel.Loaded(); };

	bool loadModel( void );

	const model& Model( void ){ return modelInstance; };

	const gantry& Gantry( void ){ return gantryInstance; };

	grid<voxData>& Slice( void ){ return modelSliceInstance; };

	static void createStorageDir( void );

	static path getPath( const string filename ){ return stateStorage / filename; };

	void buildGantry( const tubeParameter tubeParameter_,
					  const detectorRadonParameter radonParameter, const detectorIndipendentParameter indipendentParameter );

	string modelDescription( void ) const;

	bool moveModel( double& targetXRot, double& targetYRot, double& targetZTrans );

	bool sliceModel( void );

	void centerModel( void );

	void resetModel( void );

	slicePlane& Plane( void ) { return planeInstance; };


	const tubeParameter& TubeParameter( void ) const{ return xRayTubeParameter; };

	const detectorRadonParameter& RadonParameter( void ) const{ return radonParameter; };

	const detectorIndipendentParameter& DetectorParameter( void ) const{ return detectorParameter; };

	modelViewParameter& ModelViewParameter( void ){ return modelViewPara; };

	tomography& Tomography( void ) { return tomographyInstance; };

	tomographyParameter& TomographyParameter( void ){ return tomographyParamerter; };

	radonTransformed& Projections( void ){ return currentProjections; };

	filteredProjections& FilteredProjections( void ){ return currentFilteredProjections; };

	reconstrucedImage& ReconstrucedImage( void ){ return currentReconstrucedImage; };

	bool RadonTransformedLoaded( void ) const{ return storedProjections.Loaded();  };

	void assignRadonTransformed( const radonTransformed rt ){ currentProjections = rt; storedProjections.setLoaded(); };

	//void setNewRadonTransformedFlag( void ){  }

	void registerMainWindow( mainView* const ptr ) { mainWindow = ptr; };

	void registerProcessingWindow( processingView* const ptr ){ processingWindow = ptr; };

	mainView* MainWindow( void ) const{ return mainWindow; };

	processingView* ProcessingWindow( void ) const{ return processingWindow; };

	processingParameter& ProcessingParameters( void ){ return currentProcessingParameters; };


	private:

	programState( void );

	programState( const programState& ) = delete;

	programState& operator=( const programState& ) = delete;


	private:

	mainView* mainWindow = nullptr;
	processingView* processingWindow = nullptr;

	model modelInstance;
	storedObject<model> storedModel;

	fileChooser modelChooserInstance;
	storedObject<fileChooser> storedModelChooser;

	slicePlane planeInstance;
	storedObject<slicePlane> storedPlane;

	grid<voxData> modelSliceInstance;
	
	modelViewParameter modelViewPara;
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

	tomographyParameter tomographyParamerter;
	storedObject<tomographyParameter> storedTomographyParamerter;

	tomography tomographyInstance;

	radonTransformed currentProjections;
	storedObject<radonTransformed> storedProjections;

	processingParameter currentProcessingParameters;
	storedObject<processingParameter> storedProcessingParameters;

	filteredProjections currentFilteredProjections;

	reconstrucedImage currentReconstrucedImage;



};

