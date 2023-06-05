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
 #include "lineplot.h"

 #include "programState.h"
 #include "radonTransform.h"
 #include "backprojection.h"



class processingView : public Fl_Window{

	public:

	processingView( int w, int h, const char* label ) :
		Fl_Window( w, h, label ),
		
		sinogramGrp(		X( *this, .01 ),		Y( *this, .01 ),		W( *this, .49 ),			H( *this, .325 ) ),
 		sinogramWidget(		X( sinogramGrp, .0 ),	Y( sinogramGrp, .0 ),	W( sinogramGrp, 1. ),	H( sinogramGrp, 1. ) ),
		

		filterGrp(			X( *this, .01 ),		vOff( sinogramGrp ) + Y( *this, 0.025 ), W( *this, .49 ), H( *this, .29  ) ),
		filterTypeSelector( X( filterGrp, 0. ), Y( filterGrp, 0. ), W( filterGrp, .3 ), H( filterGrp, .1 ), "Filter type" ),
		filterPlot(			X( filterGrp, 0. ), Y( filterGrp, 0.15 ),		W( filterGrp, 1. ), H( filterGrp, .85 ), "Filter" ),
		//filterPlot(			"Filter", "n", "", plotLimits(), idx2CR( 720, 480 ), true ),
		
		filteredProjGrp( X( *this, .01 ), vOff( filterGrp ) + Y( *this, 0.025 ), W( *this, .49 ), H( *this, .325 ) ),
		filteredProjWidget( X( filteredProjGrp, .0 ), Y( filteredProjGrp, .0 ), W( filteredProjGrp, 1. ), H( filteredProjGrp, 1. ) ),

		newRTFlag( false ),
		filterChanged( false )
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
		for( auto& el : discreteFilter::filterTypes ) filterNames.push_back( el.second );

		filterTypeSelector.setElements( filterNames );
		string filterName = discreteFilter::filterTypes.at( PROGRAM_STATE().ProcessingParameters().filterType );
		filterTypeSelector.value( filterName );


		filterPlot.initializePlot( PROGRAM_STATE().getPath( "filterPlot.png" ), "n", "", plotLimits{ true, true }, "", "", false, true );



		Fl_Window::add( filteredProjGrp );

		filteredProjGrp.add( filteredProjWidget );
		filteredProjWidget.box( FL_BORDER_BOX );
	}

	inline void setNewRTFlag( void ){ newRTFlag = true; };

	void handleEvents( void ){

		if( PROGRAM_STATE().RadonTransformedLoaded() && newRTFlag ){

			newRTFlag = false;

			assignSinogram( PROGRAM_STATE().Projections() );

			recalcFilteredProjections();

			Fl_Window::show();
		}

		if( sinogramWidget.handleEvents() ){
			// Store contrast
			 PROGRAM_STATE().ProcessingParameters().projectionsContrast = sinogramWidget.getContrast();
		}


		if( filterChanged ){
			filterChanged = false;

			PROGRAM_STATE().ProcessingParameters().filterType = discreteFilter::getEnum( filterTypeSelector.value() );

			recalcFilteredProjections();

		}

		if( filteredProjWidget.handleEvents() ){

			if( filteredProjWidget.imageAssigned() )
				PROGRAM_STATE().ProcessingParameters().filteredProjectionsContrast = filteredProjWidget.getContrast();
		}

	}
	
	void recalcFilteredProjections( void ){

		PROGRAM_STATE().FilteredProjections() = filteredProjections( PROGRAM_STATE().Projections(), PROGRAM_STATE().ProcessingParameters().filterType );
		
		filterPlot.setLimits( plotLimits{ false, true, PROGRAM_STATE().FilteredProjections().Filter().getRelevantRange(), range(), 1., pow( PROGRAM_STATE().FilteredProjections().Resolution().row, 2. )});
		filterPlot.plotRef().assignData( PROGRAM_STATE().FilteredProjections().Filter().PlotValues() );
		filterPlot.assignData();

		filteredProjImage = monoImage( PROGRAM_STATE().FilteredProjections().getGrid(), true );

		filteredProjWidget.assignImage( filteredProjImage );
		filteredProjWidget.changeContrast( PROGRAM_STATE().ProcessingParameters().filteredProjectionsContrast );

	}

	
	private:

	void assignSinogram( const radonTransformed newSinogram ){

		//sinogram = newSinogram;
		sinogramImg = monoImage( newSinogram.Data(), true );

		sinogramWidget.assignImage( sinogramImg );

		sinogramWidget.changeContrast( PROGRAM_STATE().ProcessingParameters().projectionsContrast );

		//filteredProjImage = monoImage(  )

	}


	public:

	bool newRTFlag;
	bool filterChanged;

	Fl_Group sinogramGrp;
	monoImage sinogramImg;
	Fl_GridImage_Adjust sinogramWidget;

	Fl_Group filterGrp;
	
	Fl_Selector filterTypeSelector;
	Fl_Plot<dotplot> filterPlot;

	Fl_Group filteredProjGrp;
	monoImage filteredProjImage;
	Fl_GridImage_Adjust filteredProjWidget;


 };