/*********************************************************************
 * @file   plots.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   April 2023
 *********************************************************************/



 /*********************************************************************
   Includes
*********************************************************************/

#include "plot.h"
#include "linePlot.h"
#include "geometryPlot.h"
#include "fl_GrayscaleImage.h"


/*********************************************************************
   Implementations
*********************************************************************/


/*
	plot implementation
*/

void Plot::Initialise( const path path, const string label, const string xlabel, const string ylabel,
				 const PlotLimits limits, const GridIndex imgSize, const string xFormat, const string yFormat, const bool axisEqual, const bool grid ){

	label_ = label;
	image_path_ = path;
	x_label_ = xlabel;
	y_label_ = ylabel;
	limits_ = limits;
	image_size_ = imgSize;
	x_format_ = xFormat;
	y_format_ = yFormat;
	are_axis_equal_ = axisEqual;
	this->enable_grid_ = grid;

	reset();

}

Plot::Plot( const path imgPath_, const string xlabel_, const string ylabel_,
		const PlotLimits limits, const GridIndex imgSize_, const bool grid_ ) :
	image_path_( imgPath_ ),
	x_label_( xlabel_ ), y_label_( ylabel_ ),
	enable_grid_( grid_ ),
	limits_( limits ),
	image_size_( imgSize_ )
{
	reset();
}

void Plot::reset( void ){
	plot_2D_.cleanup();
	plot_2D_.clear();
}

void Plot::DrawPlot( void ){


	plot_2D_.xlabel( x_label_ ); plot_2D_.ylabel( y_label_ );
	
	
	RGB color = Fl_GrayscaleImage::background_color;

	char colorBuffer[32];
	snprintf( colorBuffer, 32, "#%2X%2X%2X", color.red, color.green, color.blue );
	string colorString{ colorBuffer };

	plot_2D_.gnuplot( "set object 1 rectangle from screen 0,0 to screen 1,1 fillcolor rgb \"" + colorString + "\" behind ");

	if( !limits_.autoXRange )
		plot_2D_.xrange( limits_.xRange.start(), limits_.xRange.end() );

	if( !limits_.autoYRange )
		plot_2D_.yrange( limits_.yRange.start(), limits_.yRange.end() );

	if( !x_format_.empty() )
		plot_2D_.gnuplot( "set format x '" + x_format_ + "'" );

	if( !y_format_.empty() )
		plot_2D_.gnuplot( "set format y '" + y_format_ + "'" );

	if( are_axis_equal_ )
		plot_2D_.gnuplot( "set size ratio - 1" );

	if( enable_grid_ )
		plot_2D_.grid().show();

	plot_2D_.palette( "set1" );
	plot_2D_.fontSize( image_size_.r / 18 );
	plot_2D_.legend().fontSize( image_size_.r / 18 );

	if( !label_.empty() ){
		plot_2D_.gnuplot( "set title \"" + label_ + "\"" );
		plot_2D_.gnuplot( "show title" );
	}

	sciplot::Figure  fig = { {plot_2D_} };
	sciplot::Canvas canvas = { {fig} };

	canvas.size( static_cast<size_t>( image_size_.c ), static_cast<size_t>( image_size_.r ) );
	canvas.save( image_path_.string() );

}


/*
	lineplot implementation
*/

LinePlot::LinePlot( const string name, const string xlabel_, const string ylabel_,
			const PlotLimits limits, const GridIndex imgSize_, const bool grid_ ) :
	Plot{ name, xlabel_, ylabel_, limits, imgSize_, grid_ },
	X(), Y()
{}

LinePlot::LinePlot( void ) :
	Plot{}, X(), Y()
{}

void LinePlot::AssignData( const vector<double> X_, const vector<double> Y_ ){

	X = X_;
	Y = Y_;

	if( limits_.xFactor != 1. ){
		for( auto& x : X ){
			x *= limits_.xFactor;
		}
	}

	if( limits_.yFactor != 1. ){
		for( auto& y : Y ){
			y *= limits_.yFactor;
		}
	}

	CreatePlot();
}

void LinePlot::CreatePlot( void ){
	Plot::reset();
	Plot::plot_2D_.drawCurve( X, Y ).labelNone();
	Plot::DrawPlot();
}

void DotPlot::CreatePlot( void ){
	Plot::reset();
	Plot::plot_2D_.drawPoints( X, Y ).labelNone();
	Plot::DrawPlot();
}


/*
	geoplot implementation
*/

void Geometryplot::ResetObjects( void ){
	lines_.clear();
	points_.clear();
}

void Geometryplot::CreatePlot( void ){

	Plot::reset();

	for( const auto& Line : lines_ ){

		vector<double> X{ Line.first.x, Line.second.x };
		vector<double> Y{ Line.first.y, Line.second.y };

		Plot::plot_2D_.drawCurve( X, Y ).labelNone();
	}

	for( const auto& point : points_ ){

		vector<double> X{ point.first.x };
		vector<double> Y{ point.first.y };

		Plot::plot_2D_.drawPoints( X, Y ).label( point.second.c_str() );
	}

	Plot::DrawPlot();
}
