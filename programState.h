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

	static const path storage_path_;		/*!< Path to persisting storage location*/
	
	/*!
	 * @brief Get reference of singlular instance
	 * @return Reference to program state
	*/
	static ProgramState& GetInstance();
	
	/*!
	 * @brief Destructor
	 * @details Stores objects in persisting storage
	*/
	~ProgramState( void );
	
	/*!
	 * @brief Get flag for state reset at exit
	 * @return True when state will be reset at exit
	*/
	bool reset_state_at_exit( void ) const{ return reset_state_at_exit_; };

	/*!
	 * @brief Set flag to reset program state at program exit
	*/
	void SetResetStateAtExit( void ){ reset_state_at_exit_ = true; };
	
	/*!
	 * @brief Get persitent storage path of file with given name
	 * @param filename Name of file
	 * @return Absoulte path of file
	*/
	path GetAbsolutePath( const string filename ) const{ return storage_path_ / filename; };


	private:

	bool reset_state_at_exit_;			/*!< Flag indicating whether to reset the program state at program exit_*/


	/*!
	 * @brief Constructor
	*/
	ProgramState( void );

	/*!
	 * @brief Copy constructor 
	 * @details Deleted for singleton pattern
	*/
	ProgramState( const ProgramState& ) = delete;

	/*!
	 * @brief Copy assignment 
	 * @details Deleted for singleton pattern
	*/
	ProgramState& operator=( const ProgramState& ) = delete;

	/*!
	 * @brief Create the storage directory
	*/
	void CreateStorageDirectory( void );

	/*!
	 * @brief Delete storage directory
	*/
	void DeleteStorageDirectory( void );


};

