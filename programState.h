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
 #include "programState.fwd.h"


 /*********************************************************************
	Definitions
 *********************************************************************/


ProgramState& PROGRAM_STATE( void );


class ProgramState{

	public:

	const path storage_path_;		/*!< path to persisting storage location*/
	
	/*!
	 * @brief get reference of singlular instance
	 * @return Reference to program state
	*/
	static ProgramState& GetInstance();
	
	/*!
	 * @brief destructor
	 * @details Stores objects in persisting storage
	*/
	~ProgramState( void );
	
	/*!
	 * @brief get flag for state reset at exit
	 * @return true when state will be reset at exit
	*/
	bool reset_state_at_exit( void ) const{ return reset_state_at_exit_; };

	/*!
	 * @brief set flag to reset program state at program exit
	*/
	void SetResetStateAtExit( void ){ reset_state_at_exit_ = true; };
	
	/*!
	 * @brief get persitent storage path of file with given name
	 * @param filename Name of file
	 * @return absoulte path of file
	*/
	path GetAbsolutePath( const string filename ) const{ return storage_path_ / filename; };


	private:

	bool reset_state_at_exit_;			/*!< Flag indicating whether to reset the program state at program exit_*/


	/*!
	 * @brief constructor
	*/
	ProgramState( void );

	/*!
	 * @brief copy constructor 
	 * @details Deleted for singleton pattern
	*/
	ProgramState( const ProgramState& ) = delete;

	/*!
	 * @brief copy assignment 
	 * @details Deleted for singleton pattern
	*/
	ProgramState& operator=( const ProgramState& ) = delete;

	/*!
	 * @brief create the storage directory
	*/
	void CreateStorageDirectory( void );

	/*!
	 * @brief delete storage directory
	*/
	void DeleteStorageDirectory( void );


};