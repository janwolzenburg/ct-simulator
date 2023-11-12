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
#include "programState.h"
#include "mainWindow.h"


ModelView::ModelView( int x, int y, int w, int h, mainWindow& main_window ) :
	
	Fl_Group{ x, y, w, h },
	
	title_{			X( *this, 0. ),		Y( *this, 0. ),		W( *this, 1. ),		H( *this, 0.035 ),	"Model" },

	head_group_{	X( *this, 0. ),		Y( *this, 0.04 ),		W( *this, 1. ),		H( *this, .05 ) },
	load_model_button_{	X( head_group_, .1 ),	Y( head_group_, .05 ),	W( head_group_, .25 ),	H( head_group_, .9 ),	"Load model" },

	model_inspection_group_{	X( *this, 0. ),		vOff( head_group_ ),	W( *this , 1. ),	H( *this, .75 ) },
	model_information_{	X( model_inspection_group_, 0. ),	Y( model_inspection_group_, 0. ),	W( model_inspection_group_, .5 ),	H( model_inspection_group_, .15 ) },
	reset_model_button_{	X( model_inspection_group_, .6 ),	Y( model_inspection_group_, .06 ),	W( model_inspection_group_, .25 ),	H( model_inspection_group_, .05 ), "Reset model" },

	loading_status_{	X( model_inspection_group_, 0. ),	Y( model_inspection_group_, .175 ),	W( model_inspection_group_, 1. ),	H( model_inspection_group_, .725 ),	"No model loaded" },
	model_slice_image_{	X( model_inspection_group_, 0. ),	Y( model_inspection_group_, .175 ),	W( model_inspection_group_, 1. ),	H( model_inspection_group_, .725 ) },

	model_movement_group_{	X( *this, 0. ),		vOff( model_inspection_group_ ),	W( *this, 1. ),		H( *this, .165 ) },
	x_rotation_{		X( model_movement_group_, .1 ),	Y( model_movement_group_, .0 ),	W( model_movement_group_, .3 ),	H( model_movement_group_, .25 ), "x-Rotation" },
	y_rotation_{		X( model_movement_group_, .1 ),	Y( model_movement_group_, .5 ),	W( model_movement_group_, .3 ),	H( model_movement_group_, .25 ), "y-Rotation" },
	z_position_{		X( model_movement_group_, .5 ),	Y( model_movement_group_, .0 ),	W( model_movement_group_, .3 ),	H( model_movement_group_, .25 ), "z-Translation" },

	main_window_( main_window ),

	storedModelChooser{ FileChooser{ "Choose CT model", "*.model", path{ "./" } }, "storedModelChooser.model" },
	model_{ Model{}, "storedModel.txt" },
	properties_{ ModelViewProperties{}, "storedViewParameter.model" },
	modelSliceInstance{},
	

	load_model{ *this, &ModelView::loadModel },
	update_model_{ *this, &ModelView::UpdateModel },
	reset_model_{ *this, &ModelView::resetModel }

{

	Fl_Group::add( title_ );
	title_.box( FL_NO_BOX ); title_.align( FL_ALIGN_CENTER ); title_.labelsize( 30 );


	//--------------------------- Head ---------------------------//

	Fl_Group::add( head_group_ );

	// Labelsize and callback
	head_group_.add( load_model_button_ );
	load_model_button_.labelsize( (int) ( .5 * (double) load_model_button_.h() ) );
	load_model_button_.callback( CallbackFunction<ModelView>::Fl_Callback, &load_model );
	




	//--------------------------- View ---------------------------//

	// View group dictates resizing
	Fl_Group::add_resizable( model_inspection_group_ );
	model_inspection_group_.add( model_information_ );
	model_inspection_group_.add( loading_status_ );
	model_inspection_group_.add( model_slice_image_ );

	model_inspection_group_.add( reset_model_button_ );
	reset_model_button_.labelsize( (int) ( .6 * (double) reset_model_button_.h() ) );
	reset_model_button_.callback( CallbackFunction<ModelView>::Fl_Callback, &reset_model_ );

	// Model data_
	model_information_.hide();

	// Labelsize and box
	loading_status_.labelsize( (int) ( .05 * (double) loading_status_.h() ) );

	// Hide image initially
	model_inspection_group_.resizable( loading_status_ );
	model_slice_image_.hide();


	//--------------------------- Movement ---------------------------//

	Fl_Group::add( model_movement_group_ );
	model_movement_group_.add( x_rotation_ );
	model_movement_group_.add( y_rotation_ );
	model_movement_group_.add( z_position_ );

	// Counter ranges
	x_rotation_.range( -180., 180. );		x_rotation_.step( 1., 10. );
	y_rotation_.range( -180., 180. );		y_rotation_.step( 1., 10. );
	z_position_.range( -500., 500. );	z_position_.step( 1., 10. );

	// Labelsizes
	x_rotation_.labelsize( (int) ( .50 * (double) x_rotation_.h() ) );
	y_rotation_.labelsize( (int) ( .50 * (double) y_rotation_.h() ) );
	z_position_.labelsize( (int) ( .50 * (double) z_position_.h() ) );


	// Callbacks for Counters and reset button
	x_rotation_.callback( CallbackFunction<ModelView>::Fl_Callback, &update_model_ );
	y_rotation_.callback( CallbackFunction<ModelView>::Fl_Callback, &update_model_ );
	z_position_.callback( CallbackFunction<ModelView>::Fl_Callback, &update_model_ );


	// Set values
	x_rotation_.value(properties_.slice_plane.rotation_angle_x );
	y_rotation_.value( properties_.slice_plane.rotation_angle_y );
	z_position_.value( properties_.slice_plane.position_z );

	// Hide initially
	model_movement_group_.hide();

	
	if( IsModelLoaded() ){
		UpdateModel();
		model_slice_image_.SetSliderBounds( model_.attenuationRange() );
		model_slice_image_.ChangeSliderValues( properties_.contrast );
	}
}

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

	const SlicePlane backupPlane = properties_.slice_plane; 
	SlicePlane& planeInstance =  properties_.slice_plane;

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
	
	properties_.SetAsLoaded();

	DataGrid<VoxelData> tempSlice = model_.GetSlice(  properties_.slice_plane.surface, 1. );
	
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
		properties_.slice_plane.rotation_angle_x = 0.;
	 properties_.slice_plane.rotation_angle_y = 0.;
	 properties_.slice_plane.position_z = 0.;

	centerModel();

	x_rotation_.value( 0. );
	y_rotation_.value( 0. );
	z_position_.value( 0. );

	sliceModel();
	model_slice_image_.AssignImage( modelSliceInstance );
	properties_.contrast = model_slice_image_.GetContrast();

	this->window()->activate();

}

void ModelView::loadModel( void ){

	this->window()->deactivate();

	model_slice_image_.hide(); loading_status_.show(); model_information_.hide();
	loading_status_.label( "Loading model..." );

	path modelToLoad = storedModelChooser.ChooseFile();
	storedModelChooser.SetAsLoaded();

	if( !model_.Load( modelToLoad ) ){
		loading_status_.label( "Loading failed!" );
		this->window()->activate();
		return;
	}

	model_slice_image_.SetSliderBounds( model_.attenuationRange() );
	properties_.contrast = model_slice_image_.GetContrast();

	resetModel();
	UpdateModel();


	model_slice_image_.show(); loading_status_.hide(); model_information_.show();
	model_movement_group_.show();


	
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
	double oldXRot = x_rotation_.value();
	double oldYRot = y_rotation_.value();
	double oldZTrans = z_position_.value();

	// Movement did not succeed?
	if( !moveModel( oldXRot, oldYRot, oldZTrans ) ){
		// moveModel changes arguments to previous value
		x_rotation_.value( oldXRot );
		y_rotation_.value( oldYRot );
		z_position_.value( oldZTrans );
	}
	else{
		// New assignment only necessary, when movement succeeded
		model_slice_image_.AssignImage( modelSliceInstance );//, false );
		properties_.contrast = model_slice_image_.GetContrast();
	}

	model_slice_image_.show(); loading_status_.hide(); model_information_.show();
	model_movement_group_.show();

	model_information_string_ = modelDescription();
	model_information_.value( model_information_string_.c_str() );
	this->window()->activate();

}
