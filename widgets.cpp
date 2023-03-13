/******************************************************************
* @file   widgets.cpp
* @brief  Implementations
*
* @author Jan Wolzenburg
* @date   March 2023
* ********************************************************************/



/*********************************************************************
  Includes
*********************************************************************/

#include <FL/fl_draw.H>
#include "widgets.h"


/*********************************************************************
  Implementations
*********************************************************************/


Fl_Image_Widget::Fl_Image_Widget( int x, int y, int w, int h, const char* label ) :
	Fl_Widget{ x, y, w, h, label }
{

};

void Fl_Image_Widget::assignImage( const greyImage& img ){
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

	redraw();
}

void Fl_Image_Widget::draw( void ){
	fl_draw_image_mono( scaledImage.getDataPtr(), (int) x(), (int) y(), (int) scaledImage.Width(), (int) scaledImage.Height() );
}