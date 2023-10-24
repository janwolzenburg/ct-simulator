
#include "processingWindow.h"



void processingWindow::recalcFilteredProjections( void ){

	//Fl_Window::window()->deactivate();
	PROGRAM_STATE().deactivateAll();

	Fl_Progress_Window* processingProgressWindow = new Fl_Progress_Window{ (Fl_Window*) this, 20, 5, "Processing progress"};
	//processingProgressWindow->show();

	PROGRAM_STATE().currentFilteredProjections = filteredProjections{ PROGRAM_STATE().currentProjections, PROGRAM_STATE().currentProcessingParameters.filterType, processingProgressWindow };

	filterPlot.setLimits( plotLimits{ false, true, PROGRAM_STATE().currentFilteredProjections.Filter().getRelevantRange(), range(), 1., 1. } );//pow( PROGRAM_STATE().currentFilteredProjections.Resolution().row, 2. ) });
	filterPlot.plotRef().assignData( PROGRAM_STATE().currentFilteredProjections.Filter().PlotValues() );
	filterPlot.assignData();

	filteredProjImage = monoImage{ PROGRAM_STATE().currentFilteredProjections.getGrid(), true };

	filteredProjWidget.assignImage( filteredProjImage );
	filteredProjWidget.setSliderBoundsFromImage();
	//filteredProjWidget.changeContrast( PROGRAM_STATE().currentProcessingParameters.filteredProjectionsContrast );
	PROGRAM_STATE().currentProcessingParameters.filteredProjectionsContrast = filteredProjWidget.getContrast();

	PROGRAM_STATE().currentReconstrucedImage = reconstrucedImage{ PROGRAM_STATE().currentFilteredProjections, processingProgressWindow };

	reconstructionImage = monoImage{ PROGRAM_STATE().currentReconstrucedImage.getGrid(), true };

	reconstructionImageWidget.assignImage( reconstructionImage );
	//reconstructionImageWidget.changeContrast( PROGRAM_STATE().currentProcessingParameters.reconstrucedImageContrast );
	reconstructionImageWidget.setSliderBoundsFromImage();
	PROGRAM_STATE().currentProcessingParameters.reconstrucedImageContrast = reconstructionImageWidget.getContrast();

	delete processingProgressWindow;

	//Fl_Window::window()->activate();
	PROGRAM_STATE().activateAll();

}