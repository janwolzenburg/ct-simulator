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
void Fl_Plot<plotType>::Initialise( const path path_, const string xlabel_, const string ylabel_,
					 const PlotLimits limits_, const string xFormat_, const string yFormat_, const bool axisEqual_, const bool grid_ ){

	plot_.Initialise( path_, label_, xlabel_, ylabel_, limits_, GridIndex{ static_cast<size_t>( Fl_Widget::w() ), static_cast<size_t>( Fl_Widget::h() ) }, xFormat_, yFormat_, axisEqual_, grid_ );
}

template<class plotType>
void Fl_Plot<plotType>::draw( void ){

	CalculateScaled();

	if( image_.get() != nullptr )
		image_->draw( x(), y() );

}

template<class plotType>
void Fl_Plot<plotType>::resize( int x, int y, int w, int h ){

	Fl_Widget::resize( x, y, w, h );

	GridIndex currentImgSize = plot_.image_size();
	if( RelativeDeviation( static_cast<int>( currentImgSize.c ), w ) > 0.15 ||
		RelativeDeviation( static_cast<int>( currentImgSize.r ), h ) > 0.15 ){

		plot_.SetSize( GridIndex{ static_cast<size_t>( w ), static_cast<size_t>( h ) } );
		plot_.CreatePlot();
		AssignImage( plot_.image_path() );
	} 
	redraw();

}

template<class plotType>
void Fl_Plot<plotType>::CalculateScaled( void ){

	if( raw_image_.get() == nullptr ) return;

	int scaledWidth = w(), scaledHeight = h();

	const double aspectRatioWidget = static_cast<double>( w() ) / static_cast<double>( h() );
	const double aspectRatioImage = static_cast<double>( raw_image_->w() ) / static_cast<double>( raw_image_->h() );

	// fit image vertically
	if( aspectRatioWidget > aspectRatioImage ){

		scaledHeight = h();
		scaledWidth = static_cast<int>( static_cast<double>( scaledHeight ) * aspectRatioImage );

	}
	// fit image horizontally
	else{

		scaledWidth = w();
		scaledHeight = static_cast<int>( static_cast<double>( scaledWidth ) / aspectRatioImage );

	}

	image_ = std::unique_ptr<Fl_Image>( raw_image_->copy( scaledWidth, scaledHeight ) );

}

template<class plotType>
void Fl_Plot<plotType>::AssignImage( const path filename ){


	raw_image_ = std::make_unique<Fl_PNG_Image>( filename.string().c_str() );

	redraw();
}