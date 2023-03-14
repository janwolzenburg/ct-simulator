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

	bool load( const path filePath ){

		// Does the file exist?
		if( !std::filesystem::exists( filePath ) ) return false;

		// Load file
		vector<char> binaryData = importSerialized( filePath );
		vector<char>::iterator binaryDataIt = binaryData.begin();

		if( !validBinaryData( object.FILE_PREAMBLE, binaryData, binaryDataIt ) ) return false;

		object = C{ binaryData, binaryDataIt };
		loaded = true;
		return loaded;
	};

	void saveObject( const bool force = false ) const {

		if( !loaded && !force ) return;

		vector<char> binaryData;
		object.serialize( binaryData );

		exportSerialized( file, binaryData );

	};

	inline bool Loaded( void ) const { return loaded; };

	inline void setLoaded( void ){ loaded = true; };



	private:
	path file;
	C& object;
	bool loaded;

	void loadStored( void ){

		load( file );
		
	};

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

	void saveState( void ) const;


	inline bool ModelLoaded( void ) const{ return storedModel.Loaded(); };

	bool loadModel( void );

	inline model& Model( void ){ return storedModelInstance; };


	bool sliceModel( void );

	inline bool SliceLoaded( void ) const{ return storedModelSlice.Loaded(); };

	inline greyImage& Slice( void ){ return modelSliceInstance; };


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

	fileChooser storedModelChooserInstance;
	storedObject<fileChooser> storedModelChooser;

	static const surfLim viewPlane;
	greyImage modelSliceInstance;
	storedObject<greyImage> storedModelSlice;


};
