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



/*********************************************************************
   Implementations
*********************************************************************/


/*
	plot implementation
*/

void plot::initialize( const path path_, const string xlabel_, const string ylabel_,
				 const plotLimits limits_, const idx2CR imgSize_, const string xFormat_, const string yFormat_, const bool axisEqual_, const bool grid_ ){

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

void plot::setSize( const idx2CR size ){
	imgSize = size;
}

idx2CR plot::getSize( void ) const{
	return imgSize;
}

void plot::redraw( void ){
	reset();
	drawPlot();
}

plot::plot( const path imgPath_, const string xlabel_, const string ylabel_,
		const plotLimits limits_, const idx2CR imgSize_, const bool grid_ ) :

	imagePath( imgPath_ ),
	xlabel( xlabel_ ), ylabel( ylabel_ ),
	limits( limits_ ), imgSize( imgSize_ ),
	grid( grid_ ){
	reset();
}

plot::plot( void ) :
	plot( "Dummy", "X", "Y", plotLimits{}, idx2CR{ 5, 5 }, false ){}

void plot::reset( void ){
	plot2D.cleanup();
	plot2D.clear();
}

void plot::drawPlot( void ){

	plot2D.xlabel( xlabel ); plot2D.ylabel( ylabel );
	plot2D.legend().hide();

	if( !limits.autoXRange )
		plot2D.xrange( limits.xRange.start, limits.xRange.end );

	if( !limits.autoYRange )
		plot2D.yrange( limits.yRange.start, limits.yRange.end );

	if( !xFormat.empty() )
		plot2D.gnuplot( "set format x '" + xFormat + "'" );

	if( !yFormat.empty() )
		plot2D.gnuplot( "set format y '" + yFormat + "'" );

	if( axisEqual )
		plot2D.gnuplot( "set size ratio - 1" );

	if( grid )
		plot2D.grid().show();


	plot2D.palette( "set1" );

	plot2D.fontSize( imgSize.row / 20 );

	sciplot::Figure  fig = { {plot2D} };
	sciplot::Canvas canvas = { {fig} };

	canvas.size( (size_t) ( imgSize.col ), (size_t) ( imgSize.row ) );
	canvas.save( imagePath.string() );

}