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

#include "Fl_GreyImage.h"



/*********************************************************************
  Implementations
*********************************************************************/



/*
	Fl_Image_Widget implementation
*/

Fl_Image_Widget::Fl_Image_Widget( int x, int y, int w, int h, const char* label ) :
	Fl_Widget{ x, y, w, h, label }{

};

void Fl_Image_Widget::assignImage( const greyImage& img ){
	originalImage = img;

	calculateScaled();

	redraw();
}


void Fl_Image_Widget::draw( void ){

	int centerX = this->parent()->x() + ( this->parent()->w() - (int) scaledImage.Width() ) / 2;
	//int centerY = this->parent()->y() + ( this->parent()->h() - (int) scaledImage.Height() ) / 2;

	fl_draw_image_mono( scaledImage.getDataPtr(), centerX, y(), (int) scaledImage.Width(), (int) scaledImage.Height() );
}


void Fl_Image_Widget::resize( int x, int y, int w, int h ){

	Fl_Widget::resize( x, y, w, h );

	calculateScaled();

	redraw();

}


void Fl_Image_Widget::calculateScaled( void ){

	if( originalImage.Width() == 0 || originalImage.Height() == 0 ) return;

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

	scaledImage = greyImage{ originalImage, (size_t) scaledWidth, (size_t) scaledHeight };

}