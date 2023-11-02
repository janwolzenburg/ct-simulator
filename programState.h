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

 //#include "programState.fwd.h"
 #include "generel.h"
 #include "dataGrid.h"
 #include "voxel.h"
 #include "tomography.h"
 #include "projections.h"
 #include "processingParameters.h"
 #include "filteredProjections.h"
 #include "mainWindow.fwd.h"
 #include "processingWindow.fwd.h"
 #include "model.h"
 #include "persistingObject.h"
 #include "fileChooser.h"
 #include "xRayTube.h"
 #include "detectorProperties.h"
 #include "gantry.h"
 #include "reconstructedImage.h"


 /*********************************************************************
	Definitions
 *********************************************************************/


class programState& PROGRAM_STATE( void );


class programState{

	public:

	static const path stateStorage;		/*!< Path to persisting storage location*/
	
	/*!
	 * @brief Get reference of singlular instance
	 * @return Reference to program state
	*/
	static programState& getInstance();
	

	Tomography tomographyInstance;				/*!<Instance of the tomography*/
	TomographyProperties tomographyParamerters;	/*!<Parameter of tomography*/
	Projections currentProjections;		/*!<The current projections from last tomography*/

	processingParameter currentProcessingParameters;	/*!<Current processing parameters*/
	FilteredProjections currentFilteredProjections;		/*!<Current filtered projections*/
	ReconstrucedImage currentReconstrucedImage;			/*!<Current image reconstructed from filtered projections*/
	
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
	 * @brief Set flag to reset program state at program exit_
	*/
	void resetStateStorageAtExit( void ){ resetStateAtExit = true; };
	
	/*!
	 * @brief Get persitent storage path of file with given name_
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
	
	const Model& model( void ) const;


	/********************************************* Gantry ******************************************/
	/***********************************************************************************************/
	
	/*!
	 * @brief Get reference to tube instance
	 * @return Constant reference to tube instance
	*/
	XRayTube Tube( void ) const{ return gantryInstance.tube(); };

	/*!
	 * @brief Get reference to tube parameter
	 * @return Constant reference to tube parameter
	*/
	XRayTubeProperties TubeParameter( void ) const{ return xRayTubeParameter; };

	/*!
	 * @brief Get reference to radon parameter
	 * @return Constant reference to radon parameter
	*/
	ProjectionsProperties RadonParameter( void ) const{ return radonParameter; };

	/*!
	 * @brief Get reference to detector parameter
	 * @return Constant reference to detector parameter
	*/
	PhysicalDetectorProperties DetectorParameter( void ) const{ return physical_detector_properties_; };

	/*!
	 * @brief Get physical detector parameter
	 * @return Physical detector parameter
	*/
	DetectorProperties DetectorPhysicalParameter( void ) const{ return gantryInstance.detector().properties(); };
		
	/*!
	 * @brief Get reference to gantry
	 * @return Constant reference to gantry
	*/
	Gantry gantry( void ){ return gantryInstance; };

	/*!
	 * @brief Build gantry and store given parameter
	 * @param tubeParameter_ Tube parameter
	 * @param radonParameter Radon parameter for detector
	 * @param indipendentParameter Detector parameter
	*/
	void buildGantry( const XRayTubeProperties tubeParameter_,
					  const ProjectionsProperties radonParameter, const PhysicalDetectorProperties indipendentParameter );


	/***************************************** Tomography ******************************************/
	/***********************************************************************************************/

	/*!
	 * @brief Check if a radon transformed is loaded
	 * @return True when a radon transform is currently loaded
	*/
	bool RadonTransformedLoaded( void ) const{ return storedProjections.was_loaded();  };

	/*!
	 * @brief Assign a radon transformed 
	 * @details Store as current projections and set flags
	 * @param rt Radon transform to assign
	*/
	void assignRadonTransformed( const Projections rt );
	
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


	bool ProcessingParameterLoaed( void ) const{ return storedProcessingParameters.was_loaded();  };
	void ProcessingParameterSetLoaed( void ) { storedProcessingParameters.SetAsLoaded(); };


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


	bool resetStateAtExit;									/*!<Flag indicating whether to reset the program state at program exit_*/


	XRayTubeProperties xRayTubeParameter;									/*!<xRay tube attributes*/
	PersistingObject<XRayTubeProperties> storedXRayTubeParameter;				/*!<Persisting storage of tube attributes*/
	ProjectionsProperties radonParameter;								/*!<Parameter in radon space affecting the detector*/
	PersistingObject<ProjectionsProperties> storedRadonParameter;			/*!<Persisting storage of radon parameter*/
	PhysicalDetectorProperties physical_detector_properties_;						/*!<Parameter only dependent on the physical properties_ od detector*/
	PersistingObject<PhysicalDetectorProperties> storedDetectorParameter;	/*!<Persisting storage of the detector parameter*/
	Gantry gantryInstance;												/*!<Instance of the gantry constructed from tube and detector parameter*/

	PersistingObject<TomographyProperties> storedTomographyParamerter;	/*!<Persisting storage of the tomography parameter*/
	PersistingObject<Projections> storedProjections;				/*!<Persisting storage of projections*/
	PersistingObject<processingParameter> storedProcessingParameters;	/*!<Persisting storage of processing parameter*/

	FileChooser exportChooserInstance;				/*!<File chooser for sinogram export*/
	PersistingObject<FileChooser> storedExportChooser;	/*!<Persisting storage of sinogram export file selection*/

	FileChooser importChooserInstance;				/*!<File chooser for sinogram import*/
	PersistingObject<FileChooser> storedImportChooser;	/*!<Persisting storage of sinogram import file selection*/



};

