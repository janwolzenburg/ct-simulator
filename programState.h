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

 #include "programState.fwd.h"
 #include "model.h"
 #include "fileChooser.h"


 /*********************************************************************
	Definitions
 *********************************************************************/

 template< class C >
 class storedObject{

	public:


	public:
	string fileName;
	C object;
	bool loaded;

 };

#ifdef WIN32

	const string stateStorageString{ ".\\stateStorage\\" };

#else

	const string stateStorageString{ "./stateStorage/" };

#endif // WIN32


 programState& PROGRAM_STATE( void );


class programState{

	public:

	static programState& getInstance();

	void loadModel( void );

	void loadModel( const path modelFile );

	void saveState( void ) const;

	private:
	 
	programState( void );

	programState( const programState& tree_ ) = delete;

	programState& operator=( const programState& tree_ ) = delete;


	private:

	const path stateStorage;

	const string modelChooserFilename = "storedModelChooser.chooser";
	fileChooser modelFileChooser;

	const string modelFilename = "storedModel.model";
	model currentModel;
	bool modelLoaded;


};
