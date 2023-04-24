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
#include "plots.h"


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

	void initializePlot( const path path_, const string xlabel_, const string ylabel_,
						 const plotLimits limits_, const string xFormat_, const string yFormat_, const bool axisEqual_, const bool grid_ ){

		plotInstance.initialize( path_, xlabel_, ylabel_, limits_, idx2CR{ (size_t) Fl_Widget::w(), (size_t) Fl_Widget::h() }, xFormat_, yFormat_, axisEqual_, grid_ );

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

		idx2CR currentImgSize = plotInstance.getSize();
		if( relDeviation( (int) currentImgSize.col, w ) > 0.15 ||
			relDeviation( (int) currentImgSize.row, h ) > 0.15 ){
			
			plotInstance.setSize( idx2CR{ (size_t) w, (size_t) h });
			plotInstance.create();
			assignImage( plotInstance.getImgPath() );
		}
		//calculateScaled();

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

		if( image != nullptr ) delete image;
		
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

