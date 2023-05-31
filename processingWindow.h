#pragma once
/*********************************************************************
 * @file   processingWindow.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   May 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

 #include "widgets.h"

 #include "FL/Fl_Window.H"

 #include "Fl_GridImage.h"
 #include "monoImage.h"
 #include "Fl_Lineplot.h"

 #include "programState.h"
 #include "radonTransform.h"
 #include "backprojection.h"

class processingView : public Fl_Window{

	public:

	processingView( int w, int h, const char* label ) :
		Fl_Window( w, h, label ),
		
		sinogramGrp(		X( *this, .05 ),		Y( *this, .05 ),		W( *this, .4 ),			H( *this, .4 ) ),
 		sinogramWidget(		X( sinogramGrp, .0 ),	Y( sinogramGrp, .0 ),	W( sinogramGrp, 1. ),	H( sinogramGrp, 1. ) ),
		

		filterGrp(			X( *this, .05 ),		vOff( sinogramGrp ),	W( *this, .4 ),			H( *this, .4 ) ),
		filterType( discreteFilter::TYPE::constant ),
		filterPlot(			X( filterGrp, 0. ),		Y( filterGrp, 0. ),		W( filterGrp, 1. ), H	( filterGrp, 1. ), "Filter" ),
		//filterPlot(			"Filter", "n", "", plotLimits(), idx2CR( 720, 480 ), true ),
		

		newRTFlag( false )
	{
		Fl_Window::add( sinogramGrp );
		Fl_Window::resizable( *this );

		sinogramGrp.add( sinogramWidget );
		sinogramWidget.box( FL_BORDER_BOX );
	
		Fl_Window::add( filterGrp );
		filterGrp.add( filterPlot );
	
		filterPlot.initializePlot( PROGRAM_STATE().getPath( "filterPlot.png" ), "n", "", plotLimits{ true, true }, "", "", false, true );

	}

	inline void setNewRTFlag( void ){ newRTFlag = true; };

	void handleEvents( void ){

		if( PROGRAM_STATE().RadonTransformedLoaded() && newRTFlag ){

			newRTFlag = false;

			assignSinogram( PROGRAM_STATE().Projections() );

			PROGRAM_STATE().FilteredProjections() = filteredProjections( PROGRAM_STATE().Projections(), filterType );
			filterPlot.plotRef().assignData( PROGRAM_STATE().FilteredProjections().Filter().PlotValues() );
			filterPlot.assignData();

			Fl_Window::show();
		}

		sinogramWidget.handleEvents();

	}
	
	
	
	private:

	void assignSinogram( const radonTransformed newSinogram ){

		//sinogram = newSinogram;
		sinogramImg = monoImage( newSinogram.Data(), true );

		sinogramWidget.assignImage( sinogramImg );

	}


	public:

	//radonTransformed sinogram;
	bool newRTFlag;

	Fl_Group sinogramGrp;
	monoImage sinogramImg;
	//Fl_GridImage sinogramWidget;
	Fl_GridImage_Adjust sinogramWidget;

	Fl_Group filterGrp;
	
	discreteFilter::TYPE filterType;
	Fl_Plot<linePlot> filterPlot;

	//filteredProjections projectionsFilt;

 };