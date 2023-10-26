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

#include "plots.h"
#include "lineplot.h"
#include "geoplot.h"



/*********************************************************************
   Implementations
*********************************************************************/


/*
	plot implementation
*/

void plot::initialise( const path path_, const string label_, const string xlabel_, const string ylabel_,
				 const plotLimits limits_, const GridIndex imgSize_, const string xFormat_, const string yFormat_, const bool axisEqual_, const bool grid_ ){

	label = label_;
	imagePath = path_;
	xlabel = xlabel_;
	ylabel = ylabel_;
	limits = limits_;
	imgSize = imgSize_;
	xFormat = xFormat_;
	yFormat = yFormat_;
	axisEqual = axisEqual_;
	this->grid = grid_;

	reset();

}

plot::plot( const path imgPath_, const string xlabel_, const string ylabel_,
		const plotLimits limits_, const GridIndex imgSize_, const bool grid_ ) :
	imagePath( imgPath_ ),
	xlabel( xlabel_ ), ylabel( ylabel_ ),
	grid( grid_ ),
	limits( limits_ ),
	imgSize( imgSize_ )
{
	reset();
}

void plot::reset( void ){
	plot2D.cleanup();
	plot2D.clear();
}

void plot::drawPlot( void ){

	plot2D.xlabel( xlabel ); plot2D.ylabel( ylabel );
	plot2D.legend().hide();

	if( !limits.autoXRange )
		plot2D.xrange( limits.xRange.start(), limits.xRange.end() );

	if( !limits.autoYRange )
		plot2D.yrange( limits.yRange.start(), limits.yRange.end() );

	if( !xFormat.empty() )
		plot2D.gnuplot( "set format x '" + xFormat + "'" );

	if( !yFormat.empty() )
		plot2D.gnuplot( "set format y '" + yFormat + "'" );

	if( axisEqual )
		plot2D.gnuplot( "set size ratio - 1" );

	if( grid )
		plot2D.grid().show();

	plot2D.palette( "set1" );
	plot2D.fontSize( imgSize.r / 20 );

	if( !label.empty() ){
		plot2D.gnuplot( "set title \"" + label + "\"" );
		plot2D.gnuplot( "show title" );
	}

	sciplot::Figure  fig = { {plot2D} };
	sciplot::Canvas canvas = { {fig} };

	canvas.size( (size_t) ( imgSize.c ), (size_t) ( imgSize.r ) );
	canvas.save( imagePath.string() );

}


/*
	lineplot implementation
*/

lineplot::lineplot( const string name_, const string xlabel_, const string ylabel_,
			const plotLimits limits_, const GridIndex imgSize_, const bool grid_ ) :
	plot{ name_, xlabel_, ylabel_, limits_, imgSize_, grid_ },
	X(), Y()
{}

lineplot::lineplot( void ) :
	plot{}, X(), Y()
{}

void lineplot::assignData( const vector<double> X_, const vector<double> Y_ ){

	X = X_;
	Y = Y_;

	if( limits.xFactor != 1. ){
		for( auto& x : X ){
			x *= limits.xFactor;
		}
	}

	if( limits.yFactor != 1. ){
		for( auto& y : Y ){
			y *= limits.yFactor;
		}
	}

	create();
}

void lineplot::create( void ){
	plot::reset();
	plot::plot2D.drawCurve( X, Y );
	plot::drawPlot();
}

void dotplot::create( void ){
	plot::reset();
	plot::plot2D.drawPoints( X, Y );
	plot::drawPlot();
}


/*
	geoplot implementation
*/

void geoplot::resetObjects( void ){
	lines.clear();
	points.clear();
}

void geoplot::create( void ){

	plot::reset();

	for( const auto& line : lines ){

		vector<double> X{ line.first.x, line.second.x };
		vector<double> Y{ line.first.y, line.second.y };

		plot::plot2D.drawCurve( X, Y );
	}

	for( const auto& point : points ){

		vector<double> X{ point.x };
		vector<double> Y{ point.y };

		plot::plot2D.drawPoints( X, Y );
	}

	plot::drawPlot();
}
