/******************************************************************
* @file   fl_processingWindow.cpp
* @brief  Implementations
*
* @author Jan Wolzenburg
* @date   October 2023
* ********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include "fl_ProcessingWindow.h"
#include "programState.h"
#include "widgets.h"



 /*********************************************************************
	Implementations
 *********************************************************************/

 
PersistingObject<FileChooser> Fl_ProcessingWindow::export_filteredProjections_file_chooser_{ FileChooser{ "Export", "*.filteredprojections", path{ "./" }, Fl_Native_File_Chooser::Type::BROWSE_SAVE_FILE }, "exportFilteredProjections.chooser" };
PersistingObject<FileChooser> Fl_ProcessingWindow::export_image_chooser_{ FileChooser{ "Export", "*.backprojection", path{ "./" }, Fl_Native_File_Chooser::Type::BROWSE_SAVE_FILE }, "exportBackprojection.chooser" };

Fl_ProcessingWindow::Fl_ProcessingWindow( int w, int h, const char* label, const Projections projections ) :
	Fl_Window{ w, h, label },
	
	projections_image_{				X( *this, .025 ),			Y( *this, .03 ),			W( *this, .45 ),			H( *this, .35 ), "Projections"},
	
	information_output_{			X( *this, .1 ),			Y( *this, .4),			W( *this, .35 ),			H( *this, .225 ), "Information"}, 

	filter_group_{					X( *this, .025 ),			Y( *this, 0.7 ),			W( *this, .45 ),			H( *this, .2  ) },
	filter_type_selector_{			X( filter_group_, 0. ),		Y( filter_group_, 0. ),		W( filter_group_, .3 ),		H( filter_group_, .175 ), "Filter type" },
	filter_plot_{					X( filter_group_, 0.375 ),	Y( filter_group_, 0. ),	W( filter_group_, .6 ),		H( filter_group_, 1. ), "Filter" },
	
	
	filtered_projections_image_{		X( *this, .525 ),			Y( *this, .03 ),			W( *this, .45 ),			H( *this, .35 ), "Filtered projections"},
	export_filteredProjections_button_{ X( *this, .525 ),			Y( *this, .39 ),			W( *this, .08 ),			H( *this, .03 ), "Export"},
	reconstructed_image_{			X( *this, .525 ),			Y( *this, 0.45 ),		W( *this, .45 ),			H( *this, .5 ), "Backprojection"},
	export_image_button_{			X( *this, .525 ),			Y( *this, 0.96 ),		W( *this, .08 ),			H( *this, .03 ), "Export"},
	processing_properties_group_{	X( *this, .65 ),			Y( *this, 0.96 ),		W( *this, .275 ),			H( *this, .03  ) },
	hu_mu_selection_button_{        X( processing_properties_group_, .1 ),		Y( processing_properties_group_, 0. ),		W( processing_properties_group_, .3 ),		H( processing_properties_group_, 1. ), "Enable HU" },
	mu_water_input_{				X( processing_properties_group_, .6 ),		Y( processing_properties_group_, 0. ),		W( processing_properties_group_, .3 ),		H( processing_properties_group_, 1. ), "µ_0 water" },


	projections_( projections ),
	filtered_projections_{ FilteredProjections{}, "saved.filteredprojections", true },
	backprojection_{ Backprojection{}, "saved.backprojection", true },

	filter_change_callback_{ *this, &Fl_ProcessingWindow::FilterAndReconstructImage },
	hu_mu_selection_changed_callback_{ *this, &Fl_ProcessingWindow::UpdateImage },
	hu_mu_input_changed_callback_{  *this, &Fl_ProcessingWindow::HandleMUChange },
	export_filteredProjections_callback_{ *this, &Fl_ProcessingWindow::ExportFilteredProjections },
	export_image_callback_{ *this, &Fl_ProcessingWindow::ExportBackprojections }

{
	Fl_Window::copy_label( string{ string{ label } + ": " + projections_.tomography_properties().name}.c_str());

	Fl_Window::add( filter_group_ );
	Fl_Window::add( projections_image_ );
	Fl_Window::add( information_output_ );
	Fl_Window::add( filtered_projections_image_ );
	Fl_Window::add( reconstructed_image_ );
	Fl_Window::add( export_filteredProjections_button_ );
	Fl_Window::add( export_image_button_ );
	Fl_Window::add( processing_properties_group_ );
	Fl_Window::resizable( *this );
	
	information_output_.textfont( FL_COURIER );
	information_output_.value(	string{
									"Name:                               " + projections_.tomography_properties().name + '\n' +
									"Sinogramgröße:                      " + ToString( projections_.properties().number_of_projections()) + " x " + ToString(projections_.properties().number_of_distances()) + '\n' +
									"Sinogramauflösung:                  " + ToString( projections_.properties().angles_resolution() / 2. / PI * 360.,2 ) + "° x " + ToString( projections_.properties().distances_resolution(), 2) + " mm" + '\n' + '\n'+
									"Röntgenfilter:                      " + (projections_.tomography_properties().filter_active == true ? "ja" : "nein") + '\n' + 
									"Mittlere Röhrenenergie:             " + ToString(projections_.tomography_properties().mean_energy_of_tube) + " keV" + '\n' +
									"Energieabhängige Schwächung:        " + (projections_.tomography_properties().use_simple_absorption == false ? "ja" : "nein") + '\n' + '\n' +
									"Streuung aktiviert:                 " + (projections_.tomography_properties().scattering_enabled == true ? "ja" : "nein") + '\n' +
									"Faktor für Streuwahrscheinlichkeit: " + ToString( projections_.tomography_properties().scatter_propability_correction * 100., 1 ) + "%" + '\n' +
									"Faktor für Streuintensität:         " + ToString( projections_.tomography_properties().scattered_ray_absorption_factor * 100., 1 ) + "%" + '\n'
								}.c_str() );

	filter_group_.add( filter_plot_ );
	filter_group_.add( filter_type_selector_ );

	filter_type_selector_.align( FL_ALIGN_TOP );
	filter_type_selector_.callback( CallbackFunction<Fl_ProcessingWindow>::Fl_Callback, &filter_change_callback_ );

	vector<string> filterNames;
	for( auto& el : BackprojectionFilter::filter_types ) filterNames.push_back( el.second );
	filter_type_selector_.AssignElements( filterNames );

	filter_plot_.Initialise( PROGRAM_STATE().GetAbsolutePath( "filterPlot.png" ), "n", "a^2 * h(n)", PlotLimits{ true, true }, "", "", false, true );

	
	processing_properties_group_.add( mu_water_input_ );
	processing_properties_group_.add( hu_mu_selection_button_ );
	mu_water_input_.SetProperties( 0., 10., 7 );
	mu_water_input_.value(  projections_.tomography_properties().scattering_enabled ? attenuation_water_Per_mm : absorption_water_Per_mm );
	mu_water_input_.callback( CallbackFunction<Fl_ProcessingWindow>::Fl_Callback, &hu_mu_input_changed_callback_ );
	hu_mu_selection_button_.color( FL_BACKGROUND_COLOR, FL_DARK_GREEN );
	hu_mu_selection_button_.callback( CallbackFunction<Fl_ProcessingWindow>::Fl_Callback, &hu_mu_selection_changed_callback_ );

	mu_water_input_.tooltip( "Enter attenuation coefficient for water given by backprojected image.\nHover with mouse over a region with water and enter value.\nLook out for the exponent!");
	hu_mu_selection_button_.tooltip("Switch between view of absorption coefficient and Houndsfield Units. Least are calculated by the entered value.");

	projections_image_.align( FL_ALIGN_TOP );
	filtered_projections_image_.align( FL_ALIGN_TOP );
	reconstructed_image_.align( FL_ALIGN_TOP );

	projections_image_.labelsize( 20 );
	filtered_projections_image_.labelsize( 20 );
	reconstructed_image_.labelsize( 20 );
	
	projections_image_.Setlabel( { "angle in rad", " d in mm" } ); 
	filtered_projections_image_.Setlabel( { "angle in rad", " d in mm" } ); 
	reconstructed_image_.Setlabel( { "x in mm", " y in mm" } ); 

	export_filteredProjections_button_.callback( CallbackFunction<Fl_ProcessingWindow>::Fl_Callback, &export_filteredProjections_callback_ );
	export_image_button_.callback( CallbackFunction<Fl_ProcessingWindow>::Fl_Callback, &export_image_callback_ );
	
	projections_image_.ResetBounds();
	filtered_projections_image_.ResetBounds();
	reconstructed_image_.ResetBounds();

	
	this->show();

	
	FilterAndReconstructImage();
}


void Fl_ProcessingWindow::FilterAndReconstructImage( void ){

	this->deactivate();

	Fl_Progress_Window* processingProgressWindow = new Fl_Progress_Window{ (Fl_Window*) this, 16, 2, "Processing progress"};

	filtered_projections_ = std::move( FilteredProjections{ projections_, BackprojectionFilter::GetType( filter_type_selector_.current_element() ), processingProgressWindow } );

	if( filtered_projections_.filter().type() == BackprojectionFilter::TYPE::constant )
		filter_plot_.hide();
	else
		filter_plot_.show();

	filter_plot_.SetLimits( PlotLimits{ false, true, filtered_projections_.filter().GetRelevantRange(), NumberRange{}, 1., pow( filtered_projections_.resolution().r, 2.) } );
	filter_plot_.plot().AssignData(filtered_projections_.filter().GetPlotValues() );
	filter_plot_.AssignData();
	
	projections_image_.AssignImage( std::move( GrayscaleImage{ projections_.data(), true } ) );
	filtered_projections_image_.AssignImage( std::move( GrayscaleImage{ filtered_projections_.data_grid(), true } ) );

	projections_image_.SetAxis( { projections_.start().c, projections_.start().r }, {projections_.resolution().c, projections_.resolution().r} );
	filtered_projections_image_.SetAxis( { filtered_projections_.start().c, filtered_projections_.start().r }, {filtered_projections_.resolution().c, filtered_projections_.resolution().r} );

	backprojection_ = std::move( Backprojection{ filtered_projections_, processingProgressWindow } );

	UpdateImage();

	delete processingProgressWindow;

	this->activate();

}

void Fl_ProcessingWindow::ExportFilteredProjections( void ){
	path exportPath = export_filteredProjections_file_chooser_.ChooseFile();
	export_filteredProjections_file_chooser_.SetAsLoaded();
	if( exportPath.empty() ) return;

	if( exportPath.extension() != ".filteredprojections" )
		exportPath += ".filteredprojections";

	filtered_projections_.Save( exportPath );
}

void Fl_ProcessingWindow::ExportBackprojections( void ){

	path exportPath = export_image_chooser_.ChooseFile();
	export_image_chooser_.SetAsLoaded();
	if( exportPath.empty() ) return;

	if( exportPath.extension() != ".backprojection" )
		exportPath += ".backprojection";

	backprojection_.Save( exportPath );
}


void Fl_ProcessingWindow::UpdateImage( void ){

	DataGrid<> raw_image_data = backprojection_.getGrid();
	bool values_changed = false;

	for( size_t column_index = 0; column_index < raw_image_data.size().c; column_index++ ){
		for( size_t row_index = 0; row_index < raw_image_data.size().r; row_index++ ){
		
			double current_value = raw_image_data.GetData( GridIndex{ column_index, row_index } );

			// Correct if simple absorption is not used
			if( !projections_.tomography_properties().use_simple_absorption ){

				raw_image_data.SetData( GridIndex{ column_index, row_index }, 
							VoxelData::GetAbsorptionAtReferenceEnergy( 
								current_value, 
								projections_.tomography_properties().mean_energy_of_tube ) );
			}
			
			if( static_cast<bool>( hu_mu_selection_button_.value() ) ){

				current_value = raw_image_data.GetData( GridIndex{ column_index, row_index } );
				raw_image_data.SetData( GridIndex{ column_index, row_index }, 
									  ( current_value  -  mu_water_input_.value() ) / mu_water_input_.value() * 1000. );
				reconstructed_image_.SetValueTip( "HU" );
				
			}
			else{
				reconstructed_image_.SetValueTip( "µ_0 in mm^-1" );
			}

		}
	}

	reconstructed_image_.AssignImage( std::move( GrayscaleImage{ std::move( raw_image_data ), true } ) );
	reconstructed_image_.SetAxis( { raw_image_data.start().c, -raw_image_data.start().r }, {raw_image_data.resolution().c, -raw_image_data.resolution().r} );

}


void Fl_ProcessingWindow::HandleMUChange( void ){
	if( static_cast<bool>( hu_mu_selection_button_.value() ) )
		UpdateImage();
}