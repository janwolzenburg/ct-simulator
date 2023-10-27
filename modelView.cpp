/******************************************************************
* @file   modelView.cpp
* @brief  Implementations
*
* @author Jan Wolzenburg
* @date   April 2023
* ********************************************************************/



/*********************************************************************
  Includes
*********************************************************************/

#include "modelView.h"
#include "serialization.h"
#include "widgets.h"


/*********************************************************************
  Implementations
*********************************************************************/

modelView::modelView( int x, int y, int w, int h ) :
	Fl_Group{ x, y, w, h },
	headGrp{	X( *this, 0. ),		Y( *this, 0. ),		W( *this, 1. ),		H( *this, .05 ) },
	loadBtn{	X( headGrp, .3 ),	Y( headGrp, .05 ),	W( headGrp, .4 ),	H( headGrp, .9 ),	"Load model" },

	viewGrp{	X( *this, 0. ),		vOff( headGrp ) ,	W( *this , 1. ),	H( *this, .7 ) },
	modelData{	X( viewGrp, 0. ),	Y( viewGrp, 0. ),	W( viewGrp, 1. ),	H( viewGrp, .15 ) },
	viewBox{	X( viewGrp, 0. ),	Y( viewGrp, .175 ),	W( viewGrp, 1. ),	H( viewGrp, .725 ),	"No model loaded" },
	viewImg{	X( viewGrp, 0. ),	Y( viewGrp, .175 ),	W( viewGrp, 1. ),	H( viewGrp, .725 ) },

	moveGrp{	X( *this, 0. ),		vOff( viewGrp ),	W( *this, 1. ),		H( *this, .25 ) },
	xRot{		X( moveGrp, .1 ),	Y( moveGrp, .1 ),	W( moveGrp, .5 ),	H( moveGrp, .1 ), "x-Rotation" },
	yRot{		X( moveGrp, .1 ),	Y( moveGrp, .35 ),	W( moveGrp, .5 ),	H( moveGrp, .1), "y-Rotation" },
	zTrans{		X( moveGrp, .1 ),	Y( moveGrp, .6 ),	W( moveGrp, .5 ),	H( moveGrp, .1 ), "z-Translation" },
	resetBtn{	X( moveGrp, .7 ),	Y( moveGrp, .4 ),	W( moveGrp, .2 ),	H( moveGrp, .2 ), "Reset" },

	loadBtnPressed( false ),
	updateModelFlag( false ),
	resetBtnPressed( false )

{

	//--------------------------- Head ---------------------------//

	Fl_Group::add( headGrp );
	Fl_Group::box( FL_BORDER_BOX );

	// Labelsize and callback
	headGrp.add( loadBtn );
	headGrp.box( FL_BORDER_BOX );
	loadBtn.labelsize( (int) ( .5 * (double) loadBtn.h() ) );
	loadBtn.callback( button_cb, &loadBtnPressed );



	//--------------------------- View ---------------------------//

	// View group dictates resizing
	Fl_Group::add_resizable( viewGrp );
	viewGrp.box( FL_BORDER_BOX );
	viewGrp.add( modelData );
	viewGrp.add( viewBox );
	viewGrp.add( viewImg );


	// Model data_
	modelData.hide();

	// Labelsize and box
	viewBox.labelsize( (int) ( .05 * (double) viewBox.h() ) );
	viewBox.box( FL_BORDER_BOX );

	// Hide image initially
	viewGrp.resizable( viewBox );
	viewImg.hide();


	//--------------------------- Movement ---------------------------//

	Fl_Group::add( moveGrp );
	moveGrp.box( FL_BORDER_BOX );
	moveGrp.add( xRot );
	moveGrp.add( yRot );
	moveGrp.add( zTrans );
	moveGrp.add( resetBtn );

	// Counter ranges
	xRot.range( -180., 180. );		xRot.step( 1., 10. );
	yRot.range( -180., 180. );		yRot.step( 1., 10. );
	zTrans.range( -500., 500. );	zTrans.step( 1., 10. );

	// Labelsizes
	xRot.labelsize( (int) ( .80 * (double) xRot.h() ) );
	yRot.labelsize( (int) ( .80 * (double) yRot.h() ) );
	zTrans.labelsize( (int) ( .80 * (double) zTrans.h() ) );
	resetBtn.labelsize( (int) ( .6 * (double) resetBtn.h() ) );

	// Callbacks for Counters and reset button
	xRot.callback( button_cb, &updateModelFlag );
	yRot.callback( button_cb, &updateModelFlag );
	zTrans.callback( button_cb, &updateModelFlag );
	resetBtn.callback( button_cb, &resetBtnPressed );



	// Set values
	xRot.value( PROGRAM_STATE().modelViewPara.plane.rotationAngleX );
	yRot.value( PROGRAM_STATE().modelViewPara.plane.rotationAngleY );
	zTrans.value( PROGRAM_STATE().modelViewPara.plane.positionZ );

	// Hide initially
	moveGrp.hide();

	if( PROGRAM_STATE().ModelLoaded() ){
		viewImg.setSliderBounds( PROGRAM_STATE().model().attenuationRange() );
		viewImg.changeContrast( PROGRAM_STATE().modelViewPara.viewContrast );
	}
}

void modelView::loadModel( void ){

	Fl_Group::window()->deactivate();
	viewImg.hide(); viewBox.show(); modelData.hide();
	viewBox.label( "Loading model..." );

	PROGRAM_STATE().loadModel();
	PROGRAM_STATE().resetModel();

	UpdateModel();

	viewImg.setSliderBounds( PROGRAM_STATE().model().attenuationRange() );
	PROGRAM_STATE().modelViewPara.viewContrast = viewImg.getContrast();

	viewImg.show(); viewBox.hide(); modelData.show();
	moveGrp.show();
	Fl_Group::window()->activate();
}

void modelView::handleEvents( void ){

	if( LoadBtnPressed() ){
		loadModel();
	}

	if( ModelNeedsUpdate() ){
		UpdateModel();
	}

	if( ResetBtnPressed() ){
		resetModel();
	}

	if( viewImg.handleEvents() ){
		// Store contrast
		PROGRAM_STATE().modelViewPara.viewContrast = viewImg.getContrast();
	}
}

void modelView::sliceModel( void ){
	Fl_Group::window()->deactivate();
	PROGRAM_STATE().sliceModel();
	Fl_Group::window()->activate();
}

void modelView::resetModel( void ){

	Fl_Group::window()->deactivate();
	PROGRAM_STATE().resetModel();

	xRot.value( 0. );
	yRot.value( 0. );
	zTrans.value( 0. );

	PROGRAM_STATE().sliceModel();
	viewImg.assignImage( PROGRAM_STATE().modelSliceInstance, true );

	Fl_Group::window()->activate();

}

void modelView::UpdateModel( void ){

	Fl_Group::window()->deactivate();
	
	// Store in variable for moveModel function call
	double oldXRot = xRot.value();
	double oldYRot = yRot.value();
	double oldZTrans = zTrans.value();

	// Movement did not succeed?
	if( !PROGRAM_STATE().moveModel( oldXRot, oldYRot, oldZTrans ) ){
		// moveModel changes arguments to previous value
		xRot.value( oldXRot );
		yRot.value( oldYRot );
		zTrans.value( oldZTrans );
	}
	else{
		// New assignment only necessary, when movement succeeded
		viewImg.assignImage( PROGRAM_STATE().modelSliceInstance, true );
	}

	viewImg.show(); viewBox.hide(); modelData.show();
	moveGrp.show();

	modelDataString = PROGRAM_STATE().modelDescription();
	modelData.value( modelDataString.c_str() );
	Fl_Group::window()->activate();

}


const string modelViewParameter::FILE_PREAMBLE{ "Ver01MODELVIEWPARAMETER_FILE_PREAMBLE" };

size_t modelViewParameter::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;

	num_bytes += SerializeBuildIn( FILE_PREAMBLE, binary_data );
	num_bytes += viewContrast.Serialize( binary_data );
	num_bytes += plane.Serialize( binary_data );

	return num_bytes;

}