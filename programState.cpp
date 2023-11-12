/*********************************************************
* @file   programState.cpp
* @brief  Implementations
*
* @author Jan Wolzenburg
* @date   March 2023
* ********************************************************************/


/*********************************************************************
	Includes
 *********************************************************************/

#include "programState.h"


 /*********************************************************************
	Implementations
 *********************************************************************/


#ifdef WIN32
	const path programState::stateStorage{ ".\\stateStorage\\" };
#else
	const path programState::stateStorage{ "./stateStorage/" };
#endif // WIN32


programState& PROGRAM_STATE( void ){
	return programState::getInstance();
}


programState& programState::getInstance(){
	static programState instance;
	return instance;
}

programState::programState( void ) :

	resetStateAtExit( false )

{
	createStorageDir();
}

programState::~programState( void ) {
	
	if( resetStateAtExit ){
		deleteStorageDir();
		return;
	}
	
}


void programState::createStorageDir( void ){
	// Check if state storage directory erxists
	if( !std::filesystem::is_directory( stateStorage ) ) std::filesystem::create_directory( stateStorage );
}

void programState::deleteStorageDir( void ){

	// Check if state storage directory erxists
	if( !std::filesystem::is_directory( stateStorage ) ) return;

	// Remove all content
	for( const auto& file : std::filesystem::directory_iterator( stateStorage ) )
		std::filesystem::remove_all( file.path() );
}
