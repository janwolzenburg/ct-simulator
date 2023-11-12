#pragma once
/*********************************************************************
 * @file   persistingObject.h
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

/*!
 * @brief Class for a persisting object instance to be stored on Disc
 * @tparam C Class of object
*/
template< class C >
class PersistingObject : public C{

	
	static_assert( std::is_pointer<decltype( &C::FILE_PREAMBLE )>::value );
	static_assert( std::is_member_function_pointer<decltype( &C::Serialize )>::value );

	public:

	/*!
	 * @brief Constructor
	 * @details Tries to load object from path
	 * @param standard Standard instance
	 * @param file_path Path to load from and save file
	*/
	PersistingObject( const C&& standard, const path file_path );

	/*!
	 * @brief Constructor
	 * @details Tries to load object from program storage
	 * @param standard Standard instance
	 * @param file_name Name of file in program storage
	*/
	PersistingObject( const C&& standard, const char* file_name );

	/*!
	 * @brief Destructor
	 * @details Stored instance on Disc
	*/
	~PersistingObject( void );

	/*!
	 * @brief Copy assignment operator
	 * @param instance Instance to assign
	 * @return Reference to this
	*/
	PersistingObject& operator=( const C& instance );

	/*!
	 * @brief Check if an object was loaded
	 * @return True when object was loaded
	*/
	bool was_loaded( void ) const{ return was_loaded_; };
	
	/*!
	 * @brief Set loaded flag
	*/
	void SetAsLoaded( void ){ was_loaded_ = true; };

	/*!
	 * @brief Load object from specific file
	 * @param file_path Path to serialized file
	 * @return True if file exists and contains valid data
	*/
	bool Load( const path file_path );

	/*!
	 * @brief Save the object
	 * @param force When false only store if object was loaded before
	*/
	bool Save( const bool force = false ) const;


	private:

	path file_path_;		/*!>Path to file on filesystem*/
	bool was_loaded_;		/*!>Flag to indicate whether the object was loaded or is default*/


	/*!
	 * @brief Load file from stored path
	*/
	bool LoadFromFile( void ){ return Load( file_path_ ); };

};

#include "persistingObject.hpp"