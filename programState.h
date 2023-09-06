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


	inline bool ModelLoaded( void ) const{ return storedModel.Loaded(); };

	bool loadModel( void );

	inline const model& Model( void ){ return modelInstance; };

	inline const gantry& Gantry( void ){ return gantryInstance; };

	inline rgbImage& Slice( void ){ return modelSliceInstance; };

	static void createStorageDir( void );

	static path getPath( const string filename ){ return stateStorage / filename; };

	void buildGantry( const tubeParameter tubeParameter_,
					  const detectorRadonParameter radonParameter, const detectorIndipendentParameter indipendentParameter );

	string modelDescription( void ) const;

	void moveModel( const double targetXRot, const double targetYRot, const double targetZTrans );

	void sliceModel( void );

	void centerModel( void );

	void resetModel( void );

	inline const slicePlane& Plane( void ) const{ return planeInstance; };

	inline const tubeParameter& TubeParameter( void ) const{ return xRayTubeParameter; };

	inline const detectorRadonParameter& RadonParameter( void ) const{ return radonParameter; };

	inline const detectorIndipendentParameter& DetectorParameter( void ) const{ return detectorParameter; };

	inline tomography& Tomography( void ) { return tomographyInstance; };

	inline tomographyParameter& TomographyParameter( void ){ return tomographyParamerter; };

	inline radonTransformed& Projections( void ){ return currentProjections; };

	inline filteredProjections& FilteredProjections( void ){ return currentFilteredProjections; };

	inline reconstrucedImage& ReconstrucedImage( void ){ return currentReconstrucedImage; };

	inline bool RadonTransformedLoaded( void ) const{ return storedProjections.Loaded();  };

	inline void assignRadonTransformed( const radonTransformed rt ){ currentProjections = rt; storedProjections.setLoaded(); };

	//inline void setNewRadonTransformedFlag( void ){  }

	inline void registerMainWindow( mainView* const ptr ) { mainWindow = ptr; };

	inline void registerProcessingWindow( processingView* const ptr ){ processingWindow = ptr; };

	inline mainView* MainWindow( void ) const{ return mainWindow; };

	inline processingView* ProcessingWindow( void ) const{ return processingWindow; };

	inline processingParameter& ProcessingParameters( void ){ return currentProcessingParameters; };


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


	rgbImage modelSliceInstance;



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

