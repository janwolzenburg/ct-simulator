/******************************************************************
* @file   tomographyExecution.cpp
* @brief  Implementations
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
#include "progress.h"
#include "widgets.h"
#include "mainWindow.h"


Fl_TomographyExecution::Fl_TomographyExecution( int x, int y, int w, int h, mainWindow& main_window ) :
	Fl_Group{ x, y, w, h },
	main_window_( main_window ),
	title_{ X( *this, 0. ), Y( *this, 0. ), W( *this, 1. ), H( *this, .035 ), "Tomography"},

	tomography_properties_group_{		X( *this, .0 ),				Y( *this, .04 ),				W( *this, 1. ),				H( *this, .6 ) },
	properties_title_{			X( tomography_properties_group_, 0. ),	Y( tomography_properties_group_, 0. ),	W( tomography_properties_group_, 1. ),	H( tomography_properties_group_, .05 ), "Parameter" },
	
	maximum_scatterings_input_{		X( tomography_properties_group_, 0. ),	Y( tomography_properties_group_, .1 ),	W( tomography_properties_group_, .45 ),	H( tomography_properties_group_, .045 ), "Maximum loops" },
	scattering_propability_factor_input_{	X( tomography_properties_group_, .33 ),	Y( tomography_properties_group_, .6 ),	W( tomography_properties_group_, .45 ),	H( tomography_properties_group_, .045 ), "Propability factor" },
	disable_scattering_button_{		X( tomography_properties_group_, .66 ),	Y( tomography_properties_group_, .2 ),	W( tomography_properties_group_, .3 ),	H( tomography_properties_group_, .05 ), "Scattering" },
	
	information_{			X( tomography_properties_group_, 0.1 ),	Y( tomography_properties_group_, .4 ),	W( tomography_properties_group_, .8 ),	H( tomography_properties_group_, .4 ), "Information" },


	control_group_{				X( *this, .0 ), vOff( tomography_properties_group_ ), W( *this, 1. ), H( *this, .1 ) },
	record_slice_button_{		X( control_group_, .05 ), Y( control_group_, .1 ), W( control_group_, .4 ), H( control_group_, .4 ), "Record Slice" },
	export_projections_button_{			X( control_group_, .55 ), Y( control_group_, .1 ), W( control_group_, .4 ), H( control_group_, .4 ), "Export Sinogram" },

	
	export_projections_file_chooser_{ FileChooser{ "Export Sinogram", "*.sinogram", path{ "./" }, Fl_Native_File_Chooser::Type::BROWSE_SAVE_FILE }, "export.chooser" },
	
	tomography_{},
	tomography_properties_{ TomographyProperties{}, "tomography.properties" },
	

	projections_{ Projections{}, "projections.sinogram" },
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
	tomography_properties_group_.add( information_ );


	properties_title_.box( FL_NO_BOX ); properties_title_.align( FL_ALIGN_CENTER ); properties_title_.labelsize( 20 );

	maximum_scatterings_input_.align( FL_ALIGN_TOP );
	scattering_propability_factor_input_.align( FL_ALIGN_TOP );

	maximum_scatterings_input_.bounds(0, 100);
	maximum_scatterings_input_.step( 1. );
	scattering_propability_factor_input_.bounds( 1, 1000 );
	scattering_propability_factor_input_.step( 1. );
	scattering_propability_factor_input_.lstep( 100. );


	maximum_scatterings_input_.tooltip( "Maximum amount of iterations for ray tracing. How often a ray can be scattered." );
	scattering_propability_factor_input_.tooltip( "Correction factor for scattering propability. More scattering with higher value." );
	disable_scattering_button_.tooltip( "Enable or disable scattering." );

	maximum_scatterings_input_.value( (double) tomography_properties_.max_scattering_occurrences );
	scattering_propability_factor_input_.value( tomography_properties_.scatter_propability_correction );
	disable_scattering_button_.value( tomography_properties_.scattering_enabled );
	disable_scattering_button_.color( FL_BACKGROUND_COLOR, FL_DARK_GREEN );

	maximum_scatterings_input_.callback( CallbackFunction<Fl_TomographyExecution>::Fl_Callback, &update_properties_callback_ );

	scattering_propability_factor_input_.callback( CallbackFunction<Fl_TomographyExecution>::Fl_Callback, &update_properties_callback_ );
	disable_scattering_button_.callback( CallbackFunction<Fl_TomographyExecution>::Fl_Callback, &update_properties_callback_ );

	information_.align( FL_ALIGN_TOP );

	Fl_Group::add( control_group_ );

	control_group_.add( record_slice_button_ );
	control_group_.add( export_projections_button_ );

	export_projections_button_.deactivate();

	record_slice_button_.callback( CallbackFunction<Fl_TomographyExecution>::Fl_Callback, &record_slice_callback_ );
	export_projections_button_.callback( CallbackFunction<Fl_TomographyExecution>::Fl_Callback, &export_projections_callback_ );

	this->deactivate();
}

void Fl_TomographyExecution::AssignProjections( const Projections projections ){
	projections_ = projections;
	export_projections_button_.activate();

	std::unique_ptr<Fl_ProcessingWindow> ptr = std::make_unique<Fl_ProcessingWindow>(  (int) ( 1920. * 0.9 ), (int) ( 1080. * 0.9 ), "Processing", projections_ );
	processing_windows_.push_back( std::move( ptr ) );
	processing_windows_.back()->show();
}


void Fl_TomographyExecution::UpdateProperties( void ){
		tomography_properties_ = TomographyProperties{ (bool) disable_scattering_button_.value(), (size_t) maximum_scatterings_input_.value(), scattering_propability_factor_input_.value() };
}

void Fl_TomographyExecution::DoTomography( void ){

		parent()->deactivate();

		Fl_Progress_Window* radiationProgressWindow = new Fl_Progress_Window{ (Fl_Window*) window(), 20, 5, "Radiation progress"};
		

		tomography_ = Tomography{ tomography_properties_ };

		if( radiationProgressWindow != nullptr ){
			Projections new_projections = tomography_.RecordSlice( main_window_.gantryBuild.projections_properties(), main_window_.gantryBuild.gantry(), main_window_.modView.model(), 0, radiationProgressWindow);
			AssignProjections( std::move( new_projections ) );
			delete radiationProgressWindow;
		}

		parent()->activate();
}

void Fl_TomographyExecution::UpdateInformation( ProjectionsProperties projection_properties, DetectorProperties detector_properties, XRayTube tube ){
		string informationString = "";


		informationString += "Sinogramgröße:      " + ToString( projection_properties.number_of_angles() ) + " x " + ToString( projection_properties.number_of_distances() ) + '\n';
		informationString += "Sinogramauflösung:  " + ToString( projection_properties.angles_resolution() / 2. / PI * 360.,2 ) + "° x " + ToString( projection_properties.distances_resolution(), 2) + " mm" + '\n' + '\n';
		informationString += "Gantryrotationen:   " + ToString( projection_properties.number_of_frames_to_fill() ) + '\n';
		informationString += "Detektorwinkel:	  " + ToString( detector_properties.arc_angle / 2. / PI * 360., 2 ) + "°" + '\n';


		informationString += "Elektrische Leistung:	  " + ToString( tube.GetElectricalPower()) + "W" + '\n';
		informationString += "Strahlleistung:	  " + ToString( tube.GetEmittedBeamPower() ) + "W" + '\n';
		
		information_.value( informationString.c_str() );
}

void Fl_TomographyExecution::ExportProjections( void ){

	path exportPath = export_projections_file_chooser_.ChooseFile();
	export_projections_file_chooser_.SetAsLoaded();
	if( exportPath.empty() ) return;

	if( exportPath.extension() != ".sinogram" )
		exportPath += ".sinogram";

	projections_.Save( exportPath );

}