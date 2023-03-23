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

	//inline model& Model( void ){ return modelInstance; };

	//bool sliceModel( void );


	inline greyImage& Slice( void ){ return modelSliceInstance; };

	//bool rotateViewX( const double angleDeg );

	//bool rotateViewY( const double angleDeg );

	//bool translateViewZ( const double amount );

	static void createStorageDir( void );

	static path getPath( const string filename ){ return stateStorage / filename; };

	void buildGantry( const tubeParameter tubeParameter_,
					  const detectorRadonParameter radonParameter, const detectorIndipendentParameter indipendentParameter ){

		gantryInstance.reset();
		gantry newGantry{ gantryInstance.CSys(), tubeParameter_, radonParameter, indipendentParameter };

		gantryInstance = newGantry;

	};

	string modelDescription( void ) const{

		string modelDataString;

		modelDataString.clear();
		modelDataString += "Name: \t" + modelInstance.Name() + '\n';
		modelDataString += "Voxel: \t\t\t" + toString( modelInstance.NumVox().x ) + " x " + toString( modelInstance.NumVox().y ) + " x " + toString( modelInstance.NumVox().z ) + "\n";
		modelDataString += "Voxel Größe: \t" + toString( modelInstance.VoxSize().x, 2 ) + " x " + toString( modelInstance.VoxSize().y, 2 ) + " x " + toString( modelInstance.VoxSize().z, 2 ) + "  mm^3\n";
		modelDataString += "Model Größe: \t" + toString( modelInstance.ModSize().x ) + " x " + toString( modelInstance.ModSize().y ) + " x " + toString( modelInstance.ModSize().z ) + "  mm^3";

		return modelDataString;
	}

	void moveModel( const double targetXRot, const double targetYRot, const double targetZTrans ){


		if( targetXRot != planeInstance.rotationAngleX ){

			const double rotationAngle = targetXRot - planeInstance.rotationAngleX;
			planeInstance.rotationAngleX = targetXRot;

			const line axis{ planeInstance.surface.R1(), planeInstance.surface.O() };

			modelInstance.CSys()->rotateM( axis, rotationAngle / 360. * 2. * PI );
		}

		if( targetYRot != planeInstance.rotationAngleY ){

			const double rotationAngle = targetYRot - planeInstance.rotationAngleY;
			planeInstance.rotationAngleY = targetYRot;

			const line axis{ planeInstance.surface.R2(), planeInstance.surface.O() };

			modelInstance.CSys()->rotateM( axis, rotationAngle / 360. * 2. * PI );
		}

		if( targetZTrans != planeInstance.rotationAngleY ){

			const double translation = targetZTrans - planeInstance.positionZ;
			planeInstance.positionZ = targetZTrans;

			modelInstance.CSys()->translateM( ( (vec3) planeInstance.surface.Normal() ) * translation );
		}


		sliceModel();

	}

	void sliceModel( void ){

		grid modelSliceGrid = modelInstance.getSlice( planeInstance.surface, 1. );
		modelSliceInstance = greyImage{ modelSliceGrid };

	}

	void centerModel( void ){

		// Center model
		v3 center = primitiveVec3{ modelInstance.ModSize() } / -2.;

		modelInstance.CSys()->setPrimitive( primitiveCartCSys{ center, v3{1,0,0}, v3{0,1,0}, v3{0,0,1} } );


	}

	void resetModel( void ){

		// Reset plane
		planeInstance.rotationAngleX = 0.;
		planeInstance.rotationAngleY = 0.;
		planeInstance.positionZ = 0.;

	}

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

