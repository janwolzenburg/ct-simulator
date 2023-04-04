
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
	string name;
	string xlabel;
	string ylabel;

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

		plot.xrange( Min( X ), Max( X ) );
		plot.yrange( Min( Y ), Max( Y ) );

		//plot.xtics().format("%.2e");
		plot.gnuplot( "set format x '%.e'" );
		plot.fontSize( 10 );
		plot.drawCurve( X, Y );



		sciplot::Figure  fig = { {plot} };
		sciplot::Canvas canvas = { {fig} };

		size_t width = 600;
		canvas.size( width, width * 3 / 4 );

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
		image( nullptr )
	{

	};

	~Fl_Line_Plot(){
		delete image;
	}

	void assignData( const vectorPair data, const plotInfo info ){

		plot.create( data, info );

		assignImage( info.name );

	}

	virtual void draw( void ){
		
		image->draw( x(), y() );

	}

	//virtual void resize( int x, int y, int w, int h );

	//void calculateScaled( void );


	private:

	void assignImage( const string filename ){

		Fl_PNG_Image tempImage{ filename.c_str() };

		delete image;
		image = tempImage.copy( Fl_Widget::w(), Fl_Widget::h() );

		draw();
	}

	linePlot plot;
	Fl_Image* image;
	//greyImage scaledImage;
};