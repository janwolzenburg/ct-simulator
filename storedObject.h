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

#include <string>
using std::string;

#include <vector>
using std::vector;

#include "generel.h"


/*********************************************************************
   Definitions
*********************************************************************/

template< class C >
class storedObject{

	public:
	storedObject( const path filePath_, C& objectRef );

	bool load( const path filePath );

	void saveObject( const bool force = false ) const;

	inline bool Loaded( void ) const{ return loaded; };

	inline void setLoaded( void ){ loaded = true; };


	private:

	path file;
	C& object;
	bool loaded;

	void loadStored( void );

};


#include "storedObject.hpp"
