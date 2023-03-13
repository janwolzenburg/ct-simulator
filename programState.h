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
 #include "image.h"



 /*********************************************************************
	Definitions
 *********************************************************************/

 /*!
  * @brief 
  * @tparam C Class with ... 
 */
 template< class C >
 class storedObject{

	public:
	storedObject( const path filePath_, C& objectRef ) :
		file( filePath_ ),
		object( objectRef ),
		loaded( false )
	{
		loadStored();
	};

	void loadStored( void ){
		
		// Does the file exist?
		if( !std::filesystem::exists( file ) ) return;

		// Load file
		vector<char> binaryData = importSerialized( file );
		vector<char>::iterator binaryDataIt = binaryData.begin();

		if( !validBinaryData( object.FILE_PREAMBLE, binaryData, binaryDataIt ) ) return;

		object = C{ binaryData, binaryDataIt };
		loaded = true;
	};

	public:
	path file;
	C& object;
	bool loaded;

 };





 programState& PROGRAM_STATE( void );


class programState{

	public:

	#ifdef WIN32

		const path stateStorage{ ".\\stateStorage\\" };

	#else

		const path stateStorage{ "./stateStorage/" };

	#endif // WIN32


	public:

	static programState& getInstance();


	//inline bool ModelLoaded( void ) const{ return modelLoaded; };

	// void loadModel( void );

	// void loadModel( const path modelFile );

	//inline model& Model( void ) { return currentModel; };

	//inline greyImage& ModelSlice( void )  { return modelSlice; };

	//void saveState( void ) const;



	private:
	 
	path getPath( const string filename ) const{

		return stateStorage / filename;

	};

	programState( void );

	programState( const programState& tree_ ) = delete;

	programState& operator=( const programState& tree_ ) = delete;


	private:


	model storedModelInstance;
	storedObject<model> storedModel;

	//const string modelChooserFilename = "storedModelChooser.chooser";
	//fileChooser modelFileChooser;

	//const string modelFilename = "storedModel.model";
	//model currentModel;
	//bool modelLoaded;

	//const string modelSliceImageFilename = "modelSliceImage.image";
	//greyImage modelSlice;
	//bool modelSliceLoaded;

};
