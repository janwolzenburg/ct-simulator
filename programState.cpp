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
	#define STATE_STORAGE ".\\stateStorage\\"
#else
	#define STATE_STORAGE  "./stateStorage/"
#endif // WIN32


ProgramState& PROGRAM_STATE( void ){
	return ProgramState::GetInstance();
}


ProgramState& ProgramState::GetInstance(){
	static ProgramState instance;
	return instance;
}

ProgramState::ProgramState( void ) :

	reset_state_at_exit_( false ),
	storage_path_{ STATE_STORAGE }

{
	CreateStorageDirectory();
}

ProgramState::~ProgramState( void ) {
	
	if( reset_state_at_exit_ ){
		DeleteStorageDirectory();
		return;
	}
	
}


void ProgramState::CreateStorageDirectory( void ){
	// Check if state storage directory erxists
	if( !std::filesystem::is_directory( storage_path_ ) ) std::filesystem::create_directory( storage_path_ );
}

void ProgramState::DeleteStorageDirectory( void ){

	// Check if state storage directory erxists
	if( !std::filesystem::is_directory( storage_path_ ) ) return;

	// Remove all content
	for( const auto& file : std::filesystem::directory_iterator( storage_path_ ) )
		std::filesystem::remove_all( file.path() );
}
