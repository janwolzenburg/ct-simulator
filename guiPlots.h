
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


class plotLimits{

	public:

	range xRange;
	range yRange;

	bool autoXRange;
	bool autoYRange;

 };


class plot{

	public:

	inline string getImgPath( void ) const { return imgPath; };

	void initialize( const string name_, const string xlabel_, const string ylabel_,
					 const plotLimits limits_, const idx2CR imgSize_, const string xFormat_, const string yFormat_, const bool axisEqual_ ){

		name = name_;
		imgPath = PROGRAM_STATE().getPath( name + ".png" ).string();
		xlabel = xlabel_;
		ylabel = ylabel_;
		limits = limits_;
		imgSize = imgSize_;
		xFormat = xFormat_;
		yFormat = yFormat_;
		axisEqual = axisEqual_;

		reset();

	}


	protected:

	plot( const string name_, const string xlabel_, const string ylabel_,
		  const plotLimits limits_, const idx2CR imgSize_ ) :

		name( name_ ), imgPath( PROGRAM_STATE().getPath( name + ".png" ).string() ),
		xlabel( xlabel_ ), ylabel( ylabel_ ),
		limits( limits_ ), imgSize( imgSize_ )
	{
		reset();
	}

	plot( void ) :
		plot( "Dummy", "X", "Y", plotLimits{ { 0, 1 }, { 0, 1 }, true, true }, idx2CR{ 5, 5 } )
	{
	}

	void reset( void ){
		plot2D.cleanup();
		plot2D.clear();
	}

	void drawPlot( void ){

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
		

		plot2D.palette( "set1" );

		plot2D.fontSize( imgSize.row / 20 );

		sciplot::Figure  fig = { {plot2D} };
		sciplot::Canvas canvas = { {fig} };

		canvas.size( (size_t) ( 1.5 * imgSize.col ), (size_t) ( 1.5 * imgSize.row ) );
		canvas.save( imgPath );

	}

	
	protected:
	
	string name;
	string xlabel;
	string ylabel;

	bool axisEqual = false;

	string xFormat;
	string yFormat;

	plotLimits limits;

	idx2CR imgSize;
	string imgPath;
	sciplot::Plot2D plot2D;

 };


class linePlot : public plot{

	public:

	linePlot( const string name_, const string xlabel_, const string ylabel_,
			  const plotLimits limits_, const idx2CR imgSize_ ) : 
		plot( name_, xlabel_, ylabel_, limits_, imgSize_ ),
		X(), Y()
	{
	
	}
	
	linePlot( void ) : 
		plot(),
		X(), Y()
	{
	}

	void assignData( const vector<double> X_, const vector<double> Y_ ){

		X = X_;
		Y = Y_;

		create();
	}


	void assignData( const vectorPair XY){

		assignData( XY.first, XY.second );

	}

	void create( void ) {

		plot::reset();
		plot::plot2D.drawCurve( X, Y );
		plot::drawPlot();

	}


	private:

	vector<double> X, Y;

 };


template< class plotType >
class Fl_Plot : public Fl_Widget{


	public:

	Fl_Plot( int x, int y, int w, int h,  const char* label = 0L ) :
		Fl_Widget( x, y, w, h, label ),
		plotInstance(),
		sourceImage( nullptr ),
		image( nullptr )
	{

	}

	~Fl_Plot(){
		delete image;
	}

	void initializePlot( const string name_, const string xlabel_, const string ylabel_,
						 const plotLimits limits_, const string xFormat_, const string yFormat_, const bool axisEqual_ ){

		plotInstance.initialize( name_, xlabel_, ylabel_, limits_, idx2CR{ (size_t) Fl_Widget::w(), (size_t) Fl_Widget::h() }, xFormat_, yFormat_, axisEqual_ );

	}

	inline plotType& plotRef( void ){ return plotInstance; };

	void assignData( void ){

		assignImage( plotInstance.getImgPath() );

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

	plotType plotInstance;
	Fl_PNG_Image* sourceImage;
	Fl_Image* image;
};


class geoPlot : public plot{

	public:

	geoPlot( const string name_, const string xlabel_, const string ylabel_,
			  const plotLimits limits_, const idx2CR imgSize_ ) :
		plot( name_, xlabel_, ylabel_, limits_, imgSize_ )
	{

	}

	geoPlot( void ) :
		plot()
	{
	}


	void addLine( const v2 start, const v2 end ){

		lines.emplace_back( start, end );

	}

	void addPoint( const v2 point ){

		points.emplace_back( point );

	}

	void create( void ){

		plot::reset();
		lines.clear();
		points.clear();

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


	private:

	vector<std::pair<v2, v2>> lines;
	vector<v2> points;

};
