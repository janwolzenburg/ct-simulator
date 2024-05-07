#pragma once
/*********************************************************************
 * @file   fl_Plot.cpp
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
void Fl_Plot<plotType>::Initialise( const path image_path, const string x_label, const string y_label,
					 const PlotLimits limits, const string x_format, const string y_format, const bool axis_equal, const bool grid ){

	plot_.Initialise( image_path, label_, x_label, y_label, limits, GridIndex{ static_cast<size_t>( Fl_Widget::w() ), static_cast<size_t>( Fl_Widget::h() ) }, x_format, y_format, axis_equal, grid );
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

	GridIndex current_image_size = plot_.image_size();
	if( RelativeDeviation( static_cast<int>( current_image_size.c ), w ) > 0.15 ||
		RelativeDeviation( static_cast<int>( current_image_size.r ), h ) > 0.15 ){

		plot_.SetSize( GridIndex{ static_cast<size_t>( w ), static_cast<size_t>( h ) } );
		plot_.CreatePlot();
		AssignImage( plot_.image_path() );
	} 
	redraw();

}

template<class plotType>
void Fl_Plot<plotType>::CalculateScaled( void ){

	if( raw_image_.get() == nullptr ) return;

	int scaled_width = w(), scaled_height = h();

	const double aspect_ratio_widget = static_cast<double>( w() ) / static_cast<double>( h() );
	const double aspect_ratio_image = static_cast<double>( raw_image_->w() ) / static_cast<double>( raw_image_->h() );

	// fit image vertically
	if( aspect_ratio_widget > aspect_ratio_image ){

		scaled_height = h();
		scaled_width = static_cast<int>( static_cast<double>( scaled_height ) * aspect_ratio_image );

	}
	// fit image horizontally
	else{

		scaled_width = w();
		scaled_height = static_cast<int>( static_cast<double>( scaled_width ) / aspect_ratio_image );

	}
	image_->~Fl_Image();
	image_ = std::unique_ptr<Fl_Image>( raw_image_->copy( scaled_width, scaled_height ) );

}

template<class plotType>
void Fl_Plot<plotType>::AssignImage( const path filename ){

	raw_image_->~Fl_PNG_Image();
	raw_image_ = std::make_unique<Fl_PNG_Image>( filename.string().c_str() );

	redraw();
}