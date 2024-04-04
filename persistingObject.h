#pragma once
/*********************************************************************
 * @file   persistingObject.h
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

/*!
 * @brief class for a persisting object instance to be stored on Disc
 * @tparam C class of object
*/
template< class C >
class PersistingObject : public C{

	
	static_assert( std::is_pointer<decltype( &C::FILE_PREAMBLE )>::value );
	static_assert( std::is_member_function_pointer<decltype( &C::Serialize )>::value );

	public:

	/*!
	 * @brief constructor
	 * @details tries to load object from path
	 * @param standard standard instance
	 * @param file_path path to load from and save file
	 * @param deactivate_saving deactivate saving at destruction
	*/
	PersistingObject( const C&& standard, const path file_path, const bool deactivate_saving = false );

	/*!
	 * @brief constructor
	 * @details tries to load object from program storage
	 * @param standard standard instance
	 * @param file_name name of file in program storage
	 * @param deactivate_saving deactivate saving at destruction
	*/
	PersistingObject( const C&& standard, const char* file_name, const bool deactivate_saving = false );

	/*!
	 * @brief destructor
	 * @details stores instance on disc
	*/
	~PersistingObject( void );

	/*!
	 * @brief copy assignment operator
	 * @param instance instance to assign
	 * @return reference to this
	*/
	PersistingObject& operator=( const C& instance );

	/*!
	 * @brief check if an object was loaded
	 * @return true when object was loaded
	*/
	bool was_loaded( void ) const{ return was_loaded_; };
	
	/*!
	 * @brief set loaded flag
	*/
	void SetAsLoaded( void ){ was_loaded_ = true; };

	/*!
	 * @brief load object from specific file
	 * @param file_path path to serialized file
	 * @return true if file exists and contains valid data
	*/
	bool Load( const path file_path );

	/*!
	 * @brief save obejct to specific path
	 * @param file_path path to save object at
	 * @param force force saving when not loaded
	 * @return true at success
	*/
	bool Save( const path file_path, const bool force = false ) const;


	private:

	path file_path_;			/*!< path to file on filesystem*/
	bool was_loaded_;			/*!< flag to indicate whether the object was loaded or is default*/
	bool disable_saving;	/*!< flag to disable saving*/


	/*!
	 * @brief load file from stored path
	*/
	bool LoadFromFile( void ){ return Load( file_path_ ); };
	
	/*!
	 * @brief save the object
	 * @param force when false only store if object was loaded before
	*/
	bool SaveToFile( const bool force = false ) const;
};

#include "persistingObject.hpp"