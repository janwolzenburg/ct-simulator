/******************************************************************
* @file   processingWindow.cpp
* @brief  Implementations
*
* @author Jan Wolzenburg
* @date   October 2023
* ********************************************************************/

#include "processingWindow.h"
#include "filteredProjections.h"
#include "widgets.h"
#include "reconstructedImage.h"

processingWindow::processingWindow( int w, int h, const char* label, Projections projections ) :
	Fl_Window{ w, h, label },
	currentprojections( projections ),
	sinogramGrp{		X( *this, .01 ),			Y( *this, .01 ),							W( *this, .49 ),			H( *this, .325 ) },
 	sinogramWidget{		X( sinogramGrp, .0 ),		Y( sinogramGrp, .0 ),						W( sinogramGrp, 1. ),		H( sinogramGrp, 1. ) },
		
	filterGrp{			X( *this, .01 ),			vOff( sinogramGrp ) + Y( *this, 0.025 ),	W( *this, .49 ),			H( *this, .29  ) },
	filterTypeSelector{ X( filterGrp, 0. ),			Y( filterGrp, 0. ),							W( filterGrp, .3 ),			H( filterGrp, .1 ), "Filter type" },
	filterPlot{			X( filterGrp, 0. ),			Y( filterGrp, 0.15 ),						W( filterGrp, 1. ),			H( filterGrp, .85 ), "Filter" },
		
	filteredProjGrp{	X( *this, .01 ),			vOff( filterGrp ) + Y( *this, 0.025 ),		W( *this, .49 ),			H( *this, .325 ) },
	filteredProjWidget{ X( filteredProjGrp, .0 ),	Y( filteredProjGrp, .0 ),					W( filteredProjGrp, 1. ),	H( filteredProjGrp, 1. ) },

	reconstructionGrp{ X( *this, .51 ),				Y( *this, 0.01 ),							W( *this, .48 ),			H( *this, .8 ) },
	reconstructionImageWidget{ X( reconstructionGrp, .0 ), Y( reconstructionGrp, .0 ),			W( reconstructionGrp, 1. ), H( reconstructionGrp, 1. ) }

{
	Fl_Window::add( sinogramGrp );
	Fl_Window::resizable( *this );

	sinogramGrp.add( sinogramWidget );
	sinogramWidget.box( FL_BORDER_BOX );

	Fl_Window::add( filterGrp );
	filterGrp.add( filterPlot );
	filterGrp.add( filterTypeSelector );

	filterTypeSelector.align( FL_ALIGN_TOP );
	filterTypeSelector.callback( HandleFilterChange, this );

	vector<string> filterNames;
	for( auto& el : BackprojectionFilter::filter_types ) filterNames.push_back( el.second );
	filterTypeSelector.AssignElements( filterNames );


	filterPlot.Initialise( PROGRAM_STATE().getPath( "filterPlot.png" ), "n", "a^2 * h(n)", PlotLimits{ true, true }, "", "", false, true );

	Fl_Window::add( filteredProjGrp );

	filteredProjGrp.add( filteredProjWidget );
	filteredProjWidget.box( FL_BORDER_BOX );

	Fl_Window::add( reconstructionGrp );

	reconstructionGrp.add( reconstructionImageWidget );
	reconstructionImageWidget.box( FL_BORDER_BOX );

	sinogramWidget.ResetBounds();

	sinogramWidget.AssignImage( GrayscaleImage{ projections.data(), true } );


	filteredProjWidget.ResetBounds();
	reconstructionImageWidget.ResetBounds();

	recalcFilteredProjections();
}

void processingWindow::HandleFilterChange( Fl_Widget* selector_widget, void* window ){
	
	processingWindow* windowPtr = static_cast<processingWindow*>( window );

	windowPtr->recalcFilteredProjections();
}


void processingWindow::recalcFilteredProjections( void ){

	this->deactivate();

	Fl_Progress_Window* processingProgressWindow = new Fl_Progress_Window{ (Fl_Window*) this, 20, 5, "Processing progress"};

	currentFilteredProjections = FilteredProjections{ currentprojections, BackprojectionFilter::GetType( filterTypeSelector.current_element() ), processingProgressWindow };

	if( currentFilteredProjections.filter().type() == BackprojectionFilter::TYPE::constant )
		filterPlot.hide();
	else
		filterPlot.show();

	filterPlot.SetLimits( PlotLimits{ false, true, currentFilteredProjections.filter().GetRelevantRange(), NumberRange{}, 1., pow( currentFilteredProjections.resolution().r, 2.) } );
	filterPlot.plot().AssignData(currentFilteredProjections.filter().GetPlotValues() );
	filterPlot.AssignData();
	

	filteredProjWidget.AssignImage( GrayscaleImage{ currentFilteredProjections.data_grid(), true } );

	currentReconstrucedImage = ReconstrucedImage{ currentFilteredProjections, processingProgressWindow };

	reconstructionImageWidget.AssignImage( GrayscaleImage{ currentReconstrucedImage.getGrid(), true } );

	delete processingProgressWindow;

	this->activate();

}