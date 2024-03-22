/*********************************************************************
 * @file   fl_ModelCreation.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   November 2023
 *********************************************************************/


  /*********************************************************************
	Includes
 *********************************************************************/

 #include "fl_ModelCreator.h"
 #include "FL/Fl.H"

 const std::map<Fl_ModelFeature::Shape, string> Fl_ModelFeature::shape_names{
	{ Sphere,	"Sphere" },
	{ Cube,		"Cube" } 
};


 Fl_ModelFeature::Shape Fl_ModelFeature::GetShapeEnum( const string shape_string ){

	for( auto& [matEnum, value] : Fl_ModelFeature::shape_names ){
		if( shape_string == value )
			return matEnum;
		}

	return Fl_ModelFeature::Shape::Sphere;
}


Fl_ModelFeature::Fl_ModelFeature( int x, int y, int w, int h, const char* label ) :
	Fl_Group{ x, y, w, h, label },
	active_button_{				X( *this, .025 ),						Y( *this, .125 ),		H( *this, .75 ),		H( *this, .75 ) },
	special_property_input_{	hOff( active_button_ ) + 10,			Y( *this, .1 ),			W( *this, .15 ),		H( *this, .8 ) },
	value_input_{				hOff( special_property_input_ ) + 30,	Y( *this, 0.05 ),		W( *this, .105 ),			H( *this, .9 ), "µ: " },
	x_positon_input_{			hOff( value_input_ ) + 30,				Y( *this, 0.05 ),		W( *this, .085 ),			H( *this, .9 ), "x: " },
	y_positon_input_{			hOff( x_positon_input_ ) + 30,			Y( *this, 0.05 ),		W( *this, .085 ),			H( *this, .9 ), "y: " },
	z_positon_input_{			hOff( y_positon_input_ ) + 30,			Y( *this, 0.05 ),		W( *this, .085 ),			H( *this, .9 ), "z: " },
	size_input_{				hOff( z_positon_input_ ) + 50,			Y( *this, 0.05 ),		W( *this, .085 ),			H( *this, .9 ), "Size: " },
	shape_input_{				hOff( size_input_ ) + 20,				Y( *this, 0.125 ),		W( *this, .125 ),		H( *this, .75 ) },
	is_visible_( true )
{
	Fl_Group::add( active_button_ );		
	active_button_.color( FL_DARK_RED, FL_DARK_GREEN );
	active_button_.tooltip( "Activate feature.");



	Fl_Group::add( special_property_input_ );

	vector<string> property_names;
	for( auto& el : VoxelData::special_property_names ) property_names.push_back( el.second );
	property_names.pop_back();
	special_property_input_.AssignElements( property_names );
	special_property_input_.SetCurrentElement( VoxelData::special_property_names.at( VoxelData::SpecialProperty::None ) );
	special_property_input_.tooltip( "Select if the model feature has a special property.");
		
	Fl_Group::add( value_input_ ); value_input_.align( FL_ALIGN_LEFT );
	value_input_.copy_tooltip( string{ "Absorption coefficient at " + ToString<int>( static_cast<int>( reference_energy_for_mu_eV ) / 1000 ) + "keV in mm^-1 "}.c_str() );
	value_input_.SetProperties( 0., 100., 5, None );
	value_input_.value( absorption_water_Per_mm );

	Fl_Group::add( x_positon_input_ ); Fl_Group::add( y_positon_input_ ); Fl_Group::add( z_positon_input_ );
	x_positon_input_.align( FL_ALIGN_LEFT ); x_positon_input_.SetProperties( .001, 1000., 3, None );
	y_positon_input_.align( FL_ALIGN_LEFT ); y_positon_input_.SetProperties( .001, 1000., 3, None );
	z_positon_input_.align( FL_ALIGN_LEFT ); z_positon_input_.SetProperties( .001, 1000., 3, None );
	x_positon_input_.tooltip( "x-coordinate of feature center in mm");
	y_positon_input_.tooltip( "y-coordinate of feature center in mm");
	z_positon_input_.tooltip( "z-coordinate of feature center in mm");

	Fl_Group::add( size_input_ ); size_input_.align( FL_ALIGN_LEFT ); 
	size_input_.SetProperties( .001, 1000., 3, None );
	size_input_.tooltip( "Size of feature in mm. ");
		

	Fl_Group::add( shape_input_ );

	vector<string> shape_names_string;
	for( auto& el : shape_names ) shape_names_string.push_back( el.second );

	shape_input_.AssignElements( shape_names_string );
	shape_input_.SetCurrentElement( shape_names.at( Shape::Sphere ) );
	shape_input_.tooltip( "Select shape of feature.");

}

void Fl_ModelFeature::callback( Fl_Callback* callback_function, void* p ){


	active_button_.callback( callback_function, p );
	special_property_input_.callback( callback_function, p );
	value_input_.callback( callback_function, p );
	x_positon_input_.callback( callback_function, p );
	y_positon_input_.callback( callback_function, p );
	z_positon_input_.callback( callback_function, p );
	size_input_.callback( callback_function, p );
	shape_input_.callback( callback_function, p );

}

void Fl_ModelFeature::HideFields( void ){

		
	if( !is_visible_ ) return;

	special_property_input_.hide();
	value_input_.hide();
	x_positon_input_.hide();
	y_positon_input_.hide();
	z_positon_input_.hide();
	size_input_.hide();
	shape_input_.hide();

	is_visible_ = false;
}

void Fl_ModelFeature::ShowFields( void ){

		
	if( is_visible_ ) return;

	special_property_input_.show();
	value_input_.show();
	x_positon_input_.show();
	y_positon_input_.show();
	z_positon_input_.show();
	size_input_.show();
	shape_input_.show();
		
	is_visible_ = true;
}


Fl_ModelCreator::Fl_ModelCreator( int w, int h, const char* label ) :
	Fl_Window{ w, h, label },
		
	model_size_group_{		X( *this, .015 ),				Y( *this, .075 ),				W( *this, .25 ),				H( *this, .9 ),				"Model size" },
	model_size_x_input_{	X( model_size_group_, .450 ),	Y( model_size_group_, .05 ),	W( model_size_group_, .45 ),	H( model_size_group_, .05 ),	"# Voxel x: " },
	model_size_y_input_{	X( model_size_group_, .450 ),	Y( model_size_group_, .125 ),	W( model_size_group_, .45 ),	H( model_size_group_, .05 ),	"# Voxel x: " },
	model_size_z_input_{	X( model_size_group_, .450 ),	Y( model_size_group_, .20 ),	W( model_size_group_, .45 ),	H( model_size_group_, .05 ),	"# Voxel x: " },
	voxel_size_x_input_{	X( model_size_group_, .450 ),	Y( model_size_group_, .3 ),		W( model_size_group_, .45 ),	H( model_size_group_, .05 ),	"Voxelsize x in mm: " },
	voxel_size_y_input_{	X( model_size_group_, .450 ),	Y( model_size_group_, .375 ),	W( model_size_group_, .45 ),	H( model_size_group_, .05 ),	"Voxelsize x in mm: " },
	voxel_size_z_input_{	X( model_size_group_, .450 ),	Y( model_size_group_, .45 ),	W( model_size_group_, .45 ),	H( model_size_group_, .05 ),	"Voxelsize x in mm: " },
	name_input_{			X( model_size_group_, .450 ),	Y( model_size_group_, .55 ),	W( model_size_group_, .4 ),		H( model_size_group_, .05 ),	"Name: " },
	store_size_button_{		X( model_size_group_, .2 ),		Y( model_size_group_, .7 ),		W( model_size_group_, .6 ),		H( model_size_group_, .075 ),	"Store size" },

	features_group_{		X( *this, .275 ),				Y( *this, .075 ),				W( *this, .71 ),					H( *this, .9 ),				"Model features" },
	background_input_{		X( features_group_, .2 ),		Y( features_group_, .05 ),		W( features_group_, .2 ),		H( features_group_, .05 ),		"Background in 1/mm" },
	information_{			X( features_group_, .5 ),		Y( features_group_, .05 ),		W( features_group_, .45 ),		H( features_group_, .15 ),		"" },
	build_button_{			X( features_group_, .15 ),		Y( features_group_, .15 ),		W( features_group_, .2 ),		H( features_group_, .05 ),		"Build model" },

	model_size_changed_callback_{ *this, &Fl_ModelCreator::UpdateModelSize },
	model_size_stored_callback_{ *this, &Fl_ModelCreator::StoreModelSize },
	feature_changed_callback_{ *this, &Fl_ModelCreator::UpdateFeatures },
	build_model_callback_{ *this, &Fl_ModelCreator::BuildModel },

	model_size_{ 3, 3, 3 },
	voxel_size_{ 1., 1., 1. },
	background_absorption_( absorption_water_Per_mm )
{
	
	Fl_Window::resizable( *this );
	Fl_Window::add( model_size_group_ );
	model_size_group_.labelsize( 30 );

	model_size_group_.box( FL_BORDER_BOX );
	model_size_group_.add( model_size_x_input_ ); model_size_group_.add( model_size_y_input_ ); model_size_group_.add( model_size_z_input_ ); 
	model_size_group_.add( voxel_size_x_input_ ); model_size_group_.add( voxel_size_y_input_ ); model_size_group_.add( voxel_size_z_input_ ); 

	model_size_x_input_.align( FL_ALIGN_LEFT ); model_size_y_input_.align( FL_ALIGN_LEFT ); model_size_z_input_.align( FL_ALIGN_LEFT );
	voxel_size_x_input_.align( FL_ALIGN_LEFT ); voxel_size_y_input_.align( FL_ALIGN_LEFT ); voxel_size_z_input_.align( FL_ALIGN_LEFT );

	model_size_x_input_.SetProperties( 1, 4096, 0, None ); model_size_y_input_.SetProperties( 1, 4096, 0, None );
	model_size_z_input_.SetProperties( 1, 4096, 0, None );

	voxel_size_x_input_.SetProperties( .001, 1000., 3, None ); voxel_size_y_input_.SetProperties( .001, 1000., 3, None );
	voxel_size_z_input_.SetProperties( .001, 1000., 3, None );

	model_size_x_input_.value( 3 ); model_size_y_input_.value( 3 ); model_size_z_input_.value( 3 );
	voxel_size_x_input_.value( 1. ); voxel_size_y_input_.value( 1. ); voxel_size_z_input_.value( 1. );

	model_size_x_input_.callback( CallbackFunction<Fl_ModelCreator>::Fl_Callback, &model_size_changed_callback_ );
	model_size_y_input_.callback( CallbackFunction<Fl_ModelCreator>::Fl_Callback, &model_size_changed_callback_ );
	model_size_z_input_.callback( CallbackFunction<Fl_ModelCreator>::Fl_Callback, &model_size_changed_callback_ );
		
	voxel_size_x_input_.callback( CallbackFunction<Fl_ModelCreator>::Fl_Callback, &model_size_changed_callback_ );
	voxel_size_y_input_.callback( CallbackFunction<Fl_ModelCreator>::Fl_Callback, &model_size_changed_callback_ );
	voxel_size_z_input_.callback( CallbackFunction<Fl_ModelCreator>::Fl_Callback, &model_size_changed_callback_ );

		
	model_size_group_.add( name_input_ );
	name_input_.align( FL_ALIGN_LEFT );
	name_input_.value( "Model name" );
	name_ = string{ name_input_.value() };

	model_size_group_.add( store_size_button_ );
	store_size_button_.labelsize( 25 );
	store_size_button_.callback( CallbackFunction<Fl_ModelCreator>::Fl_Callback, &model_size_stored_callback_ );



	Fl_Window::add( features_group_ );
	features_group_.labelsize( 30 );
	features_group_.box( FL_BORDER_BOX );

	features_group_.add( background_input_ );
	background_input_.SetProperties( 0., 100., 5, None );
	background_input_.copy_tooltip( string{ "Background of model. The absorption coefficient at " + ToString<int>( static_cast<int>( reference_energy_for_mu_eV ) / 1000 ) + "keV in mm^-1." }.c_str() );
	background_input_.value( background_absorption_ );
		
	features_group_.add( information_ );
	information_.value("Add features as you like. Feature have center coordinates\nwith respect to model's origin at one corner.\nFeatures can have a special property and different\nshapes and sizes.");


	for( size_t feature_index = 0; feature_index < num_features; feature_index++ ){

		features_.emplace_back( std::make_unique<Fl_ModelFeature>(
								X( features_group_, .075 ),		Y( features_group_, .25 + feature_index * ( h_feature + 0.02 ) ),		W( features_group_, .85 ),		H( features_group_, h_feature )
							) );

		Fl_ModelFeature& feature = *features_.back();
		features_group_.add( feature );
		feature.copy_label( string{ "Feature " + to_string( feature_index + 1) }.c_str() );
		feature.align( FL_ALIGN_LEFT );
		feature.callback( CallbackFunction<Fl_ModelCreator>::Fl_Callback, &feature_changed_callback_ );
	}

	features_group_.add( build_button_ );
	build_button_.labelsize( 20 );
	build_button_.callback( CallbackFunction<Fl_ModelCreator>::Fl_Callback, &build_model_callback_ );
	
		
	features_group_.deactivate();

	bool first = true;
	for( auto& feature_ptr : features_ ){
		if( first ){
			first = false;
			continue;
		}
			
		feature_ptr->HideFields();
	}

};


void Fl_ModelCreator::UpdateModelSize( void ){

	model_size_.x = ForceRange<unsigned int>( model_size_x_input_.value(), 1, 4096 );
	model_size_.y = ForceRange<unsigned int>( model_size_y_input_.value(), 1, 4096 );
	model_size_.z = ForceRange<unsigned int>( model_size_z_input_.value(), 1, 4096 );

	voxel_size_.x = ForceRange<double>( voxel_size_x_input_.value(), .001, 1000. );
	voxel_size_.y = ForceRange<double>( voxel_size_y_input_.value(), .001, 1000. );
	voxel_size_.z = ForceRange<double>( voxel_size_z_input_.value(), .001, 1000. );

	name_ = string{ name_input_.value() };
};

void Fl_ModelCreator::StoreModelSize( void ){

	model_size_group_.deactivate();
		
	features_group_.activate();
	features_.front()->SetActive();
};

void Fl_ModelCreator::UpdateFeatures( void ){
		
	// backgrouund changed
	if( background_absorption_ != background_input_.value() ){
		background_absorption_ = background_input_.value();
	}

	for( auto& feature_ptr : features_ ){
			
		if( feature_ptr->IsActive() )
			feature_ptr->ShowFields();
		else
			feature_ptr->HideFields();
	}

};

void Fl_ModelCreator::BuildModel( void ){

	unique_ptr<Fl_Progress_Window> progress_window = std::make_unique<Fl_Progress_Window>( this, 16, 1, "Progress");

	deactivate();

	PersistingObject<Model> model{ Model{ GlobalSystem()->CreateCopy( "Model system"), model_size_, voxel_size_, name_, VoxelData{ background_absorption_, reference_energy_for_mu_eV, VoxelData::SpecialProperty::None } }, "build model.model", true };

		

	for( size_t x = 0; x < model.size().x; x++ ){
		progress_window->ChangeLineText( 0, string{ "Builing model slice " + to_string( x + 1) + " of " + to_string( model.size().x ) } );
		Fl::check(); 
		for( size_t y = 0; y < model.size().y; y++ ){
			for( size_t z = 0; z < model.size().z; z++ ){

				const Point3D point( Tuple3D( (static_cast<double>( x ) + 0.5) * voxel_size_.x , (static_cast<double>( y ) + 0.5) * voxel_size_.y , (static_cast<double>( z ) + 0.5) * voxel_size_.z ), model.coordinate_system() );

				// check active features
				for( auto& feature_ptr : features_ ){
						
					auto& feature = *feature_ptr;

					// skip if not active
					if( !feature.IsActive() ) continue;


					const Point3D center{ feature.GetCenter(), model.coordinate_system() };
					const double size = feature.GetSize();
					const double value = feature.GetValue();
					const VoxelData::SpecialProperty special_property = feature.GetProperty();

					// check if current point is inside feature
					bool has_feature = false;
					switch( feature.GetShape() ){

						case Fl_ModelFeature::Shape::Sphere:

							if( ( center - point ).length() <= size/2. )
								has_feature = true;
						break;


						case Fl_ModelFeature::Shape::Cube:
							if( point.X() < center.X() + size / 2. && point.X() >= center.X() - size / 2. &&
								point.Y() < center.Y() + size / 2. && point.Y() >= center.Y() - size / 2. &&
								point.Z() < center.Z() + size / 2. && point.Z() >= center.Z() - size / 2.   )
								has_feature = true;

						break;

					}

					if( has_feature ){
						model.SetVoxelData( VoxelData{ value, reference_energy_for_mu_eV, special_property }, Index3D{ x, y, z } );
					}
						

				}
			}
		}
	}

	delete progress_window.release();

	FileChooser model_export{ "Export model", "*.model", path{ "./" }, Fl_Native_File_Chooser::Type::BROWSE_SAVE_FILE };
		
	path export_path  = model_export.ChooseFile();
		
	if( export_path.empty() ) return;

	if( export_path.extension() != ".model" )
	export_path += ".model";

	model.Save( export_path, true );

	activate();
}
