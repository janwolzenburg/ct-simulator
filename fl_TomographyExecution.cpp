/******************************************************************
* @file   tomographyExecution.cpp
*
* @author Jan Wolzenburg
* @date   September 2023
* ********************************************************************/



/*********************************************************************
  Includes
*********************************************************************/


#include "fl_TomographyExecution.h"
#include "fl_GantryCreation.h"

#include "filteredProjections.h"
#include "fl_ProgressWindow.h"
#include "widgets.h"
#include "fl_MainWindow.h"


Fl_TomographyExecution::Fl_TomographyExecution( int x, int y, int w, int h, Fl_MainWindow& main_window ) :
	Fl_Group{ x, y, w, h },
	title_{																X( *this, 0. ), Y( *this, 0. ), W( *this, 1. ), H( *this, .035 ), "Tomography"},

	tomography_properties_group_{					X( *this, .0 ),				Y( *this, .04 ),				W( *this, 1. ),				H( *this, .6 ) },
	properties_title_{										X( tomography_properties_group_, 0. ),	Y( tomography_properties_group_, 0. ),	W( tomography_properties_group_, 1. ),	H( tomography_properties_group_, .05 ), "Parameter" },
	maximum_scatterings_input_{						X( tomography_properties_group_, 0. ),	Y( tomography_properties_group_, .1 ),	W( tomography_properties_group_, .45 ),	H( tomography_properties_group_, .045 ), "Maximum loops" },
	scattering_propability_factor_input_{	X( tomography_properties_group_, .5 ),	Y( tomography_properties_group_, .1 ),	W( tomography_properties_group_, .45 ),	H( tomography_properties_group_, .045 ), "Propability factor" },
	disable_scattering_button_{						X( tomography_properties_group_, .5 ),	Y( tomography_properties_group_, .3 ),	W( tomography_properties_group_, .3 ),	H( tomography_properties_group_, .05 ), "Scattering" },
	scattering_absorption_factor_input_{  X( tomography_properties_group_, .0 ),	Y( tomography_properties_group_, .2 ),	W( tomography_properties_group_, .45 ),	H( tomography_properties_group_, .045 ), "Absorption factor" },
	use_simple_absorption_button_{				X( tomography_properties_group_, .0 ),	Y( tomography_properties_group_, .3 ),	W( tomography_properties_group_, .45 ),	H( tomography_properties_group_, .05 ), "Simple absorption" },
	simulation_quality_input_{						X( tomography_properties_group_, .0 ),	Y( tomography_properties_group_, .4 ),	W( tomography_properties_group_, .45 ),	H( tomography_properties_group_, .05 ), "Simulation quality" },
	information_{													X( tomography_properties_group_, 0. ),	Y( tomography_properties_group_, .5 ),	W( tomography_properties_group_, .95 ),	H( tomography_properties_group_, .4 ), "Information" },

	control_group_{								X( *this, .0 ),						vOff( tomography_properties_group_ ), W( *this, 1. ), H( *this, .1 ) },
	name_input_{									X( control_group_, .05 ), Y( control_group_, .1 ), W( control_group_, .9 ), H( control_group_, .4 ), "Name" },			
	record_slice_button_{					X( control_group_, .05 ), Y( control_group_, .6 ), W( control_group_, .4 ), H( control_group_, .4 ), "Record Slice" },
	export_projections_button_{		X( control_group_, .55 ), Y( control_group_, .6 ), W( control_group_, .4 ), H( control_group_, .4 ), "Export Projectiions" },

	
	main_window_( main_window ),

	export_projections_file_chooser_{ FileChooser{ "Export projections", "*.projections", path{ "./" }, Fl_Native_File_Chooser::Type::BROWSE_SAVE_FILE }, "projectionsExport.chooser" },
	
	tomography_properties_{ TomographyProperties{}, "saved.tomographyproperties" },
	tomography_{},
	
	projections_{ Projections{}, "saved.projections" },
	processing_windows_( 0 ),

	record_slice_callback_{ *this, &Fl_TomographyExecution::DoTomography },
	update_properties_callback_{ *this, &Fl_TomographyExecution::UpdateProperties },
	export_projections_callback_{ *this, &Fl_TomographyExecution::ExportProjections }
{


	Fl_Group::add( title_ );
	title_.box( FL_NO_BOX ); title_.align( FL_ALIGN_CENTER ); title_.labelsize( 30 );

	Fl_Group::add( tomography_properties_group_ );

	tomography_properties_group_.add( properties_title_ );

	tomography_properties_group_.add( maximum_scatterings_input_ );
	tomography_properties_group_.add( scattering_propability_factor_input_ );
	tomography_properties_group_.add( disable_scattering_button_ );
	tomography_properties_group_.add( use_simple_absorption_button_ );
	tomography_properties_group_.add( simulation_quality_input_ );
	tomography_properties_group_.add( information_ );
	tomography_properties_group_.add( scattering_absorption_factor_input_ );


	properties_title_.box( FL_NO_BOX ); properties_title_.align( FL_ALIGN_CENTER ); properties_title_.labelsize( 20 );

	maximum_scatterings_input_.align( FL_ALIGN_TOP );
	scattering_propability_factor_input_.align( FL_ALIGN_TOP );
	scattering_absorption_factor_input_.align( FL_ALIGN_TOP );
	simulation_quality_input_.align( FL_ALIGN_TOP );

	maximum_scatterings_input_.bounds(0, 100);
	maximum_scatterings_input_.step( 1. );
	scattering_propability_factor_input_.bounds( 0., 100 );
	scattering_propability_factor_input_.step( .5 );
	scattering_propability_factor_input_.lstep( 10. );
	scattering_absorption_factor_input_.bounds( .0, 100. );
	scattering_absorption_factor_input_.step( .5 );
	scattering_absorption_factor_input_.lstep( 10. );
	


	maximum_scatterings_input_.tooltip( "Maximum amount of iterations for ray tracing. How often a ray can be scattered." );
	scattering_propability_factor_input_.tooltip( "Correction factor in % for scattering propability. More scattering with higher value." );
	scattering_absorption_factor_input_.tooltip( "factor to scale a ray's energy when it is scattered. In %" );
	disable_scattering_button_.tooltip( "Enable or disable scattering." );
	use_simple_absorption_button_.tooltip( "If enabled \"simple\" absorption is active which is not energy dependent." );
	simulation_quality_input_.tooltip( "Change quality of simulation. Low number is faster but not as realistic." );

	maximum_scatterings_input_.value( static_cast<double>( tomography_properties_.max_scattering_occurrences ) );
	scattering_propability_factor_input_.value( tomography_properties_.scatter_propability_correction*100. );
	scattering_absorption_factor_input_.value( tomography_properties_.scattered_ray_absorption_factor*100. );
	disable_scattering_button_.value( static_cast<int>( tomography_properties_.scattering_enabled ) );
	use_simple_absorption_button_.value( static_cast<int>( tomography_properties_.use_simple_absorption ) );
	disable_scattering_button_.color( FL_BACKGROUND_COLOR, FL_DARK_GREEN );
	use_simple_absorption_button_.color( FL_BACKGROUND_COLOR, FL_DARK_GREEN );

	simulation_quality_input_.bounds( 1., 100. );
	simulation_quality_input_.step( 1. );
	simulation_quality_input_.lstep( 5. );
	simulation_quality_input_.value( static_cast<double>( tomography_properties_.simulation_quality ) );

	maximum_scatterings_input_.callback( CallbackFunction<Fl_TomographyExecution>::Fl_Callback, &update_properties_callback_ );

	scattering_propability_factor_input_.callback( CallbackFunction<Fl_TomographyExecution>::Fl_Callback, &update_properties_callback_ );
	scattering_absorption_factor_input_.callback( CallbackFunction<Fl_TomographyExecution>::Fl_Callback, &update_properties_callback_ );
	disable_scattering_button_.callback( CallbackFunction<Fl_TomographyExecution>::Fl_Callback, &update_properties_callback_ );
	use_simple_absorption_button_.callback( CallbackFunction<Fl_TomographyExecution>::Fl_Callback, &update_properties_callback_ );
	simulation_quality_input_.callback( CallbackFunction<Fl_TomographyExecution>::Fl_Callback, &update_properties_callback_ );

	information_.align( FL_ALIGN_TOP );
	information_.textfont( FL_COURIER );

	Fl_Group::add( control_group_ );
	
	control_group_.add( name_input_ );
	control_group_.add( record_slice_button_ );
	control_group_.add( export_projections_button_ );
	name_input_.tooltip("Give a name to identify this recording.");
	name_input_.maximum_size( 40 );

	export_projections_button_.deactivate();

	name_input_.callback(  CallbackFunction<Fl_TomographyExecution>::Fl_Callback, &update_properties_callback_  );
	record_slice_button_.callback( CallbackFunction<Fl_TomographyExecution>::Fl_Callback, &record_slice_callback_ );
	export_projections_button_.callback( CallbackFunction<Fl_TomographyExecution>::Fl_Callback, &export_projections_callback_ );

	this->deactivate();
}

void Fl_TomographyExecution::AssignProjections( const Projections projections ){
	projections_ = projections;

	if( tomography_properties_.use_simple_absorption ){
		tomography_properties_.mean_energy_of_tube = -1.;
	}

	export_projections_button_.activate();

	std::unique_ptr<Fl_ProcessingWindow> ptr = std::make_unique<Fl_ProcessingWindow>(  static_cast<int>( 1920. * 0.9 ), static_cast<int>( 1080. * 0.9 ), "Processing", projections_, &main_window_ );
	processing_windows_.push_back( std::move( ptr ) );
}


void Fl_TomographyExecution::UpdateProperties( void ){

	if (use_simple_absorption_button_.value() == 1 && disable_scattering_button_.value() == 1) {
		disable_scattering_button_.value(0);
	}

	tomography_properties_ = TomographyProperties{	static_cast<bool>( disable_scattering_button_.value() ), 
													static_cast<size_t>( maximum_scatterings_input_.value() ), 
													scattering_propability_factor_input_.value()/100., 
													static_cast<bool>( use_simple_absorption_button_.value() ), 
													scattering_absorption_factor_input_.value()/100.,
													name_input_.value(),main_window_.gantry_creation_.gantry().tube().properties().has_filter_,
													static_cast<size_t>( simulation_quality_input_.value() ) };

	if( simulation_properties.quality != tomography_properties_.simulation_quality ){
		simulation_properties = SimulationProperties{ tomography_properties_.simulation_quality };
		main_window_.gantry_creation_.UpdateGantry();
	}

	UpdateInformation( main_window_.gantry_creation_.projections_properties(), main_window_.gantry_creation_.gantry().tube() );

}

void Fl_TomographyExecution::DoTomography( void ){

		main_window_.deactivate();

		Fl_Progress_Window* radiationProgressWindow = new Fl_Progress_Window{ (Fl_Window*) window(), 16, 2, "Radiation progress"};
		
		tomography_properties_.mean_energy_of_tube =  main_window_.gantry_creation_.gantry().tube().GetMeanEnergy();
		tomography_properties_.filter_active =  main_window_.gantry_creation_.gantry().tube().properties().has_filter_;

		tomography_ = Tomography{ tomography_properties_ };

		if( radiationProgressWindow != nullptr ){
			optional<Projections> new_projections = tomography_.RecordSlice( main_window_.gantry_creation_.projections_properties(), main_window_.gantry_creation_.gantry(), main_window_.model_view_.model(), 0, radiationProgressWindow);
			
			if( new_projections.has_value() ){
				AssignProjections( std::move( new_projections.value() ) );
			}
			delete radiationProgressWindow;
		}

		main_window_.activate();
}


void Fl_TomographyExecution::UpdateInformation( const ProjectionsProperties& projection_properties, XRayTube tube ){
		string informationString = "";


		informationString += "# Projektionswinkel:             " + ConvertToString( projection_properties.number_of_projections() ) + '\n'; 
		informationString += "# Projektionen je Winkel:        " + ConvertToString( projection_properties.number_of_distances() ) + '\n';
		informationString += "  Auflösung:                     " + ConvertToString( projection_properties.angles_resolution() / 2. / PI * 360.,2 ) + "° x " + ConvertToString( projection_properties.distances_resolution(), 2) + " mm" + '\n' + '\n';
		informationString += "# Gantryrotationen:              " + ConvertToString( projection_properties.number_of_frames_to_fill() ) + '\n';
		informationString += "  mittlere Strahlungenergie:     " + ConvertToString( tube.GetMeanEnergy() ) + " eV" + '\n';
		informationString += "# Energien im Spektrum:          " + ConvertToString( simulation_properties.number_of_points_in_spectrum ) + '\n' + '\n';
		informationString += "# Streumöglichkeiten je Energie: " + ConvertToString( simulation_properties.bins_per_energy ) + '\n';
		informationString += "# mögliche Streuwinkel:          " + ConvertToString( simulation_properties.number_of_scatter_angles ) + '\n';

		information_.value( informationString.c_str() );
}

void Fl_TomographyExecution::ExportProjections( void ){

	path export_path = export_projections_file_chooser_.ChooseFile();
	export_projections_file_chooser_.SetAsLoaded();
	if( export_path.empty() ) return;

	if( export_path.extension() != ".projections" )
		export_path += ".projections";

	projections_.Save( export_path );

}