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
 #include "cSysTree.h"
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


 class slicePlane {

	public:
	slicePlane( void ) :
		cSys( CSYS_TREE().addCSys("Slice plane system") ),
		surface{	uvec3{ v3{ 1, 0, 0 }, cSys },
					uvec3{ v3{ 0, 1, 0 }, cSys },
					pnt3{  v3{0, 0, 0}, cSys },
					-300, 300, -300, 300 },
		rotationAngleX( 0. ),
		rotationAngleY( 0. ),
		positionZ( 0. )
	{
	
	};
	
	slicePlane( const vector<char>& binData, vector<char>::const_iterator& it ) :
		cSys{  CSYS_TREE().addCSys( binData, it ) },
		surface{ binData, it, cSys },
		rotationAngleX( deSerializeBuildIn<double>( (double) 0., binData, it )  ),
		rotationAngleY( deSerializeBuildIn<double>( (double) 0., binData, it ) ),
		positionZ( deSerializeBuildIn<double>( (double) 0., binData, it ) )
	{

		

	}

	void rotX( const double targetAngle ){

		const double rotationAngle = targetAngle - rotationAngleX;

		cSys->rotateM( cSys->Parent()->xAxis(), rotationAngle );
		rotationAngleX = targetAngle;

	}


	size_t serialize( vector<char>& binData ) const{
		
		size_t numBytes = 0;
		numBytes += cSys->serialize( binData );
		numBytes += surface.serialize( binData );
		numBytes += serializeBuildIn( rotationAngleX, binData );
		numBytes += serializeBuildIn( rotationAngleY, binData );
		numBytes += serializeBuildIn( positionZ, binData );

		return numBytes;

	};

	public:
	cartCSys* cSys;
	surfLim surface;
	double rotationAngleX;
	double rotationAngleY;
	double positionZ;

	static const string FILE_PREAMBLE;

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

	inline model& Model( void ){ return modelInstance; };


	bool sliceModel( void );

	//inline bool SliceLoaded( void ) const{ return storedModelSlice.Loaded(); };

	inline greyImage& Slice( void ){ return modelSliceInstance; };

	bool rotateViewX( const double angleDeg ) { 
		viewPlaneInstance.rotX( angleDeg / 360. * 2. * PI );
		return sliceModel();
	};


	private:
	 
	path getPath( const string filename ) const{

		return stateStorage / filename;

	};

	programState( void );

	programState( const programState& tree_ ) = delete;

	programState& operator=( const programState& tree_ ) = delete;


	private:


	model modelInstance;
	storedObject<model> storedModel;

	fileChooser modelChooserInstance;
	storedObject<fileChooser> storedModelChooser;

	slicePlane viewPlaneInstance;
	storedObject<slicePlane> storedViewPlane;

	greyImage modelSliceInstance;
	


};
