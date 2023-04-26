#pragma once
/*********************************************************************
 * @file   Fl_plots.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   April 2023
 *********************************************************************/



 /*********************************************************************
   Includes
*********************************************************************/

#include "generelMath.h"
#include "Fl_plots.h"



/*********************************************************************
   Implementations
*********************************************************************/


/*
	Fl_plot implementation
*/

template<class plotType>
Fl_Plot<plotType>::Fl_Plot( int x, int y, int w, int h, const char* label_ ) :
	Fl_Widget( x, y, w, h, label_ ),
	label( label_ ),
	plotInstance(),
	sourceImage( nullptr ),
	image( nullptr ){

}

template<class plotType>
Fl_Plot<plotType>::~Fl_Plot(){
	delete image;
}

template<class plotType>
void Fl_Plot<plotType>::initializePlot( const path path_, const string xlabel_, const string ylabel_,
					 const plotLimits limits_, const string xFormat_, const string yFormat_, const bool axisEqual_, const bool grid_ ){

	plotInstance.initialize( path_, label, xlabel_, ylabel_, limits_, idx2CR{ (size_t) Fl_Widget::w(), (size_t) Fl_Widget::h() }, xFormat_, yFormat_, axisEqual_, grid_ );

}

template<class plotType>
void Fl_Plot<plotType>::assignData( void ){

	assignImage( plotInstance.getImgPath() );

}

template<class plotType>
void Fl_Plot<plotType>::draw( void ){

	calculateScaled();

	if( image != nullptr )
		image->draw( x(), y() );

}

template<class plotType>
void Fl_Plot<plotType>::resize( int x, int y, int w, int h ){

	Fl_Widget::resize( x, y, w, h );

	idx2CR currentImgSize = plotInstance.getSize();
	if( relDeviation( (int) currentImgSize.col, w ) > 0.15 ||
		relDeviation( (int) currentImgSize.row, h ) > 0.15 ){

		plotInstance.setSize( idx2CR{ (size_t) w, (size_t) h } );
		plotInstance.create();
		assignImage( plotInstance.getImgPath() );
	}
	//calculateScaled();

	redraw();

}

template<class plotType>
void Fl_Plot<plotType>::calculateScaled( void ){

	if( sourceImage == nullptr ) return;

	int scaledWidth = w(), scaledHeight = h();

	const double aspectRatioWidget = (double) w() / (double) h();
	const double aspectRatioImage = (double) sourceImage->w() / (double) sourceImage->h();

	// Fit image vertically
	if( aspectRatioWidget > aspectRatioImage ){

		scaledHeight = h();
		scaledWidth = (int) ( (double) scaledHeight * aspectRatioImage );

	}
	// Fit image horizontally
	else{

		scaledWidth = w();
		scaledHeight = (int) ( (double) scaledWidth / aspectRatioImage );

	}

	if( image != nullptr ) delete image;

	image = sourceImage->copy( scaledWidth, scaledHeight );

}

template<class plotType>
void Fl_Plot<plotType>::assignImage( const path filename ){

	delete sourceImage;
	sourceImage = new Fl_PNG_Image{ filename.string().c_str() };

	draw();
}