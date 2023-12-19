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

#include "fl_ModelView.h"
#include "serialization.h"
#include "programState.h"
#include "fl_MainWindow.h"


Fl_ModelView::Fl_ModelView( int x, int y, int w, int h, Fl_MainWindow& main_window ) :
	Fl_Group{ x, y, w, h },
	
	title_{			X( *this, 0. ),		Y( *this, 0. ),		W( *this, 1. ),		H( *this, 0.035 ),	"Model" },

	head_group_{	X( *this, 0. ),		Y( *this, 0.04 ),		W( *this, 1. ),		H( *this, .05 ) },
	load_model_button_{	X( head_group_, .1 ),	Y( head_group_, .05 ),	W( head_group_, .25 ),	H( head_group_, .9 ),	"Load model" },

	model_inspection_group_{	X( *this, 0. ),		vOff( head_group_ ),	W( *this , 1. ),	H( *this, .75 ) },
	model_information_{	X( model_inspection_group_, 0.05 ),	Y( model_inspection_group_, 0.01 ),	W( model_inspection_group_, .45 ),	H( model_inspection_group_, .15 ) },
	reset_model_button_{	X( model_inspection_group_, .6 ),	Y( model_inspection_group_, .06 ),	W( model_inspection_group_, .25 ),	H( model_inspection_group_, .05 ), "Reset model" },
	loading_status_{	X( model_inspection_group_, 0. ),	Y( model_inspection_group_, .2 ),	W( model_inspection_group_, 1. ),	H( model_inspection_group_, .725 ),	"No model loaded" },
	model_slice_image_{	X( model_inspection_group_, 0. ),	Y( model_inspection_group_, .2 ),	W( model_inspection_group_, 1. ),	H( model_inspection_group_, .725 ) },

	model_movement_group_{	X( *this, 0. ),		vOff( model_inspection_group_ ),	W( *this, 1. ),		H( *this, .165 ) },
	x_rotation_{		X( model_movement_group_, .1 ),	Y( model_movement_group_, .0 ),	W( model_movement_group_, .3 ),	H( model_movement_group_, .25 ), "x-Rotation" },
	y_rotation_{		X( model_movement_group_, .1 ),	Y( model_movement_group_, .5 ),	W( model_movement_group_, .3 ),	H( model_movement_group_, .25 ), "y-Rotation" },
	z_position_{		X( model_movement_group_, .5 ),	Y( model_movement_group_, .0 ),	W( model_movement_group_, .3 ),	H( model_movement_group_, .25 ), "z-Translation" },
	artefact_impact_{	X( model_movement_group_, .5 ),	Y( model_movement_group_, .5 ),	W( model_movement_group_, .3 ),	H( model_movement_group_, .25 ), "Artefact factor" },

	
	properties_{ ModelViewProperties{}, "saved.viewproperties" },
	main_window_( main_window ),

	model_chooser_{ FileChooser{ "Choose CT model", "*.model", path{ "./" } }, "model.chooser" },
	model_{ Model{}, "saved.model" },
	model_slice_{},
	

	load_model_callback_{ *this, &Fl_ModelView::LoadModel },
	update_model_callback_{ *this, &Fl_ModelView::UpdateModel },
	reset_model_callback_{ *this, &Fl_ModelView::ResetModel },
	update_artefact_impact_{ *this, &Fl_ModelView::UpdateArtefactImpact }

{

	Fl_Group::add( title_ );
	title_.box( FL_NO_BOX ); title_.align( FL_ALIGN_CENTER ); title_.labelsize( 30 );


	//--------------------------- Head ---------------------------//

	Fl_Group::add( head_group_ );

	// Labelsize and callback
	head_group_.add( load_model_button_ );
	load_model_button_.labelsize( static_cast<int>( .5 * static_cast<double>( load_model_button_.h() ) ) );
	load_model_button_.callback( CallbackFunction<Fl_ModelView>::Fl_Callback, &load_model_callback_ );
	




	//--------------------------- View ---------------------------//

	// View group dictates resizing
	Fl_Group::add_resizable( model_inspection_group_ );
	model_inspection_group_.add( model_information_ );
	model_inspection_group_.add( loading_status_ );
	model_inspection_group_.add( model_slice_image_ );

	model_inspection_group_.add( reset_model_button_ );
	reset_model_button_.labelsize( static_cast<int>( .6 * static_cast<double>( reset_model_button_.h() ) ) );
	reset_model_button_.callback( CallbackFunction<Fl_ModelView>::Fl_Callback, &reset_model_callback_ );

	// Model data_
	model_information_.hide();

	// Labelsize and box
	loading_status_.labelsize( static_cast<int>( .05 * static_cast<double>( loading_status_.h() ) ) );

	model_slice_image_.SetValueTip( "µ_a at " + ToString( reference_energy_for_mu_eV/1000, 0 ) + "keV\n In mm^-1");
	model_slice_image_.Setlabel( { "x in mm", " y in mm" } ); 

	// Hide image initially
	model_inspection_group_.resizable( loading_status_ );
	model_slice_image_.hide();


	//--------------------------- Movement ---------------------------//

	Fl_Group::add( model_movement_group_ );
	model_movement_group_.add( x_rotation_ );
	model_movement_group_.add( y_rotation_ );
	model_movement_group_.add( z_position_ );
	model_movement_group_.add( artefact_impact_ );

	// Counter ranges
	x_rotation_.range( -180., 180. );		x_rotation_.step( 1., 10. );
	y_rotation_.range( -180., 180. );		y_rotation_.step( 1., 10. );
	z_position_.range( -500., 500. );		z_position_.step( 1., 10. );
	artefact_impact_.range( 0. , 10. );		artefact_impact_.step( 0.01, 0.1 );

	// Labelsizes
	x_rotation_.labelsize( static_cast<int>( .50 * static_cast<double>( x_rotation_.h() ) ) );
	y_rotation_.labelsize( static_cast<int>( .50 * static_cast<double>( y_rotation_.h() ) ) );
	z_position_.labelsize( static_cast<int>( .50 * static_cast<double>( z_position_.h() ) ) );
	artefact_impact_.labelsize( static_cast<int>( .50 * static_cast<double>( artefact_impact_.h() ) ) );


	// Callbacks for Counters and reset button
	x_rotation_.callback( CallbackFunction<Fl_ModelView>::Fl_Callback, &update_model_callback_ );
	y_rotation_.callback( CallbackFunction<Fl_ModelView>::Fl_Callback, &update_model_callback_ );
	z_position_.callback( CallbackFunction<Fl_ModelView>::Fl_Callback, &update_model_callback_ );
	artefact_impact_.callback( CallbackFunction<Fl_ModelView>::Fl_Callback, &update_artefact_impact_ );
	artefact_impact_.tooltip("Values from 0 to 10. When at 0 a metal artifact\nis treated as having the same absorption as water.\nAt 10 it beheaves like real iron.");

	// Set values
	x_rotation_.value(properties_.slice_plane.rotation_angle_x );
	y_rotation_.value( properties_.slice_plane.rotation_angle_y );
	z_position_.value( properties_.slice_plane.position_z );
	artefact_impact_.value( properties_.artefact_impact );
	VoxelData::SetArtefactImpactFactor( artefact_impact_.value() );	

	// Hide initially
	model_movement_group_.hide();

	
	if( IsModelLoaded() ){
		model_slice_image_.SetSliderBounds( model_.absorptionRange() );
		model_slice_image_.ChangeSliderValues( properties_.contrast );
		UpdateModel();
	}
}

string Fl_ModelView::GetModelDescription( void ) const{

	string modelDataString;

	modelDataString.clear();
	modelDataString += "Name: \t" + model_.name() + '\n';
	modelDataString += "Voxel: \t\t\t" + ToString( model_.number_of_voxel_3D().x ) + " x " + ToString( model_.number_of_voxel_3D().y ) + " x " + ToString( model_.number_of_voxel_3D().z ) + "\n";
	modelDataString += "Voxel Größe: \t" + ToString( model_.voxel_size().x, 2 ) + " x " + ToString( model_.voxel_size().y, 2 ) + " x " + ToString( model_.voxel_size().z, 2 ) + "  mm^3\n";
	modelDataString += "Model Größe: \t" + ToString( model_.size().x ) + " x " + ToString( model_.size().y ) + " x " + ToString( model_.size().z ) + "  mm^3";

	return modelDataString;
}

bool Fl_ModelView::MoveModel( double& targetXRot, double& targetYRot, double& targetZTrans ){

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
	if( SliceModel() ) return true;
	
	// Revert changes
	planeInstance = backupPlane;
	model_.coordinate_system()->SetPrimitive( backupCSys );

	targetXRot = planeInstance.rotation_angle_x;
	targetYRot = planeInstance.rotation_angle_y;
	targetZTrans = planeInstance.position_z;


	return false;
}


bool Fl_ModelView::SliceModel( void ){
	Fl_Group::window()->deactivate();
	
	properties_.SetAsLoaded();

	DataGrid<VoxelData> tempSlice = model_.GetSlice(  properties_.slice_plane.surface, { static_cast<size_t>( model_slice_image_.w() ), static_cast<size_t>( model_slice_image_.h() ) } );
	
	if( tempSlice.size().c == 0 || tempSlice.size().r == 0 ){
		Fl_Group::window()->activate();
		return false;
	}

	model_slice_ = tempSlice;

	return true;
}


void Fl_ModelView::CenterModel( void ){

	// Center model
	Tuple3D center = PrimitiveVector3{ model_.size() } / -2.;

	model_.coordinate_system()->SetPrimitive( PrimitiveCoordinateSystem{ center, Tuple3D{ 1,0,0 }, Tuple3D{ 0, 1, 0 }, Tuple3D{ 0 ,0 ,1} } );

}


void Fl_ModelView::ResetModel( void ){

	this->window()->deactivate();
	properties_.slice_plane.rotation_angle_x = 0.;
	properties_.slice_plane.rotation_angle_y = 0.;
	properties_.slice_plane.position_z = 0.;

	CenterModel();

	x_rotation_.value( 0. );
	y_rotation_.value( 0. );
	z_position_.value( 0. );

	SliceModel();
	model_slice_image_.AssignImage( model_slice_ );
	properties_.contrast = model_slice_image_.GetContrast();

	this->window()->activate();

}

void Fl_ModelView::LoadModel( void ){

	this->window()->deactivate();

	model_slice_image_.hide(); loading_status_.show(); model_information_.hide();
	loading_status_.label( "Loading model..." );

	path modelToLoad = model_chooser_.ChooseFile();
	model_chooser_.SetAsLoaded();

	if( !model_.Load( modelToLoad ) ){
		loading_status_.label( "Loading failed!" );
		this->window()->activate();
		return;
	}

	model_slice_image_.SetSliderBounds( model_.absorptionRange() );
	properties_.contrast = model_slice_image_.GetContrast();

	ResetModel();
	UpdateModel();


	model_slice_image_.show(); loading_status_.hide(); model_information_.show();
	model_movement_group_.show();


	
	// Get necessary distance range
	
	// Top side
	GridIndex top_corner{ 0, 0 };
	for( size_t row_index = 0; row_index < model_slice_.size().r; row_index++ ){
		for( size_t column_index = 0; column_index < model_slice_.size().c; column_index++ ){
			const GridIndex grid_index{ column_index, row_index };
			if( !model_slice_.GetData( grid_index ).HasSpecificProperty( VoxelData::SpecialProperty::Undefined ) ){
				top_corner = grid_index;																// Set index
				column_index = model_slice_.size().c; row_index = model_slice_.size().r;	// Break condition
			}
		}
	}
	
	// Right side
	GridIndex right_corner{ model_slice_.size().c - 1, 0 };
	for( size_t column_index = model_slice_.size().c; column_index > 0; column_index-- ){
		for( size_t row_index = 0; row_index < model_slice_.size().r; row_index++ ){
			const GridIndex grid_index{ column_index - 1, row_index };
			if( !model_slice_.GetData( grid_index ).HasSpecificProperty( VoxelData::SpecialProperty::Undefined ) ){
				right_corner = grid_index;																// Set index
				column_index = 1; row_index = model_slice_.size().r;								// Break condition
			}
		}
	}
	
	// Bottom side
	GridIndex bottom_corner{ model_slice_.size().c - 1, model_slice_.size().r - 1 };
	for( size_t row_index = model_slice_.size().r; row_index > 0; row_index-- ){
		for( size_t column_index = model_slice_.size().c; column_index > 0; column_index-- ){
			const GridIndex grid_index{ column_index - 1, row_index - 1 };
			if( !model_slice_.GetData( grid_index ).HasSpecificProperty( VoxelData::SpecialProperty::Undefined ) ){
				bottom_corner = grid_index;																// Set index
				column_index = 1; row_index = 1;														// Break condition
			}
		}
	}

	// Left side
	GridIndex left_corner{ 0, model_slice_.size().r - 1 };
	for( size_t column_index = 0; column_index < model_slice_.size().c; column_index++ ){
		for( size_t row_index = model_slice_.size().r; row_index > 0; row_index-- ){
			const GridIndex grid_index{ column_index, row_index - 1 };
			if( !model_slice_.GetData( grid_index ).HasSpecificProperty( VoxelData::SpecialProperty::Undefined ) ){
				left_corner = grid_index;																// Set index
				column_index = model_slice_.size().c; row_index = 1;								// Break condition
			}
		}
	}

	const GridCoordinates top_left =		model_slice_.GetCoordinates( top_corner    );
	const GridCoordinates top_right =		model_slice_.GetCoordinates( right_corner  );
	const GridCoordinates bottom_left =		model_slice_.GetCoordinates( left_corner   );
	const GridCoordinates bottom_right =	model_slice_.GetCoordinates( bottom_corner );

	const double maximum_distance_from_origin = Max( Max(	sqrt( pow( top_left.c,		2. )	+ pow( top_left.r,		2. ) ),
															sqrt( pow( top_right.c,		2. )	+ pow( top_right.r,		2. ) ) ), 
													Max(	sqrt( pow( bottom_left.c,	2. )	+ pow( bottom_left.r,	2. ) ),
															sqrt( pow( bottom_right.c,	2. )	+ pow( bottom_right.r,	2. ) ) ) );

	main_window_.gantry_creation_.SetDistances( maximum_distance_from_origin );


	this->window()->activate();
	
	main_window_.gantry_creation_.activate();
	main_window_.tomography_execution_.activate();
}



void Fl_ModelView::UpdateModel( void ){

	this->window()->deactivate();


	// Store in variable for moveModel function call
	double oldXRot = x_rotation_.value();
	double oldYRot = y_rotation_.value();
	double oldZTrans = z_position_.value();

	// Movement did not succeed?
	if( !MoveModel( oldXRot, oldYRot, oldZTrans ) ){
		// moveModel changes arguments to previous value
		x_rotation_.value( oldXRot );
		y_rotation_.value( oldYRot );
		z_position_.value( oldZTrans );
	}
	else{
		// New assignment only necessary, when movement succeeded
		model_slice_image_.AssignImage( model_slice_ );
		properties_.contrast = model_slice_image_.GetContrast();
	}

	model_slice_image_.SetAxis( { model_slice_.start().c, -model_slice_.start().r }, { model_slice_.resolution().c, -model_slice_.resolution().r}, {7,7});
	model_slice_image_.show(); loading_status_.hide(); model_information_.show();
	model_movement_group_.show();

	model_information_string_ = GetModelDescription();
	model_information_.value( model_information_string_.c_str() );
	this->window()->activate();

}

void Fl_ModelView::UpdateArtefactImpact( void ){
	properties_.artefact_impact = artefact_impact_.value();
	VoxelData::SetArtefactImpactFactor( artefact_impact_.value() );	
}