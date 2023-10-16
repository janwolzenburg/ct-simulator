
#include "processingWindow.h"



void processingView::recalcFilteredProjections( void ){

	//Fl_Window::window()->deactivate();
	PROGRAM_STATE().deactivateAll();

	Fl_Progress_Window* processingProgressWindow = new Fl_Progress_Window{ (Fl_Window*) this, 20, 5, "Processing progress"};
	//processingProgressWindow->show();

	PROGRAM_STATE().FilteredProjections() = filteredProjections{ PROGRAM_STATE().Projections(), PROGRAM_STATE().ProcessingParameters().filterType, processingProgressWindow };

	filterPlot.setLimits( plotLimits{ false, true, PROGRAM_STATE().FilteredProjections().Filter().getRelevantRange(), range(), 1., 1. } );//pow( PROGRAM_STATE().FilteredProjections().Resolution().row, 2. ) });
	filterPlot.plotRef().assignData( PROGRAM_STATE().FilteredProjections().Filter().PlotValues() );
	filterPlot.assignData();

	filteredProjImage = monoImage{ PROGRAM_STATE().FilteredProjections().getGrid(), true };

	filteredProjWidget.assignImage( filteredProjImage );
	filteredProjWidget.setSliderBoundsFromImage();
	//filteredProjWidget.changeContrast( PROGRAM_STATE().ProcessingParameters().filteredProjectionsContrast );
	PROGRAM_STATE().ProcessingParameters().filteredProjectionsContrast = filteredProjWidget.getContrast();

	PROGRAM_STATE().ReconstrucedImage() = reconstrucedImage{ PROGRAM_STATE().FilteredProjections(), processingProgressWindow };

	reconstructionImage = monoImage{ PROGRAM_STATE().ReconstrucedImage().getGrid(), true };

	reconstructionImageWidget.assignImage( reconstructionImage );
	//reconstructionImageWidget.changeContrast( PROGRAM_STATE().ProcessingParameters().reconstrucedImageContrast );
	reconstructionImageWidget.setSliderBoundsFromImage();
	PROGRAM_STATE().ProcessingParameters().reconstrucedImageContrast = reconstructionImageWidget.getContrast();

	delete processingProgressWindow;

	//Fl_Window::window()->activate();
	PROGRAM_STATE().activateAll();

}