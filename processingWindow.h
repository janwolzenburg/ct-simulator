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
		
		newRTFlag( false ),
		sinogramGrp(		X( *this, .01 ),		Y( *this, .01 ),		W( *this, .49 ),			H( *this, .325 ) ),
 		sinogramWidget(		X( sinogramGrp, .0 ),	Y( sinogramGrp, .0 ),	W( sinogramGrp, 1. ),	H( sinogramGrp, 1. ) ),
		
		
		filterChanged( false ),
		filterGrp(			X( *this, .01 ),		vOff( sinogramGrp ) + Y( *this, 0.025 ), W( *this, .49 ), H( *this, .29  ) ),
		filterTypeSelector( X( filterGrp, 0. ), Y( filterGrp, 0. ), W( filterGrp, .3 ), H( filterGrp, .1 ), "Filter type" ),
		filterPlot(			X( filterGrp, 0. ), Y( filterGrp, 0.15 ),		W( filterGrp, 1. ), H( filterGrp, .85 ), "Filter" ),
		//filterPlot(			"Filter", "n", "", plotLimits(), idx2CR( 720, 480 ), true ),
		
		filteredProjGrp( X( *this, .01 ), vOff( filterGrp ) + Y( *this, 0.025 ), W( *this, .49 ), H( *this, .325 ) ),
		filteredProjWidget( X( filteredProjGrp, .0 ), Y( filteredProjGrp, .0 ), W( filteredProjGrp, 1. ), H( filteredProjGrp, 1. ) ),

		reconstructionGrp( X( *this, .51 ), Y( *this, 0.01 ), W( *this, .48 ), H( *this, .8 ) ),
		reconstructionImageWidget( X( reconstructionGrp, .0 ), Y( reconstructionGrp, .0 ), W( reconstructionGrp, 1. ), H( reconstructionGrp, 1. ) )

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
		string filterName = discreteFilter::filterTypes.at( PROGRAM_STATE().currentProcessingParameters.filterType );
		filterTypeSelector.value( filterName );


		filterPlot.initializePlot( PROGRAM_STATE().getPath( "filterPlot.png" ), "n", "", plotLimits{ true, true }, "", "", false, true );



		Fl_Window::add( filteredProjGrp );

		filteredProjGrp.add( filteredProjWidget );
		filteredProjWidget.box( FL_BORDER_BOX );

		Fl_Window::add( reconstructionGrp );

		reconstructionGrp.add( reconstructionImageWidget );
		reconstructionImageWidget.box( FL_BORDER_BOX );
	}

	void setNewRTFlag( void ){ newRTFlag = true; };

	void deactivate( void ){
		Fl_Window::deactivate();
		sinogramGrp.deactivate();
		filterGrp.deactivate();
		filteredProjGrp.deactivate();
		reconstructionGrp.deactivate();
	}

	void activate( void ){
		Fl_Window::activate();
		sinogramGrp.activate();
		filterGrp.activate();
		filteredProjGrp.activate();
		reconstructionGrp.activate();
	}


	void handleEvents( void ){

		if( PROGRAM_STATE().RadonTransformedLoaded() && newRTFlag ){

			newRTFlag = false;

			assignSinogram( PROGRAM_STATE().currentProjections );

			recalcFilteredProjections();

			Fl_Window::show();
		}

		if( sinogramWidget.handleEvents() ){
			// Store contrast
			 PROGRAM_STATE().currentProcessingParameters.projectionsContrast = sinogramWidget.getContrast();
		}


		if( filterChanged ){
			filterChanged = false;

			PROGRAM_STATE().currentProcessingParameters.filterType = discreteFilter::getEnum( filterTypeSelector.value() );

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
	
	void recalcFilteredProjections( void );

	
	private:

	void assignSinogram( const radonTransformed newSinogram ){

		//sinogram = newSinogram;
		sinogramImg = monoImage( newSinogram.Data(), true );

		sinogramWidget.assignImage( sinogramImg );

		sinogramWidget.changeContrast( PROGRAM_STATE().currentProcessingParameters.projectionsContrast );

		//filteredProjImage = monoImage(  )

	}


	public:

	
	bool newRTFlag;
	Fl_Group sinogramGrp;
	monoImage sinogramImg;
	Fl_GridImage_Adjust sinogramWidget;
	
	bool filterChanged;
	Fl_Group filterGrp;
	
	Fl_Selector filterTypeSelector;
	Fl_Plot<dotplot> filterPlot;

	Fl_Group filteredProjGrp;
	monoImage filteredProjImage;
	Fl_GridImage_Adjust filteredProjWidget;

	Fl_Group reconstructionGrp;
	monoImage reconstructionImage;
	Fl_GridImage_Adjust reconstructionImageWidget;


 };