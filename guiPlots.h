
#pragma once
/*********************************************************************
 * @file   guiPlot.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   April 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

 #include <FL/Fl_PNG_Image.H>
 #include <FL/Fl_Widget.H>

 #include "image.h"
 #include "sciplot/sciplot.hpp"
 #include "programState.h"
 #include "vectorAlgorithm.h"

class plotInfo{

	public:

	plotInfo( string name_, string xlabel_, string ylabel_ ) : 
		name( name_ ), xlabel( xlabel_ ), ylabel( ylabel_ ),
		autoXRange( true ),
		autoYRange( true )
	{};

	void setXRange( const range newRange ){
		xRange = newRange;
		autoXRange = false;
	}

	void setYRange( const range newRange ){
		yRange = newRange;
		autoYRange = false;
	}

	string name;
	string xlabel;
	string ylabel;

	range xRange;
	range yRange;

	bool autoXRange;
	bool autoYRange;

 };

class linePlot{

	public:
	linePlot( const vector<double> X, const vector<double> Y, const plotInfo info ){

		create( X, Y, info );

	}

	linePlot( const vectorPair pts, const plotInfo info ) :
		linePlot{ pts.first, pts.second, info }
	{}

	linePlot( void ){
		
	}

	void create( const vector<double> X, const vector<double> Y, const plotInfo info ){
		
		sciplot::Plot2D plot;

		plot.xlabel( info.xlabel );
		plot.ylabel( info.ylabel );
		plot.legend().hide();

		//plot.xrange( sourceGrid.Start().col, sourceGrid.End().col );
		//plot.yrange( sourceGrid.Start().row, sourceGrid.End().row );

		if( !info.autoXRange )
			plot.xrange( info.xRange.start, info.xRange.end );
		
		if( !info.autoYRange )
			plot.yrange( info.yRange.start, info.yRange.end );

		//plot.xtics().format("%.2e");
		plot.gnuplot( "set format x '%.e'" );
		plot.fontSize( 12 );
		plot.drawCurve( X, Y );



		sciplot::Figure  fig = { {plot} };
		sciplot::Canvas canvas = { {fig} };

		size_t width = 720;
		canvas.size( width, width * 3 / 8 );

		canvas.save( PROGRAM_STATE().getPath( info.name + ".png" ).string() );

	}

	void create( const vectorPair XY, const plotInfo info ){
		create( XY.first, XY.second, info );
	}


	private:
	//sciplot::Plot2D plot;

};




class Fl_Line_Plot : public Fl_Widget{


	public:

	Fl_Line_Plot( int x, int y, int w, int h,  const char* label = 0L ) :
		Fl_Widget{ x, y, w, h, label },
		plot{},
		sourceImage( nullptr ),
		image( nullptr )
	{

	};

	~Fl_Line_Plot(){
		delete image;
	}

	void assignData( const vectorPair data, const plotInfo info ){

		plot.create( data, info );

		assignImage( PROGRAM_STATE().getPath( info.name + ".png" ) );

	}

	virtual void draw( void ){
		
		calculateScaled();

		if( image != nullptr )
			image->draw( x(), y() );
	
	}

	virtual void resize( int x, int y, int w, int h ){

		Fl_Widget::resize( x, y, w, h );

		calculateScaled();

		redraw();

	}

	void calculateScaled( void ){

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
		if( image == nullptr )
			delete image;
		
		image = sourceImage->copy( scaledWidth, scaledHeight );
	}


	private:

	void assignImage( const path filename ){

		delete sourceImage;
		sourceImage = new Fl_PNG_Image{ filename.string().c_str() };

		draw();
	}

	linePlot plot;
	Fl_PNG_Image* sourceImage;
	Fl_Image* image;
};