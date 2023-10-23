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

	static const path stateStorage;		/*!< Path to persisting storage location*/
	
	/*!
	 * @brief Get reference of singlular instance
	 * @return Reference to program state
	*/
	static programState& getInstance();
	

	slicePlane planeInstance;			/*!<Surface to slice model with*/
	grid<voxData> modelSliceInstance;	/*!<Slice through model as gridded data*/
	modelViewParameter modelViewPara;	/*!<Parameter of the model view*/

	tomography tomographyInstance;				/*!<Instance of the tomography*/
	tomographyParameter tomographyParamerters;	/*!<Parameter of tomography*/
	radonTransformed currentProjections;		/*!<The current projections from last tomography*/

	processingParameter currentProcessingParameters;	/*!<Current processing parameters*/
	filteredProjections currentFilteredProjections;		/*!<Current filtered projections*/
	reconstrucedImage currentReconstrucedImage;			/*!<Current image reconstructed from filtered projections*/
	
	mainView* mainWindow = nullptr;					/*!<Pointer to the main window*/
	processingView* processingWindow = nullptr;		/*!<Pointer to the processing window*/


	/*!
	 * @brief Destructor
	 * @details Stores objects in persisting storage
	*/
	~programState( void );


	/*********************************************** Storage ***************************************/
	/***********************************************************************************************/

	/*!
	 * @brief Set flag to reset program state at program exit
	*/
	void resetStateStorageAtExit( void ){ resetStateAtExit = true; };
	
	/*!
	 * @brief Get persitent storage path of file with given name
	 * @param filename Name of file
	 * @return Absoulte path of file
	*/
	path getPath( const string filename ) const{ return stateStorage / filename; };


	/*********************************************** GUI *******************************************/
	/***********************************************************************************************/

	/*!
	 * @brief Store pointer to main window for usage in program
	 * @param ptr Pointer to the main window
	*/
	void registerMainWindow( mainView* const ptr ){ mainWindow = ptr; };

	/*!
	 * @brief Store pointer to processing window for usage in program
	 * @param ptr Pointer to the processing window
	*/
	void registerProcessingWindow( processingView* const ptr ){ processingWindow = ptr; };
	
	/*!
	 * @brief Activate main and processing window
	*/
	void activateAll( void );

	/*!
	 * @brief Deactivate main and processing window
	*/
	void deactivateAll( void );


	/********************************************* Model *******************************************/
	/***********************************************************************************************/
	
	/*!
	 * @brief Get 
	 * @param  
	 * @return 
	*/
	const model& Model( void ){ return modelInstance; };

	bool ModelLoaded( void ) const{ return storedModel.Loaded(); };

	bool loadModel( void );

	string modelDescription( void ) const;

	bool moveModel( double& targetXRot, double& targetYRot, double& targetZTrans );

	bool sliceModel( void );

	void centerModel( void );

	void resetModel( void );


	/********************************************* Gantry ******************************************/
	/***********************************************************************************************/
					  
	const tube& Tube( void ) const{ return gantryInstance.Tube(); };

	const tubeParameter& TubeParameter( void ) const{ return xRayTubeParameter; };

	const detectorRadonParameter& RadonParameter( void ) const{ return radonParameter; };

	const detectorIndipendentParameter& DetectorParameter( void ) const{ return detectorParameter; };

	const detectorPhysicalParameter DetectorPhysicalParameter( void ) const{ return gantryInstance.Detector().getPhysicalParameters(); };
		
	const gantry& Gantry( void ){ return gantryInstance; };

	void buildGantry( const tubeParameter tubeParameter_,
					  const detectorRadonParameter radonParameter, const detectorIndipendentParameter indipendentParameter );


	/***************************************** Tomography ******************************************/
	/***********************************************************************************************/

	bool RadonTransformedLoaded( void ) const{ return storedProjections.Loaded();  };

	void assignRadonTransformed( const radonTransformed rt ){ currentProjections = rt; storedProjections.setLoaded(); storedProcessingParameters.setLoaded(); };
	
	void setUpdateInformationFlag( void ) const;

	void exportSinogram( void );

	path importSinogram( void );


	private:

	/*!
	 * @brief Constructor
	*/
	programState( void );

	/*!
	 * @brief Copy constructor 
	 * @details Deleted for singleton pattern
	*/
	programState( const programState& ) = delete;

	/*!
	 * @brief Copy assignment 
	 * @details Deleted for singleton pattern
	*/
	programState& operator=( const programState& ) = delete;

	/*!
	 * @brief Create the storage directory
	*/
	void createStorageDir( void );

	/*!
	 * @brief Delete storage directory
	*/
	void deleteStorageDir( void );


	bool resetStateAtExit;							/*!<Flag indicating whether to reset the program state at program exit*/

	model modelInstance;									/*!<Current model*/
	storedObject<model> storedModel;						/*!<Persisting storage of current model*/
	fileChooser modelChooserInstance;						/*!<File chooser for the model*/
	storedObject<fileChooser> storedModelChooser;			/*!<Persisting storage of model chooser*/
	storedObject<modelViewParameter> storedModelParameter;	/*!<Persisting storage of view parameters*/
	storedObject<slicePlane> storedPlane;					/*!<Persisting storage of the slice plane*/

	tubeParameter xRayTubeParameter;									/*!<xRay tube attributes*/
	storedObject<tubeParameter> storedXRayTubeParameter;				/*!<Persisting storage of tube attributes*/
	detectorRadonParameter radonParameter;								/*!<Parameter in radon space affecting the detector*/
	storedObject<detectorRadonParameter> storedRadonParameter;			/*!<Persisting storage of radon parameter*/
	detectorIndipendentParameter detectorParameter;						/*!<Parameter only dependent on the physical properties od detector*/
	storedObject<detectorIndipendentParameter> storedDetectorParameter;	/*!<Persisting storage of the detector parameter*/
	gantry gantryInstance;												/*!<Instance of the gantry constructed from tube and detector parameter*/

	storedObject<tomographyParameter> storedTomographyParamerter;	/*!<Persisting storage of the tomography parameter*/
	storedObject<radonTransformed> storedProjections;				/*!<Persisting storage of projections*/
	storedObject<processingParameter> storedProcessingParameters;	/*!<Persisting storage of processing parameter*/

	fileChooser exportChooserInstance;				/*!<File chooser for sinogram export*/
	storedObject<fileChooser> storedExportChooser;	/*!<Persisting storage of sinogram export file selection*/

	fileChooser importChooserInstance;				/*!<File chooser for sinogram import*/
	storedObject<fileChooser> storedImportChooser;	/*!<Persisting storage of sinogram import file selection*/



};

