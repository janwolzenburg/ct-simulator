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

 #include <vector>
 using std::vector;

 #include "programState.fwd.h"

 #include "model.h"
 #include "fileChooser.h"
 #include "slicePlane.h"
 #include "storedObject.h"
 #include "image.h"

 /*********************************************************************
	Definitions
 *********************************************************************/

 class valuatorStatus{

	public:

	valuatorStatus( void ) : xRotValue( 0. ), yRotValue( 0. ), zTransValue( 0. ) {};

	valuatorStatus( const vector<char>& binData, vector<char>::const_iterator& it ) :
		xRotValue( deSerializeBuildIn( 0., binData, it ) ),
		yRotValue( deSerializeBuildIn( 0., binData, it ) ),
		zTransValue( deSerializeBuildIn( 0., binData, it ) )
	{};

	size_t serialize( vector<char>& binData ) const{

		size_t numBytes = 0;

		numBytes += serializeBuildIn( FILE_PREAMBLE, binData );
		numBytes += serializeBuildIn( xRotValue, binData );
		numBytes += serializeBuildIn( yRotValue, binData );
		numBytes += serializeBuildIn( zTransValue, binData );

		return numBytes;

	};

	public:


	static const string FILE_PREAMBLE;

	double xRotValue;
	double yRotValue;
	double zTransValue;
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

	void centerModel( void );

	bool sliceModel( void );


	inline greyImage& Slice( void ){ return modelSliceInstance; };

	bool rotateViewX( const double angleDeg );

	bool rotateViewY( const double angleDeg );

	bool translateViewZ( const double amount );

	valuatorStatus& getValStatus( void );


	private:
	 
	path getPath( const string filename ) const;

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
	
	valuatorStatus valStatusInstance;
	storedObject<valuatorStatus> storedValStatus;

};
