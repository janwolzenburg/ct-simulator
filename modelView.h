#pragma once
/*********************************************************************
 * @file   modelView.h
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


#include "generel.h"

#include <FL/Fl_Group.H>
#include <FL/Fl_Multiline_Output.H>
#include "widgets.h"

#include "programState.h"
#include "storedObject.h"
#include "slicePlane.h"



/*********************************************************************
   Definitions
*********************************************************************/

template<class C>
int X( const C& parent, const double start ){
	static_assert( std::is_base_of_v< Fl_Widget, C > );
	return parent.x() + (int) ( start * (double) parent.w() );
}

template<class C>
int Y( const C& parent, const double start ){
	static_assert( std::is_base_of_v< Fl_Widget, C > );
	return parent.y() + (int) ( start * (double) parent.h() );
}

template<class C>
int W( const C& parent, const double portion ){
	static_assert( std::is_base_of_v< Fl_Widget, C > );
	return (int) ( portion * (double) parent.w() );
}

template<class C>
int H( const C& parent, const double portion ){
	static_assert( std::is_base_of_v< Fl_Widget, C > );
	return (int) ( portion * (double) parent.h() );
}

template<class C>
int hOff( const C& parent  ){
	static_assert( std::is_base_of_v< Fl_Widget, C > );
	return parent.y() + parent.h();
}


class modelView : public Fl_Group{

	public: 

	modelView( int x, int y, int w, int h ) : 
		Fl_Group{ x, y, w, h },
		headGrp{		X( *this, 0. ),		Y( *this, 0. ),		W( *this, 1. ),		H( *this, .1 ) },
		loadBtn{		X( headGrp, .3 ),	Y( headGrp, .4 ),	W( headGrp, .4 ),	H( headGrp, .4 ),	"Load model" },

		viewGrp{		X( *this, 0. ),		hOff( headGrp ) ,	W( *this , 1. ),	H( *this, .6 ) },
		modelData{		X( viewGrp, 0. ),	Y( viewGrp, 0. ),	W( viewGrp, 1. ),	H( viewGrp, .2 ) },
		viewBox{		X( viewGrp, 0. ),	Y( viewGrp, .25 ),	W( viewGrp, 1.),	H( viewGrp, .75 ),	"No model loaded"},
		viewImg{		X( viewGrp, 0. ),	Y( viewGrp, .25 ),	W( viewGrp, 1. ),	H( viewGrp, .75 ) },

		moveGrp{		X( *this, 0. ),		hOff( viewGrp ),	W( *this, 1. ),		H( *this, .3 ) },
		xRot{			X( moveGrp, .1 ),	Y( moveGrp, .15 ),	W( moveGrp, .5 ),	H( moveGrp, .15 ), "x-Rotation" },
		yRot{			X( moveGrp, .1 ),	Y( moveGrp, .4 ),	W( moveGrp, .5 ),	H( moveGrp, .15 ), "y-Rotation" },
		zTrans{			X( moveGrp, .1 ),	Y( moveGrp, .65 ),	W( moveGrp, .5 ),	H( moveGrp, .15 ), "z-Translation" },
		resetBtn{		X( moveGrp, .7 ),	Y( moveGrp, .4 ),	W( moveGrp, .2 ),	H( moveGrp, .2 ), "Reset" },


		loadBtnPressed( false ),
		updateModel( false ),


		planeInstance{},
		storedPlane{ programState::getPath( "storedViewPlane.sliceplane" ), planeInstance }


	{
		
		//--------------------------- Head ---------------------------//

		Fl_Group::add( headGrp );

		// Labelsize and callback
		headGrp.add( loadBtn );
		loadBtn.labelsize( (int) ( .5 * (double) loadBtn.h() ) );		
		loadBtn.callback( button_cb, &loadBtnPressed );
		


		//--------------------------- View ---------------------------//

		// View group dictates resizing
		Fl_Group::add_resizable( viewGrp );
		viewGrp.add( modelData );
		viewGrp.add( viewBox );
		viewGrp.add( viewImg );

		// Model data
		modelData.hide();

		// Labelsize and box
		viewBox.labelsize( (int) ( .05 * (double) viewBox.h() ) );
		viewBox.box( FL_BORDER_BOX );
		
		// Hide image initially
		viewGrp.resizable( viewBox );
		viewImg.hide();


		//--------------------------- Movement ---------------------------//

		Fl_Group::add( moveGrp );
		moveGrp.add( xRot );
		moveGrp.add( yRot );
		moveGrp.add( zTrans );
		moveGrp.add( resetBtn );

		// Counter ranges
		xRot.range( -180., 180. );		xRot.step( 1., 10. );
		yRot.range( -180., 180. );		yRot.step( 1., 10. );
		zTrans.range( -500., 500. );	zTrans.step( 1., 10. );

		// Labelsizes
		xRot.labelsize(		(int) ( .40 * (double) xRot.h() ) );
		yRot.labelsize(		(int) ( .40 * (double) yRot.h() ) );
		zTrans.labelsize(	(int) ( .40 * (double) zTrans.h() ) );	
		resetBtn.labelsize( (int) ( .6 * (double) resetBtn.h() ) );		

		// Callbacks for Counters and reset button
		xRot.callback(		button_cb, &updateModel );
		yRot.callback(		button_cb, &updateModel );
		zTrans.callback(	button_cb, &updateModel );
		resetBtn.callback(	button_cb, &resetBtnPressed );

		// Set values
		xRot.value( planeInstance.rotationAngleX );
		yRot.value( planeInstance.rotationAngleY );
		zTrans.value( planeInstance.positionZ );

		// Hide initially
		moveGrp.hide();


	}

	~modelView( void ){

		storedPlane.saveObject( true );

	}

	void handleEvents( void ){

		if( LoadBtnPressed() ){
			
			Fl_Group::deactivate();

			viewImg.hide(); viewBox.show(); modelData.hide();
			viewBox.label( "Loading model...");
			
			PROGRAM_STATE().loadModel();
			model& modelRef = PROGRAM_STATE().Model();
			resetModel( modelRef );
			UpdateModel( modelRef );

		}


		if( ModelNeedsUpdate() && PROGRAM_STATE().ModelLoaded() ){
			Fl_Group::deactivate();
			UpdateModel( PROGRAM_STATE().Model() );
		}

		if( ResetBtnPressed() && PROGRAM_STATE().ModelLoaded() ){
			Fl_Group::deactivate();
			resetModel( PROGRAM_STATE().Model() );
			UpdateModel( PROGRAM_STATE().Model() );
		}

	}

	/*!
	 * @brief Check load button press and reset when pressed
	 * @return True when it was pressed
	*/
	inline bool LoadBtnPressed( void ){ return loadBtnPressed ? !( loadBtnPressed = false ) : false; };

	inline bool ModelNeedsUpdate( void ){ return updateModel ? !( updateModel = false ) : false; };

	inline bool ResetBtnPressed( void ){ return resetBtnPressed ? !( resetBtnPressed = false ) : false; };


	void sliceModel( model& model ) {

		grid modelSliceGrid = model.getSlice( planeInstance.surface, 1. );
		modelSliceInstance = greyImage{ modelSliceGrid };

		moveGrp.show();
		moveGrp.activate();
	}

	void resetModel( model& model ){

		centerModel( model );

		// Reset plane
		planeInstance.rotationAngleX = 0.;
		planeInstance.rotationAngleY = 0.;
		planeInstance.positionZ = 0.;

		xRot.value( 0. );
		yRot.value( 0. );
		zTrans.value( 0. );

	}

	void centerModel( model& model ){

		// Center model
		v3 center = primitiveVec3{ model.ModSize() } / -2.;

		model.CSys()->setPrimitive( primitiveCartCSys{ center, v3{1,0,0}, v3{0,1,0}, v3{0,0,1} } );


	}

	void UpdateModel( model& model ){

		Fl_Group::deactivate();

		if( xRot.value() != planeInstance.rotationAngleX ){

			const double rotationAngle = xRot.value() - planeInstance.rotationAngleX;
			planeInstance.rotationAngleX = xRot.value();
			
			const line axis{ planeInstance.surface.R1(), planeInstance.surface.O() };

			model.CSys()->rotateM( axis, rotationAngle / 360. * 2. * PI );
		}

		if( yRot.value() != planeInstance.rotationAngleY ){

			const double rotationAngle = yRot.value() - planeInstance.rotationAngleY;
			planeInstance.rotationAngleY = yRot.value();

			const line axis{ planeInstance.surface.R2(), planeInstance.surface.O() };

			model.CSys()->rotateM( axis, rotationAngle / 360. * 2. * PI );
		}

		if( zTrans.value() != planeInstance.rotationAngleY ){

			const double translation = zTrans.value() - planeInstance.positionZ;
			planeInstance.positionZ = zTrans.value();

			model.CSys()->translateM( ( (vec3) planeInstance.surface.Normal() ) * translation );
		}


		sliceModel( model );
		viewImg.assignImage( modelSliceInstance );


		modelDataString.clear();
		modelDataString += "Name: \t" + model.Name() + '\n';
		modelDataString += "Voxel: \t\t\t" + toString( model.NumVox().x ) + " x " + toString( model.NumVox().y ) + " x " + toString( model.NumVox().z ) + "\n";
		modelDataString += "Voxel Größe: \t" + toString( model.VoxSize().x, 2 ) + " x " + toString( model.VoxSize().y, 2 ) + " x " + toString( model.VoxSize().z, 2 ) + "  mm^3\n";
		modelDataString += "Model Größe: \t" + toString( model.ModSize().x ) + " x " + toString( model.ModSize().y ) + " x " + toString( model.ModSize().z ) + "  mm^3";

		modelData.value( modelDataString.c_str() );


		viewGrp.resizable( viewImg );
		viewImg.show();
		viewBox.hide();
		modelData.show();
		Fl_Group::activate();
	}



	private:

	Fl_Group headGrp;
	Fl_Button loadBtn;

	Fl_Group viewGrp;
	Fl_Multiline_Output modelData;
	Fl_Box viewBox;
	Fl_Image_Widget viewImg;
	
	Fl_Group moveGrp;
	Fl_Counter xRot;
	Fl_Counter yRot;
	Fl_Counter zTrans;
	Fl_Button resetBtn;

	string modelDataString;

	bool loadBtnPressed;

	bool updateModel;
	bool resetBtnPressed;


	slicePlane planeInstance;
	storedObject<slicePlane> storedPlane;

	greyImage modelSliceInstance;

};