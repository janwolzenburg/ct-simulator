#pragma once
/*********************************************************************
 * @file   gui.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   March 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_draw.H>

#include "image.h"

 /*********************************************************************
	Definitions
 *********************************************************************/




class Fl_Image_Widget : public Fl_Widget{

	public:

	Fl_Image_Widget( int x, int y, int w, int h, const char* label = 0L ) :
		Fl_Widget{ x, y, w, h, label }{

	};

	void assignImage( const greyImage& img ){
		originalImage = img;

		double scaledWidth = (double) w(), scaledHeight = (double) h();

		const double aspectRatioWidget = (double) w() / (double) h();
		const double aspectRatioImage = (double) originalImage.Width() / (double) originalImage.Height();

		// Fit image vertically
		if( aspectRatioWidget > aspectRatioImage ){

			scaledHeight = (double) h();
			scaledWidth = scaledHeight * aspectRatioImage;

		}
		// Fit image horizontally
		else{

			scaledWidth = (double) w();
			scaledHeight = scaledWidth / aspectRatioImage;

		}

		scaledImage = greyImage{ img, (size_t) scaledWidth, (size_t) scaledHeight };
	}

	virtual void draw( void ){
		fl_draw_image_mono( scaledImage.getDataPtr(), (int) x(), (int) y(), (int) scaledImage.Width(), (int) scaledImage.Height() );
	}

	//virtual void resize( int x, int y, int w, int h ){
	//	
	//	Fl_Widget::resize( x, y, w, h );
	//	redraw();

	//};

	private:
	greyImage originalImage;
	greyImage scaledImage;
};
