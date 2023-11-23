#pragma once
/*********************************************************************
 * @file   modelCreator.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   November 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include "FL/Fl_Window.H"
#include "FL/Fl_Group.H"
#include "FL/Fl_Button.H"
#include "FL/Fl_Int_Input.H"
#include "FL/Fl_Float_Input.H"
#include "FL/Fl_Toggle_Button.H"
#include "FL/Fl_Multiline_Output.H"
#include "fl_BoundInput.h"
#include "Fl_Selector.h"
#include "fl_ProgressWindow.h"
#include "widgets.h"

#include <memory>
using std::unique_ptr;
#include <array>
using std::array;

#include "model.h"
#include "callbackFunction.h"
#include "fileChooser.h"
#include "persistingObject.h"



 /*********************************************************************
	Definitions
 *********************************************************************/

class Fl_ModelFeature : public Fl_Group{

	public:

	enum Shape{
		Sphere,
		Cube
	};

	static const std::map<Shape, string> shape_names;

	static Shape GetShapeEnum( const string shape_string );
	
	Fl_ModelFeature( int x, int y, int w, int h, const char* label = 0L ) :
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
		value_input_.copy_tooltip( string{ "Attenuation coefficient at " + ToString<int>( static_cast<int>( reference_energy_for_mu_eV ) / 1000 ) + "keV in mm^-1 "}.c_str() );
		value_input_.SetProperties( 0., 100., 5, None );
		value_input_.value( mu_water );

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

	};

	void callback( Fl_Callback* callback_function, void* p ){


		active_button_.callback( callback_function, p );
		special_property_input_.callback( callback_function, p );
		value_input_.callback( callback_function, p );
		x_positon_input_.callback( callback_function, p );
		y_positon_input_.callback( callback_function, p );
		z_positon_input_.callback( callback_function, p );
		size_input_.callback( callback_function, p );
		shape_input_.callback( callback_function, p );

	};

	

	bool IsActive( void ) const{ return static_cast<bool>( active_button_.value() ); };

	Shape GetShape( void ) const{ return GetShapeEnum( string{ shape_input_.current_element() } ); };

	Tuple3D GetCenter( void ) const{ return { x_positon_input_.value(), y_positon_input_.value(), z_positon_input_.value() }; };

	double GetSize( void ) const{ return size_input_.value(); };

	VoxelData::SpecialProperty GetProperty( void ) const{ return VoxelData::GetPropertyEnum( string{ special_property_input_.current_element() } ); };

	double GetValue( void ) const{ return value_input_.value(); };

	void SetActive( void ){

		active_button_.value( 1 );
		ShowFields();

	}

	void HideFields( void ){

		
		if( !is_visible_ ) return;

		special_property_input_.hide();
		value_input_.hide();
		x_positon_input_.hide();
		y_positon_input_.hide();
		z_positon_input_.hide();
		size_input_.hide();
		shape_input_.hide();

		is_visible_ = false;
	};

	void ShowFields( void ){

		
		if( is_visible_ ) return;

		special_property_input_.show();
		value_input_.show();
		x_positon_input_.show();
		y_positon_input_.show();
		z_positon_input_.show();
		size_input_.show();
		shape_input_.show();
		
		is_visible_ = true;
	};

	private:

	Fl_Toggle_Button active_button_;
	Fl_Selector special_property_input_;
	Fl_BoundInput<Fl_Float_Input, double> value_input_;
	Fl_BoundInput<Fl_Float_Input, double> x_positon_input_;
	Fl_BoundInput<Fl_Float_Input, double> y_positon_input_;
	Fl_BoundInput<Fl_Float_Input, double> z_positon_input_;
	Fl_BoundInput<Fl_Float_Input, double> size_input_;
	Fl_Selector shape_input_;

	bool is_visible_;
 };



 


/*!
 * @brief Class for a window that can build models
*/
class Fl_ModelCreator : public Fl_Window{

	public:

	/*!
	 * @brief Constructor
	 * @param w Width of window
	 * @param h Height of window
	 * @param label Window title
	*/
	Fl_ModelCreator( int w, int h, const char* label ) :
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
		background_attenuation_( mu_water )
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
		background_input_.copy_tooltip( string{ "Background of model. The attenuation coefficient at " + ToString<int>( static_cast<int>( reference_energy_for_mu_eV ) / 1000 ) + "keV in mm^-1." }.c_str() );
		background_input_.value( background_attenuation_ );
		
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



	private:

	constexpr static size_t num_features = 10;
	constexpr static double h_feature = .05;

	Fl_Group model_size_group_;
	Fl_BoundInput<Fl_Int_Input, unsigned int> model_size_x_input_;
	Fl_BoundInput<Fl_Int_Input, unsigned int> model_size_y_input_;
	Fl_BoundInput<Fl_Int_Input, unsigned int> model_size_z_input_;
	Fl_BoundInput<Fl_Float_Input, double> voxel_size_x_input_;
	Fl_BoundInput<Fl_Float_Input, double> voxel_size_y_input_;
	Fl_BoundInput<Fl_Float_Input, double> voxel_size_z_input_;
	Fl_Input name_input_;
	Fl_Button store_size_button_;
	
	Fl_Group features_group_;
	Fl_BoundInput<Fl_Float_Input, double> background_input_;
	Fl_Multiline_Output information_;
	vector<unique_ptr<Fl_ModelFeature>> features_;
	Fl_Button build_button_;

	CallbackFunction<Fl_ModelCreator> model_size_changed_callback_;
	CallbackFunction<Fl_ModelCreator> model_size_stored_callback_;
	CallbackFunction<Fl_ModelCreator> feature_changed_callback_;
	CallbackFunction<Fl_ModelCreator> build_model_callback_;


	Index3D model_size_;
	Tuple3D voxel_size_;
	string name_;

	double background_attenuation_;


	void UpdateModelSize( void ){

		model_size_.x = ForceRange<unsigned int>( model_size_x_input_.value(), 1, 4096 );
		model_size_.y = ForceRange<unsigned int>( model_size_y_input_.value(), 1, 4096 );
		model_size_.z = ForceRange<unsigned int>( model_size_z_input_.value(), 1, 4096 );

		voxel_size_.x = ForceRange<double>( voxel_size_x_input_.value(), .001, 1000. );
		voxel_size_.y = ForceRange<double>( voxel_size_y_input_.value(), .001, 1000. );
		voxel_size_.z = ForceRange<double>( voxel_size_z_input_.value(), .001, 1000. );

		name_ = string{ name_input_.value() };
	};

	void StoreModelSize( void ){

		model_size_group_.deactivate();
		
		features_group_.activate();
		features_.begin()->get()->SetActive();
	};

	void UpdateFeatures( void ){
		
		// Backgrouund changed
		if( background_attenuation_ != background_input_.value() ){
			background_attenuation_ = background_input_.value();
		}

		for( auto& feature_ptr : features_ ){
			
			if( feature_ptr->IsActive() )
				feature_ptr->ShowFields();
			else
				feature_ptr->HideFields();
		}

	};

	void BuildModel( void ){

		unique_ptr<Fl_Progress_Window> progress_window = std::make_unique<Fl_Progress_Window>( this, 12, 1, "Progress");

		deactivate();

		PersistingObject<Model> model{ Model{ GlobalSystem()->CreateCopy( "Model system"), model_size_, voxel_size_, name_, VoxelData{ background_attenuation_, reference_energy_for_mu_eV, VoxelData::SpecialProperty::None } }, "build model.model", true };

		

		for( size_t x = 0; x < model.size().x; x++ ){
			progress_window->ChangeLineText( 0, string{ "Builing model slice " + to_string( x + 1) + " of " + to_string( model.size().x ) } );

			for( size_t y = 0; y < model.size().y; y++ ){
				for( size_t z = 0; z < model.size().z; z++ ){

					const Point3D point( Tuple3D( static_cast<double>( x ) * voxel_size_.x , static_cast<double>( y ) * voxel_size_.y , static_cast<double>( z ) * voxel_size_.z ), model.coordinate_system() );

					// Check active features
					for( auto& feature_ptr : features_ ){
						
						auto& feature = *feature_ptr;

						// Skip if not active
						if( !feature.IsActive() ) continue;


						const Point3D center{ feature.GetCenter(), model.coordinate_system() };
						const double size = feature.GetSize();
						const double value = feature.GetValue();
						const VoxelData::SpecialProperty special_property = feature.GetProperty();

						// Check if current point is inside feature
						bool has_feature = false;
						switch( feature.GetShape() ){

							case Fl_ModelFeature::Shape::Sphere:

								if( ( center - point ).length() <= size/2. )
									has_feature = true;
							break;


							case Fl_ModelFeature::Shape::Cube:
								if( point.X() <= center.X() + size / 2. && point.X() >= center.X() - size / 2.  &&
									point.Y() <= center.Y() + size / 2. && point.Y() >= center.Y() - size / 2.  &&
									point.Z() <= center.Z() + size / 2. && point.Z() >= center.Z() - size / 2.  )
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

		this->hide();
	}

};