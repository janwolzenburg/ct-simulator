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


ModelView::ModelView( int x, int y, int w, int h, mainWindow& main_window ) :
	
	main_window_( main_window ),

	modelChooserInstance{ "Choose CT model", "*.model", path{ "./" } },
	model_{},
	modelViewPara{},
	modelSliceInstance{},
	
	storedModelChooser{ PROGRAM_STATE().getPath( "storedModelChooser.txt" ), modelChooserInstance },
	storedModel{ PROGRAM_STATE().getPath("storedModel.model"), model_},
	storedViewParameter( PROGRAM_STATE().getPath( "storedViewParameter.txt" ), modelViewPara ),

	
	Fl_Group{ x, y, w, h },
	
	title{			X( *this, 0. ),		Y( *this, 0. ),		W( *this, 1. ),		H( *this, 0.035 ),	"Model" },

	headGrp{	X( *this, 0. ),		Y( *this, 0.04 ),		W( *this, 1. ),		H( *this, .05 ) },
	loadBtn{	X( headGrp, .1 ),	Y( headGrp, .05 ),	W( headGrp, .25 ),	H( headGrp, .9 ),	"Load model" },

	viewGrp{	X( *this, 0. ),		vOff( headGrp ),	W( *this , 1. ),	H( *this, .75 ) },
	modelData{	X( viewGrp, 0. ),	Y( viewGrp, 0. ),	W( viewGrp, .5 ),	H( viewGrp, .15 ) },
	resetBtn{	X( viewGrp, .6 ),	Y( viewGrp, .06 ),	W( viewGrp, .25 ),	H( viewGrp, .03 ), "Reset model" },

	viewBox{	X( viewGrp, 0. ),	Y( viewGrp, .175 ),	W( viewGrp, 1. ),	H( viewGrp, .725 ),	"No model loaded" },
	viewImg{	X( viewGrp, 0. ),	Y( viewGrp, .175 ),	W( viewGrp, 1. ),	H( viewGrp, .725 ) },

	moveGrp{	X( *this, 0. ),		vOff( viewGrp ),	W( *this, 1. ),		H( *this, .165 ) },
	xRot{		X( moveGrp, .1 ),	Y( moveGrp, .0 ),	W( moveGrp, .3 ),	H( moveGrp, .25 ), "x-Rotation" },
	yRot{		X( moveGrp, .1 ),	Y( moveGrp, .5 ),	W( moveGrp, .3 ),	H( moveGrp, .25 ), "y-Rotation" },
	zTrans{		X( moveGrp, .5 ),	Y( moveGrp, .0 ),	W( moveGrp, .3 ),	H( moveGrp, .25 ), "z-Translation" },

	load_model{ *this, &ModelView::loadModel },
	update_model_{ *this, &ModelView::UpdateModel },
	reset_model_{ *this, &ModelView::resetModel }

{

	
	Fl_Group::add( title );
	title.box( FL_NO_BOX ); title.align( FL_ALIGN_CENTER ); title.labelsize( 30 );


	//--------------------------- Head ---------------------------//

	Fl_Group::add( headGrp );

	// Labelsize and callback
	headGrp.add( loadBtn );
	loadBtn.labelsize( (int) ( .5 * (double) loadBtn.h() ) );
	loadBtn.callback( HandleCallback<ModelView>, &load_model );
	




	//--------------------------- View ---------------------------//

	// View group dictates resizing
	Fl_Group::add_resizable( viewGrp );
	viewGrp.add( modelData );
	viewGrp.add( viewBox );
	viewGrp.add( viewImg );

	viewGrp.add( resetBtn );
	resetBtn.labelsize( (int) ( .6 * (double) resetBtn.h() ) );
	resetBtn.callback( HandleCallback<ModelView>, &reset_model_ );

	// Model data_
	modelData.hide();

	// Labelsize and box
	viewBox.labelsize( (int) ( .05 * (double) viewBox.h() ) );

	// Hide image initially
	viewGrp.resizable( viewBox );
	viewImg.hide();


	//--------------------------- Movement ---------------------------//

	Fl_Group::add( moveGrp );
	moveGrp.add( xRot );
	moveGrp.add( yRot );
	moveGrp.add( zTrans );

	// Counter ranges
	xRot.range( -180., 180. );		xRot.step( 1., 10. );
	yRot.range( -180., 180. );		yRot.step( 1., 10. );
	zTrans.range( -500., 500. );	zTrans.step( 1., 10. );

	// Labelsizes
	xRot.labelsize( (int) ( .50 * (double) xRot.h() ) );
	yRot.labelsize( (int) ( .50 * (double) yRot.h() ) );
	zTrans.labelsize( (int) ( .50 * (double) zTrans.h() ) );


	// Callbacks for Counters and reset button
	xRot.callback( HandleCallback<ModelView>, &update_model_ );
	yRot.callback( HandleCallback<ModelView>, &update_model_ );
	zTrans.callback( HandleCallback<ModelView>, &update_model_ );


	// Set values
	xRot.value(modelViewPara.slice_plane.rotation_angle_x );
	yRot.value( modelViewPara.slice_plane.rotation_angle_y );
	zTrans.value( modelViewPara.slice_plane.position_z );

	// Hide initially
	moveGrp.hide();

	
	if( IsModelLoaded() ){
		UpdateModel();
		viewImg.SetSliderBounds( model_.attenuationRange() );
		viewImg.ChangeSliderValues( modelViewPara.contrast );
	}
}

ModelView::~ModelView( void ){
	storedModel.Save();
	storedModelChooser.Save();
	storedViewParameter.Save();
};


string ModelView::modelDescription( void ) const{

	string modelDataString;

	modelDataString.clear();
	modelDataString += "Name: \t" + model_.name() + '\n';
	modelDataString += "Voxel: \t\t\t" + ToString( model_.number_of_voxel_3D().x ) + " x " + ToString( model_.number_of_voxel_3D().y ) + " x " + ToString( model_.number_of_voxel_3D().z ) + "\n";
	modelDataString += "Voxel Größe: \t" + ToString( model_.voxel_size().x, 2 ) + " x " + ToString( model_.voxel_size().y, 2 ) + " x " + ToString( model_.voxel_size().z, 2 ) + "  mm^3\n";
	modelDataString += "Model Größe: \t" + ToString( model_.size().x ) + " x " + ToString( model_.size().y ) + " x " + ToString( model_.size().z ) + "  mm^3";

	return modelDataString;
}

bool ModelView::moveModel( double& targetXRot, double& targetYRot, double& targetZTrans ){

	const SlicePlane backupPlane = modelViewPara.slice_plane; 
	SlicePlane& planeInstance =  modelViewPara.slice_plane;

	const PrimitiveCoordinateSystem backupCSys = model_.coordinate_system()->GetPrimitive();

	if( targetXRot != planeInstance.rotation_angle_x ){

		const double rotationAngle = targetXRot - planeInstance.rotation_angle_x;
		planeInstance.rotation_angle_x = targetXRot;

		const Line axis{ planeInstance.surface.direction_1(), planeInstance.surface.origin() };

		model_.coordinate_system()->Rotate( axis, rotationAngle / 360. * 2. * PI );
	}

	if( targetYRot != planeInstance.rotation_angle_y ){

		const double rotationAngle = targetYRot - planeInstance.rotation_angle_y;
		planeInstance.rotation_angle_y = targetYRot;

		const Line axis{ planeInstance.surface.direction_2(), planeInstance.surface.origin() };

		model_.coordinate_system()->Rotate( axis, rotationAngle / 360. * 2. * PI );
	}

	if( targetZTrans != planeInstance.position_z ){

		const double translation = targetZTrans - planeInstance.position_z;
		planeInstance.position_z = targetZTrans;

		model_.coordinate_system()->Translate( ( (Vector3D) planeInstance.surface.GetNormal() ) * translation );
	}

	// Return if succeeded
	if( sliceModel() ) return true;
	
	// Revert changes
	planeInstance = backupPlane;
	model_.coordinate_system()->SetPrimitive( backupCSys );

	targetXRot = planeInstance.rotation_angle_x;
	targetYRot = planeInstance.rotation_angle_y;
	targetZTrans = planeInstance.position_z;


	return false;
}


bool ModelView::sliceModel( void ){
	Fl_Group::window()->deactivate();
	
	storedViewParameter.SetAsLoaded();

	DataGrid<VoxelData> tempSlice = model_.GetSlice(  modelViewPara.slice_plane.surface, 1. );
	
	if( tempSlice.size().c == 0 || tempSlice.size().r == 0 )
	{
	
		Fl_Group::window()->activate();
		return false;
	}

	modelSliceInstance = tempSlice;

	
	return true;
}


void ModelView::centerModel( void ){

	// Center model
	Tuple3D center = PrimitiveVector3{ model_.size() } / -2.;

	model_.coordinate_system()->SetPrimitive( PrimitiveCoordinateSystem{ center, Tuple3D{1,0,0}, Tuple3D{0,1,0}, Tuple3D{0,0,1} } );
}


void ModelView::resetModel( void ){

	this->window()->deactivate();
		modelViewPara.slice_plane.rotation_angle_x = 0.;
	 modelViewPara.slice_plane.rotation_angle_y = 0.;
	 modelViewPara.slice_plane.position_z = 0.;

	centerModel();

	xRot.value( 0. );
	yRot.value( 0. );
	zTrans.value( 0. );

	sliceModel();
	viewImg.AssignImage( modelSliceInstance );
	modelViewPara.contrast = viewImg.GetContrast();

	this->window()->activate();

}

void ModelView::loadModel( void ){

	this->window()->deactivate();

	viewImg.hide(); viewBox.show(); modelData.hide();
	viewBox.label( "Loading model..." );

	path modelToLoad = modelChooserInstance.ChooseFile();
	storedModelChooser.SetAsLoaded();

	if( !storedModel.Load( modelToLoad ) ){
		viewBox.label( "Loading failed!" );
		this->window()->activate();
		return;
	}

	viewImg.SetSliderBounds( model_.attenuationRange() );
	modelViewPara.contrast = viewImg.GetContrast();

	resetModel();
	UpdateModel();


	viewImg.show(); viewBox.hide(); modelData.show();
	moveGrp.show();


	
	// Get necessary distance range
	
	// Top side
	GridIndex top_corner{ 0, 0 };
	for( size_t row_index = 0; row_index < modelSliceInstance.size().r; row_index++ ){
		for( size_t column_index = 0; column_index < modelSliceInstance.size().c; column_index++ ){
			const GridIndex grid_index{ column_index, row_index };
			if( !modelSliceInstance.GetData( grid_index ).HasSpecificProperty( VoxelData::SpecialProperty::Undefined ) ){
				top_corner = grid_index;																// Set index
				column_index = modelSliceInstance.size().c; row_index = modelSliceInstance.size().r;	// Break condition
			}
		}
	}
	
	// Right side
	GridIndex right_corner{ modelSliceInstance.size().c - 1, 0 };
	for( size_t column_index = modelSliceInstance.size().c; column_index > 0; column_index-- ){
		for( size_t row_index = 0; row_index < modelSliceInstance.size().r; row_index++ ){
			const GridIndex grid_index{ column_index - 1, row_index };
			if( !modelSliceInstance.GetData( grid_index ).HasSpecificProperty( VoxelData::SpecialProperty::Undefined ) ){
				right_corner = grid_index;																// Set index
				column_index = 1; row_index = modelSliceInstance.size().r;								// Break condition
			}
		}
	}
	
	// Bottom side
	GridIndex bottom_corner{ modelSliceInstance.size().c - 1, modelSliceInstance.size().r - 1 };
	for( size_t row_index = modelSliceInstance.size().r; row_index > 0; row_index-- ){
		for( size_t column_index = modelSliceInstance.size().c; column_index > 0; column_index-- ){
			const GridIndex grid_index{ column_index - 1, row_index - 1 };
			if( !modelSliceInstance.GetData( grid_index ).HasSpecificProperty( VoxelData::SpecialProperty::Undefined ) ){
				bottom_corner = grid_index;																// Set index
				column_index = 1; row_index = 1;														// Break condition
			}
		}
	}

	// Left side
	GridIndex left_corner{ 0, modelSliceInstance.size().r - 1 };
	for( size_t column_index = 0; column_index < modelSliceInstance.size().c; column_index++ ){
		for( size_t row_index = modelSliceInstance.size().r; row_index > 0; row_index-- ){
			const GridIndex grid_index{ column_index, row_index - 1 };
			if( !modelSliceInstance.GetData( grid_index ).HasSpecificProperty( VoxelData::SpecialProperty::Undefined ) ){
				left_corner = grid_index;																// Set index
				column_index = modelSliceInstance.size().c; row_index = 1;								// Break condition
			}
		}
	}

	const GridCoordinates top_left =		modelSliceInstance.GetCoordinates( top_corner    );
	const GridCoordinates top_right =		modelSliceInstance.GetCoordinates( right_corner  );
	const GridCoordinates bottom_left =		modelSliceInstance.GetCoordinates( left_corner   );
	const GridCoordinates bottom_right =	modelSliceInstance.GetCoordinates( bottom_corner );

	const double maximum_distance_from_origin = Max( Max(	sqrt( pow( top_left.c,		2. )	+ pow( top_left.r,		2. ) ),
															sqrt( pow( top_right.c,		2. )	+ pow( top_right.r,		2. ) ) ), 
													Max(	sqrt( pow( bottom_left.c,	2. )	+ pow( bottom_left.r,	2. ) ),
															sqrt( pow( bottom_right.c,	2. )	+ pow( bottom_right.r,	2. ) ) ) );

	main_window_.gantryBuild.SetDistances( maximum_distance_from_origin );


	this->window()->activate();
	
	main_window_.gantryBuild.activate();
	main_window_.tomographyExecution.activate();
}



void ModelView::UpdateModel( void ){

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
		modelViewPara.contrast = viewImg.GetContrast();
	}

	viewImg.show(); viewBox.hide(); modelData.show();
	moveGrp.show();

	modelDataString = modelDescription();
	modelData.value( modelDataString.c_str() );
	this->window()->activate();

}
