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

 #include "generel.h"


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

	/*!
	 * @brief Set flag to reset program state at program exit_
	*/
	void resetStateStorageAtExit( void ){ resetStateAtExit = true; };
	
	bool ResetStateAtExit( void ) const{ return resetStateAtExit; };

	/*!
	 * @brief Get persitent storage path of file with given name_
	 * @param filename Name of file
	 * @return Absoulte path of file
	*/
	path getPath( const string filename ) const{ return stateStorage / filename; };



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


	bool resetStateAtExit;									/*!< Flag indicating whether to reset the program state at program exit_*/

};

