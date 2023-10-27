#pragma once
/*********************************************************************
 * @file   .h
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
class storedObject{

	public:

	/*!
	 * @brief Constructor
	 * @param file_path_ Path to file
	 * @param objectRef Reference to object
	*/
	storedObject( const path file_path_, C& objectRef );

	/*!
	 * @brief Load object from specific file
	 * @param file_path Path to serialized file
	 * @return True if file exists and contains valid data_
	*/
	bool load( const path file_path );

	/*!
	 * @brief Save the object
	 * @param force When false only store if object was loaded before
	*/
	void saveObject( const bool force = false ) const;

	/*!
	 * @brief Check if an object was loaded
	 * @return True when object was loaded
	*/
	bool Loaded( void ) const{ return loaded; };

	/*!
	 * @brief Set loaded flag
	*/
	void setLoaded( void ){ loaded = true; };


	private:

	path file;		/*!>Path to file on filesystem*/
	C& object;		/*!>Reference to object*/
	bool loaded;	/*!>Flag to */

	/*!
	 * @brief Load file from stored path
	*/
	void loadStored( void );

};

#include "storedObject.hpp"