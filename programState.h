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
 #include "generel.h"
 #include "grid.h"
 #include "voxel.h"
 #include "modelViewParameter.h"
 #include "tomography.h"
 #include "radonTransform.h"
 #include "processingParameters.h"
 #include "backprojection.h"
 #include "mainWindow.fwd.h"
 #include "processingWindow.fwd.h"
 #include "model.h"
 #include "storedObject.h"
 #include "fileChooser.h"
 #include "tube.h"
 #include "detectorParameter.h"
 #include "gantry.h"



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
	
	grid<voxData> modelSliceInstance;	/*!<Slice through model as gridded data*/
	modelViewParameter modelViewPara;	/*!<Parameter of the model view*/

	tomography tomographyInstance;				/*!<Instance of the tomography*/
	tomographyParameter tomographyParamerters;	/*!<Parameter of tomography*/
	radonTransformed currentProjections;		/*!<The current projections from last tomography*/

	processingParameter currentProcessingParameters;	/*!<Current processing parameters*/
	filteredProjections currentFilteredProjections;		/*!<Current filtered projections*/
	reconstrucedImage currentReconstrucedImage;			/*!<Current image reconstructed from filtered projections*/
	
	mainWindow* mainWindow_;				/*!<Pointer to the main window*/
	processingWindow* processingWindow_;	/*!<Pointer to the processing window*/


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
	void registerMainWindow( mainWindow* const ptr ){ mainWindow_ = ptr; };

	/*!
	 * @brief Store pointer to processing window for usage in program
	 * @param ptr Pointer to the processing window
	*/
	void registerProcessingWindow( processingWindow* const ptr ){ processingWindow_ = ptr; };
	
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
	 * @brief Get Reference to model
	 * @return Constant reference to model
	*/
	const model& Model( void ){ return modelInstance; };

	/*!
	 * @brief Load the model a stored path
	 * @return True at success
	*/
	bool loadModel( void );

	/*!
	 * @brief Check if a model has been loaded
	 * @return True when a model has been loaded
	*/
	bool ModelLoaded( void ) const{ return storedModel.Loaded(); };

	/*!
	 * @brief Reset model to defautl state
	*/
	void resetModel( void );
	
	/*!
	 * @brief Move model to given values with respect to the slice plane coordinate system
	 * @param targetXRot Rotation around x-axis
	 * @param targetYRot Rotation around y-axis
	 * @param targetZTrans Translation in z-direction
	 * @return True at success
	*/
	bool moveModel( double& targetXRot, double& targetYRot, double& targetZTrans );
	
	/*!
	 * @brief Center the model
	*/
	void centerModel( void );
	
	/*!
	 * @brief Slice model with stored slice plane
	 * @return True at success
	*/
	bool sliceModel( void );

	/*!
	 * @brief Get the model description
	 * @return String with model information
	*/
	string modelDescription( void ) const;


	/********************************************* Gantry ******************************************/
	/***********************************************************************************************/
	
	/*!
	 * @brief Get reference to tube instance
	 * @return Constant reference to tube instance
	*/
	const tube& Tube( void ) const{ return gantryInstance.Tube(); };

	/*!
	 * @brief Get reference to tube parameter
	 * @return Constant reference to tube parameter
	*/
	const tubeParameter& TubeParameter( void ) const{ return xRayTubeParameter; };

	/*!
	 * @brief Get reference to radon parameter
	 * @return Constant reference to radon parameter
	*/
	const detectorRadonParameter& RadonParameter( void ) const{ return radonParameter; };

	/*!
	 * @brief Get reference to detector parameter
	 * @return Constant reference to detector parameter
	*/
	const detectorIndipendentParameter& DetectorParameter( void ) const{ return detectorParameter; };

	/*!
	 * @brief Get physical detector parameter
	 * @return Physical detector parameter
	*/
	const detectorPhysicalParameter DetectorPhysicalParameter( void ) const{ return gantryInstance.Detector().getPhysicalParameters(); };
		
	/*!
	 * @brief Get reference to gantry
	 * @return Constant reference to gantry
	*/
	const gantry& Gantry( void ){ return gantryInstance; };

	/*!
	 * @brief Build gantry and store given parameter
	 * @param tubeParameter_ Tube parameter
	 * @param radonParameter Radon parameter for detector
	 * @param indipendentParameter Detector parameter
	*/
	void buildGantry( const tubeParameter tubeParameter_,
					  const detectorRadonParameter radonParameter, const detectorIndipendentParameter indipendentParameter );


	/***************************************** Tomography ******************************************/
	/***********************************************************************************************/

	/*!
	 * @brief Check if a radon transformed is loaded
	 * @return True when a radon transform is currently loaded
	*/
	bool RadonTransformedLoaded( void ) const{ return storedProjections.Loaded();  };

	/*!
	 * @brief Assign a radon transformed 
	 * @details Store as current projections and set flags
	 * @param rt Radon transform to assign
	*/
	void assignRadonTransformed( const radonTransformed rt );
	
	/*!
	 * @brief Set flag to update tomography parameter information
	*/
	void setUpdateInformationFlag( void );

	/*!
	 * @brief Export current sinogram
	*/
	void exportSinogram( void );

	/*!
	 * @brief Choose path to import sinogram
	 * @return Path to chosen sinogram
	*/
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


	bool resetStateAtExit;									/*!<Flag indicating whether to reset the program state at program exit*/

	model modelInstance;									/*!<Current model*/
	storedObject<model> storedModel;						/*!<Persisting storage of current model*/
	fileChooser modelChooserInstance;						/*!<File chooser for the model*/
	storedObject<fileChooser> storedModelChooser;			/*!<Persisting storage of model chooser*/
	storedObject<modelViewParameter> storedViewParameter;	/*!<Persisting storage of view parameters*/

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

