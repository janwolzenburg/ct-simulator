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
		updateModelFlag( false ),
		resetBtnPressed( false )


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
		xRot.callback(		button_cb, &updateModelFlag );
		yRot.callback(		button_cb, &updateModelFlag );
		zTrans.callback(	button_cb, &updateModelFlag );
		resetBtn.callback(	button_cb, &resetBtnPressed );



		// Set values
		xRot.value( PROGRAM_STATE().Plane().rotationAngleX);
		yRot.value( PROGRAM_STATE().Plane().rotationAngleY );
		zTrans.value( PROGRAM_STATE().Plane().positionZ );

		// Hide initially
		moveGrp.hide();


	}



	void loadModel( void ){

		Fl_Group::window()->deactivate();
		viewImg.hide(); viewBox.show(); modelData.hide();
		viewBox.label( "Loading model..." );

		PROGRAM_STATE().loadModel();
		PROGRAM_STATE().resetModel();
		UpdateModel();

		viewImg.show(); viewBox.hide(); modelData.show();
		moveGrp.show();
		Fl_Group::window()->activate();
	}


	void handleEvents( void ){

		if( LoadBtnPressed() ){
			loadModel();
		}


		if( ModelNeedsUpdate() ){
			UpdateModel();
		}

		if( ResetBtnPressed() ){
			resetModel();
		}

	}

	/*!
	 * @brief Check load button press and reset when pressed
	 * @return True when it was pressed
	*/
	inline bool LoadBtnPressed( void ){ return loadBtnPressed ? !( loadBtnPressed = false ) : false; };

	inline bool ModelNeedsUpdate( void ){ return updateModelFlag && PROGRAM_STATE().ModelLoaded() ? !( updateModelFlag = false ) : false; };

	inline bool ResetBtnPressed( void ){ return resetBtnPressed ? !( resetBtnPressed = false ) : false; };


	void sliceModel( void ) {
		Fl_Group::window()->deactivate();
		PROGRAM_STATE().sliceModel();
		Fl_Group::window()->activate();
	}

	void resetModel( void ){

		Fl_Group::window()->deactivate();
		PROGRAM_STATE().resetModel();

		xRot.value( 0. );
		yRot.value( 0. );
		zTrans.value( 0. );
		Fl_Group::window()->activate();

	}



	void UpdateModel( void ){

		Fl_Group::window()->deactivate();
		PROGRAM_STATE().moveModel( xRot.value(), yRot.value(), zTrans.value() );

		viewImg.assignImage( PROGRAM_STATE().Slice() );

		modelDataString = PROGRAM_STATE().modelDescription();
		modelData.value( modelDataString.c_str());
		Fl_Group::window()->activate();

	}

	inline void setUpdateFlag( void ){ updateModelFlag = true; };

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

	bool updateModelFlag;
	bool resetBtnPressed;


};