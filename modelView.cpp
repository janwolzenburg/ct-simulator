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
#include "programState.h"
#include "mainWindow.h"


modelView::modelView( int x, int y, int w, int h, mainWindow* const main_window ) :
	
	main_window_( main_window ),

	modelChooserInstance{ "Choose CT model", "*.model", path{ "./" } },
	modelInstance{},
	modelViewPara{},
	modelSliceInstance{},
	
	storedModelChooser{ PROGRAM_STATE().getPath( "storedModelChooser.txt" ), modelChooserInstance },
	storedModel{ PROGRAM_STATE().getPath("storedModel.model"), modelInstance},
	storedViewParameter( PROGRAM_STATE().getPath( "storedViewParameter.txt" ), modelViewPara ),

	
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

	main_window_->add( this );

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
	xRot.value(modelViewPara.slice_plane.rotationAngleX );
	yRot.value( modelViewPara.slice_plane.rotationAngleY );
	zTrans.value( modelViewPara.slice_plane.positionZ );

	// Hide initially
	moveGrp.hide();

	if( ModelLoaded() ){
		viewImg.SetSliderBounds( modelInstance.attenuationRange() );
		viewImg.ChangeSliderValues( modelViewPara.contrast );
	}
}

modelView::~modelView( void ){
	storedModel.Save();
	storedModelChooser.Save();
	storedViewParameter.Save();
};


string modelView::modelDescription( void ) const{

	string modelDataString;

	modelDataString.clear();
	modelDataString += "Name: \t" + modelInstance.name() + '\n';
	modelDataString += "Voxel: \t\t\t" + ToString( modelInstance.number_of_voxel_3D().x ) + " x " + ToString( modelInstance.number_of_voxel_3D().y ) + " x " + ToString( modelInstance.number_of_voxel_3D().z ) + "\n";
	modelDataString += "Voxel Größe: \t" + ToString( modelInstance.voxel_size().x, 2 ) + " x " + ToString( modelInstance.voxel_size().y, 2 ) + " x " + ToString( modelInstance.voxel_size().z, 2 ) + "  mm^3\n";
	modelDataString += "Model Größe: \t" + ToString( modelInstance.size().x ) + " x " + ToString( modelInstance.size().y ) + " x " + ToString( modelInstance.size().z ) + "  mm^3";

	return modelDataString;
}

bool modelView::moveModel( double& targetXRot, double& targetYRot, double& targetZTrans ){

	const slicePlane backupPlane = modelViewPara.slice_plane; 
	slicePlane& planeInstance =  modelViewPara.slice_plane;

	const PrimitiveCoordinateSystem backupCSys = modelInstance.coordinate_system()->GetPrimitive();

	if( targetXRot != planeInstance.rotationAngleX ){

		const double rotationAngle = targetXRot - planeInstance.rotationAngleX;
		planeInstance.rotationAngleX = targetXRot;

		const Line axis{ planeInstance.surface.direction_1(), planeInstance.surface.origin() };

		modelInstance.coordinate_system()->Rotate( axis, rotationAngle / 360. * 2. * PI );
	}

	if( targetYRot != planeInstance.rotationAngleY ){

		const double rotationAngle = targetYRot - planeInstance.rotationAngleY;
		planeInstance.rotationAngleY = targetYRot;

		const Line axis{ planeInstance.surface.direction_2(), planeInstance.surface.origin() };

		modelInstance.coordinate_system()->Rotate( axis, rotationAngle / 360. * 2. * PI );
	}

	if( targetZTrans != planeInstance.positionZ ){

		const double translation = targetZTrans - planeInstance.positionZ;
		planeInstance.positionZ = targetZTrans;

		modelInstance.coordinate_system()->Translate( ( (Vector3D) planeInstance.surface.GetNormal() ) * translation );
	}

	// Return if succeeded
	if( sliceModel() ) return true;
	
	// Revert changes
	planeInstance = backupPlane;
	modelInstance.coordinate_system()->SetPrimitive( backupCSys );

	targetXRot = planeInstance.rotationAngleX;
	targetYRot = planeInstance.rotationAngleY;
	targetZTrans = planeInstance.positionZ;


	return false;
}


bool modelView::sliceModel( void ){
	Fl_Group::window()->deactivate();
	
	storedViewParameter.SetAsLoaded();

	DataGrid<VoxelData> tempSlice = modelInstance.GetSlice(  modelViewPara.slice_plane.surface, 1. );
	
	if( tempSlice.size().c == 0 || tempSlice.size().r == 0 )
	{
	
		Fl_Group::window()->activate();
		return false;
	}

	modelSliceInstance = tempSlice;

	
	return true;
}


void modelView::centerModel( void ){

	// Center model
	Tuple3D center = PrimitiveVector3{ modelInstance.size() } / -2.;

	modelInstance.coordinate_system()->SetPrimitive( PrimitiveCoordinateSystem{ center, Tuple3D{1,0,0}, Tuple3D{0,1,0}, Tuple3D{0,0,1} } );
}


void modelView::resetModel( void ){

	this->window()->deactivate();
		modelViewPara.slice_plane.rotationAngleX = 0.;
	 modelViewPara.slice_plane.rotationAngleY = 0.;
	 modelViewPara.slice_plane.positionZ = 0.;

	centerModel();

	xRot.value( 0. );
	yRot.value( 0. );
	zTrans.value( 0. );

	sliceModel();
	viewImg.AssignImage( modelSliceInstance );//, true );

	this->window()->activate();

}

bool modelView::loadModel( void ){

	this->window()->deactivate();

	viewImg.hide(); viewBox.show(); modelData.hide();
	viewBox.label( "Loading model..." );

	path modelToLoad = modelChooserInstance.ChooseFile();
	storedModelChooser.SetAsLoaded();

	if( !storedModel.Load( modelToLoad ) ){
		viewBox.label( "Loading failed!" );
		this->window()->activate();
		return false;
	}

	viewImg.SetSliderBounds( modelInstance.attenuationRange() );
	modelViewPara.contrast = viewImg.GetContrast();

	resetModel();
	UpdateModel();


	viewImg.show(); viewBox.hide(); modelData.show();
	moveGrp.show();
	this->window()->activate();

	return true;
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

	if( viewImg.HandleEvents() ){
		// Store contrast
		modelViewPara.contrast = viewImg.GetContrast();
	}
}




void modelView::UpdateModel( void ){

	this->window()->deactivate();
	
	// Store in variable for moveModel function call
	double oldXRot = xRot.value();
	double oldYRot = yRot.value();
	double oldZTrans = zTrans.value();

	// Movement did not succeed?
	if( !moveModel( oldXRot, oldYRot, oldZTrans ) ){
		// moveModel changes arguments to previous value
		xRot.value( oldXRot );
		yRot.value( oldYRot );
		zTrans.value( oldZTrans );
	}
	else{
		// New assignment only necessary, when movement succeeded
		viewImg.AssignImage( modelSliceInstance );//, false );
	}

	viewImg.show(); viewBox.hide(); modelData.show();
	moveGrp.show();

	modelDataString = modelDescription();
	modelData.value( modelDataString.c_str() );
	this->window()->activate();

}
