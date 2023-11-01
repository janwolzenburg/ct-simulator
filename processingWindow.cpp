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

processingWindow::processingWindow( int w, int h, const char* label ) :
	Fl_Window{ w, h, label },
		
	newRTFlag( false ),
	sinogramGrp{		X( *this, .01 ),			Y( *this, .01 ),							W( *this, .49 ),			H( *this, .325 ) },
 	sinogramWidget{		X( sinogramGrp, .0 ),		Y( sinogramGrp, .0 ),						W( sinogramGrp, 1. ),		H( sinogramGrp, 1. ) },
		
		
	filterChanged( false ),
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
	filterTypeSelector.callback( button_cb, &filterChanged );

	vector<string> filterNames;
	for( auto& el : BackprojectionFilter::filter_types ) filterNames.push_back( el.second );

	filterTypeSelector.setElements( filterNames );
	string filterName = BackprojectionFilter::filter_types.at( PROGRAM_STATE().currentProcessingParameters.filterType );
	filterTypeSelector.value( filterName );


	filterPlot.initialisePlot( PROGRAM_STATE().getPath( "filterPlot.png" ), "n", "", plotLimits{ true, true }, "", "", false, true );



	Fl_Window::add( filteredProjGrp );

	filteredProjGrp.add( filteredProjWidget );
	filteredProjWidget.box( FL_BORDER_BOX );

	Fl_Window::add( reconstructionGrp );

	reconstructionGrp.add( reconstructionImageWidget );
	reconstructionImageWidget.box( FL_BORDER_BOX );
}

void processingWindow::deactivate( void ){
	Fl_Window::deactivate();
	sinogramGrp.deactivate();
	filterGrp.deactivate();
	filteredProjGrp.deactivate();
	reconstructionGrp.deactivate();
}

void processingWindow::activate( void ){
	Fl_Window::activate();
	sinogramGrp.activate();
	filterGrp.activate();
	filteredProjGrp.activate();
	reconstructionGrp.activate();
}



void processingWindow::handleEvents( void ){

	if( PROGRAM_STATE().RadonTransformedLoaded() && newRTFlag ){
		newRTFlag = false;
		assignSinogram( PROGRAM_STATE().currentProjections );
		recalcFilteredProjections();
		Fl_Window::show();
	}

	if( sinogramWidget.handleEvents() ){
		PROGRAM_STATE().currentProcessingParameters.projectionsContrast = sinogramWidget.getContrast();
	}

	if( filterChanged ){
		filterChanged = false;
		PROGRAM_STATE().currentProcessingParameters.filterType = BackprojectionFilter::GetType( filterTypeSelector.value() );
		recalcFilteredProjections();
	}

	if( filteredProjWidget.handleEvents() ){
		if( filteredProjWidget.imageAssigned() )
			PROGRAM_STATE().currentProcessingParameters.filteredProjectionsContrast = filteredProjWidget.getContrast();
	}

	if( reconstructionImageWidget.handleEvents() ){
		if( reconstructionImageWidget.imageAssigned() )
			PROGRAM_STATE().currentProcessingParameters.reconstrucedImageContrast = reconstructionImageWidget.getContrast();
	}
}

void processingWindow::assignSinogram( const Projections newSinogram ){

	sinogramImg = GrayscaleImage( newSinogram.data(), true );
	sinogramWidget.assignImage( sinogramImg );
	sinogramWidget.changeContrast( PROGRAM_STATE().currentProcessingParameters.projectionsContrast );

}

void processingWindow::recalcFilteredProjections( void ){

	PROGRAM_STATE().deactivateAll();

	Fl_Progress_Window* processingProgressWindow = new Fl_Progress_Window{ (Fl_Window*) this, 20, 5, "Processing progress"};

	PROGRAM_STATE().currentFilteredProjections = FilteredProjections{ PROGRAM_STATE().currentProjections, PROGRAM_STATE().currentProcessingParameters.filterType, processingProgressWindow };

	filterPlot.setLimits( plotLimits{ false, true, PROGRAM_STATE().currentFilteredProjections.filter().GetRelevantRange(), NumberRange{}, 1., pow(PROGRAM_STATE().currentFilteredProjections.resolution().r, 2.)});
	filterPlot.plotRef().assignData( PROGRAM_STATE().currentFilteredProjections.filter().GetPlotValues() );
	filterPlot.assignData();

	filteredProjImage = GrayscaleImage{ PROGRAM_STATE().currentFilteredProjections.data_grid(), true };

	filteredProjWidget.assignImage( filteredProjImage );
	filteredProjWidget.setSliderBoundsFromImage();
	PROGRAM_STATE().currentProcessingParameters.filteredProjectionsContrast = filteredProjWidget.getContrast();

	PROGRAM_STATE().currentReconstrucedImage = ReconstrucedImage{ PROGRAM_STATE().currentFilteredProjections, processingProgressWindow };

	reconstructionImage = GrayscaleImage{ PROGRAM_STATE().currentReconstrucedImage.getGrid(), true };

	reconstructionImageWidget.assignImage( reconstructionImage );
	reconstructionImageWidget.setSliderBoundsFromImage();
	PROGRAM_STATE().currentProcessingParameters.reconstrucedImageContrast = reconstructionImageWidget.getContrast();

	delete processingProgressWindow;

	PROGRAM_STATE().activateAll();

}