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

 #include "generel.h"
 #include "dataGrid.h"
 #include "voxel.h"
 #include "tomography.h"
 #include "projections.h"
 #include "processingParameters.h"
 #include "filteredProjections.h"
 #include "mainWindow.fwd.h"
 #include "fl_ProcessingWindow.fwd.h"
 #include "model.h"
 #include "persistingObject.h"
 #include "fileChooser.h"
 #include "xRayTube.h"
 #include "detectorProperties.h"
 #include "gantry.h"
 #include "backprojection.h"


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

	


	//mainWindow* mainWindow_;				/*!< Pointer to the main window*/

	bool resetStateAtExit;									/*!< Flag indicating whether to reset the program state at program exit_*/





};

