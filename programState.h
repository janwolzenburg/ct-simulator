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

	inline const model& Model( void ){ return modelInstance; };

	inline const gantry& Gantry( void ){ return gantryInstance; };

	//bool sliceModel( void );


	inline greyImage& Slice( void ){ return modelSliceInstance; };

	//bool rotateViewX( const double angleDeg );

	//bool rotateViewY( const double angleDeg );

	//bool translateViewZ( const double amount );

	static void createStorageDir( void );

	static path getPath( const string filename ){ return stateStorage / filename; };

	void buildGantry( const tubeParameter tubeParameter_,
					  const detectorRadonParameter radonParameter, const detectorIndipendentParameter indipendentParameter );

	string modelDescription( void ) const;

	void moveModel( const double targetXRot, const double targetYRot, const double targetZTrans );

	void sliceModel( void );

	void centerModel( void );

	void resetModel( void );

	inline const slicePlane& Plane( void ) const{ return planeInstance; };

	inline const tubeParameter& TubeParameter( void ) const{ return xRayTubeParameter; };

	inline const detectorRadonParameter& RadonParameter( void ) const{ return radonParameter; };

	inline const detectorIndipendentParameter& DetectorParameter( void ) const{ return detectorParameter; };


	private:
	 

	programState( void );

	programState( const programState& tree_ ) = delete;

	programState& operator=( const programState& tree_ ) = delete;


	private:

	model modelInstance;
	storedObject<model> storedModel;

	fileChooser modelChooserInstance;
	storedObject<fileChooser> storedModelChooser;

	slicePlane planeInstance;
	storedObject<slicePlane> storedPlane;


	greyImage modelSliceInstance;



	// Voltage, Current, material
	tubeParameter xRayTubeParameter;
	storedObject<tubeParameter> storedXRayTubeParameter;

	// Numberpoints (2D), Distance range
	detectorRadonParameter radonParameter;
	storedObject<detectorRadonParameter> storedRadonParameter;

	// Pixel arc radius, pixel size col direction, Structere, structure max angle
	detectorIndipendentParameter detectorParameter;
	storedObject<detectorIndipendentParameter> storedDetectorParameter;


	gantry gantryInstance;

};

