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


Fl_ProcessingWindow::Fl_ProcessingWindow( int w, int h, const char* label, Projections projections ) :
	Fl_Window{ w, h, label },
	
	projections_image_{				X( *this, .025 ),			Y( *this, .03 ),			W( *this, .45 ),			H( *this, .3 ), "Projections"},
	
	filter_group_{					X( *this, .025 ),			Y( *this, 0.4 ),			W( *this, .45 ),			H( *this, .29  ) },
	filter_type_selector_{			X( filter_group_, 0. ),		Y( filter_group_, 0. ),		W( filter_group_, .3 ),		H( filter_group_, .1 ), "Filter type" },
	filter_plot_{					X( filter_group_, 0. ),		Y( filter_group_, 0.15 ),	W( filter_group_, 1. ),		H( filter_group_, .85 ), "Filter" },
	
	filtered_projections_image_{	X( *this, .525 ),			Y( *this, .03 ),			W( *this, .45 ),			H( *this, .3 ), "Filtered projections"},
	
	reconstructed_image_{			X( *this, .525 ),			Y( *this, 0.375 ),		W( *this, .45 ),			H( *this, .6 ), "Backprojection"},

	projections_( projections ),
	filter_change_callback_{ *this, &Fl_ProcessingWindow::ReconstructImage }

{
	Fl_Window::resizable( *this );
	Fl_Window::add( projections_image_ );
	Fl_Window::add( filter_group_ );
	Fl_Window::add( filtered_projections_image_ );
	Fl_Window::add( reconstructed_image_ );

	
	projections_image_.ResetBounds();
	projections_image_.AssignImage( GrayscaleImage{ projections.data(), true } );


	filter_group_.add( filter_plot_ );
	filter_group_.add( filter_type_selector_ );

	filter_type_selector_.align( FL_ALIGN_TOP );
	filter_type_selector_.callback( CallbackFunction<Fl_ProcessingWindow>::Fl_Callback, &filter_change_callback_ );

	vector<string> filterNames;
	for( auto& el : BackprojectionFilter::filter_types ) filterNames.push_back( el.second );
	filter_type_selector_.AssignElements( filterNames );

	filter_plot_.Initialise( PROGRAM_STATE().GetAbsolutePath( "filterPlot.png" ), "n", "a^2 * h(n)", PlotLimits{ true, true }, "", "", false, true );

	projections_image_.align( FL_ALIGN_TOP );
	filtered_projections_image_.align( FL_ALIGN_TOP );
	reconstructed_image_.align( FL_ALIGN_TOP );

	projections_image_.labelsize( 20 );
	filtered_projections_image_.labelsize( 20 );
	reconstructed_image_.labelsize( 20 );
	
	filtered_projections_image_.ResetBounds();
	reconstructed_image_.ResetBounds();

	
	this->show();
	ReconstructImage();
}


void Fl_ProcessingWindow::ReconstructImage( void ){

	this->deactivate();

	Fl_Progress_Window* processingProgressWindow = new Fl_Progress_Window{ (Fl_Window*) this, 20, 5, "Processing progress"};

	filtered_projections_ = std::move( FilteredProjections{ projections_, BackprojectionFilter::GetType( filter_type_selector_.current_element() ), processingProgressWindow } );

	if( filtered_projections_.filter().type() == BackprojectionFilter::TYPE::constant )
		filter_plot_.hide();
	else
		filter_plot_.show();

	filter_plot_.SetLimits( PlotLimits{ false, true, filtered_projections_.filter().GetRelevantRange(), NumberRange{}, 1., pow( filtered_projections_.resolution().r, 2.) } );
	filter_plot_.plot().AssignData(filtered_projections_.filter().GetPlotValues() );
	filter_plot_.AssignData();
	

	filtered_projections_image_.AssignImage( std::move( GrayscaleImage{ filtered_projections_.data_grid(), true } ) );

	backprojection_ = std::move( Backprojection{ filtered_projections_, processingProgressWindow } );

	DataGrid<> raw_image_data = backprojection_.getGrid();
	
	for( size_t column_index = 0; column_index < raw_image_data.size().c; column_index++ ){
		for( size_t row_index = 0; row_index < raw_image_data.size().r; row_index++ ){
		
			raw_image_data.SetData( GridIndex{ column_index, row_index }, 
							VoxelData::GetAttenuationAtReferenceEnergy( 
								raw_image_data.GetData( GridIndex{ column_index, row_index } ), 
								projections_.properties().tube_mean_energy() ) );

		}
	}

	reconstructed_image_.AssignImage( std::move( GrayscaleImage{ std::move( raw_image_data ), true } ) );

	delete processingProgressWindow;

	this->activate();

}