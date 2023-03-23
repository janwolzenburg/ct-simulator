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
 #include "gantry.h"

 #include "fileChooser.h"
 #include "slicePlane.h"
 #include "storedObject.h"
 #include "image.h"

 /*********************************************************************
	Definitions
 *********************************************************************/


 programState& PROGRAM_STATE( void );


class programState{

	public:

	static const path stateStorage;


	public:

	static programState& getInstance();

	~programState( void );


	inline bool ModelLoaded( void ) const{ return storedModel.Loaded(); };

	bool loadModel( void );

	inline model& Model( void ){ return modelInstance; };

	//bool sliceModel( void );


	//inline greyImage& Slice( void ){ return modelSliceInstance; };

	//bool rotateViewX( const double angleDeg );

	//bool rotateViewY( const double angleDeg );

	//bool translateViewZ( const double amount );

	static void createStorageDir( void );

	static path getPath( const string filename ){ return stateStorage / filename; };


	private:
	 

	programState( void );

	programState( const programState& tree_ ) = delete;

	programState& operator=( const programState& tree_ ) = delete;


	private:

	model modelInstance;
	storedObject<model> storedModel;

	fileChooser modelChooserInstance;
	storedObject<fileChooser> storedModelChooser;


	gantry gantryInstance;

};

