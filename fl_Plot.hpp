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
#include "fl_Plot.h"


/*********************************************************************
   Implementations
*********************************************************************/


/*
	Fl_plot implementation
*/

template<class plotType>
Fl_Plot<plotType>::Fl_Plot( int x, int y, int w, int h, const char* label_ ) :
	Fl_Widget{ x, y, w, h, label_ },
	label_( label_ ),
	plot_(),
	raw_image_( nullptr ),
	image_( nullptr ){

}

template<class plotType>
Fl_Plot<plotType>::~Fl_Plot(){
	delete image_;
	delete raw_image_;
}

template<class plotType>
void Fl_Plot<plotType>::Initialise( const path path_, const string xlabel_, const string ylabel_,
					 const PlotLimits limits_, const string xFormat_, const string yFormat_, const bool axisEqual_, const bool grid_ ){

	plot_.Initialise( path_, label_, xlabel_, ylabel_, limits_, GridIndex{ (size_t) Fl_Widget::w(), (size_t) Fl_Widget::h() }, xFormat_, yFormat_, axisEqual_, grid_ );
}

template<class plotType>
void Fl_Plot<plotType>::draw( void ){

	CalculateScaled();

	if( image_ != nullptr )
		image_->draw( x(), y() );

}

template<class plotType>
void Fl_Plot<plotType>::resize( int x, int y, int w, int h ){

	Fl_Widget::resize( x, y, w, h );

	GridIndex currentImgSize = plot_.image_size();
	if( RelativeDeviation( (int) currentImgSize.c, w ) > 0.15 ||
		RelativeDeviation( (int) currentImgSize.r, h ) > 0.15 ){

		plot_.SetSize( GridIndex{ (size_t) w, (size_t) h } );
		plot_.CreatePlot();
		AssignImage( plot_.image_path() );
	} 
	redraw();

}

template<class plotType>
void Fl_Plot<plotType>::CalculateScaled( void ){

	if( raw_image_ == nullptr ) return;

	int scaledWidth = w(), scaledHeight = h();

	const double aspectRatioWidget = (double) w() / (double) h();
	const double aspectRatioImage = (double) raw_image_->w() / (double) raw_image_->h();

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

	if( image_ != nullptr ) delete image_;

	image_ = raw_image_->copy( scaledWidth, scaledHeight );

}

template<class plotType>
void Fl_Plot<plotType>::AssignImage( const path filename ){

	delete raw_image_;

	raw_image_ = new Fl_PNG_Image{ filename.string().c_str() };

	redraw();
}