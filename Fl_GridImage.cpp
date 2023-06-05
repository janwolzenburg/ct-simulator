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
#include "Fl_GridImage.h"



/*********************************************************************
  Implementations
*********************************************************************/



/*
	Fl_GridImage implementation
*/

Fl_GridImage::Fl_GridImage( int x, int y, int w, int h, const char* label ) :
	Fl_Widget{ x, y, w, h, label },
	imgAssigned( false )
{

};

void Fl_GridImage::assignImage( const monoImage& img ){
	originalImage = img;
	imgAssigned = true;

	updateScaled();
}


void Fl_GridImage::draw( void ){

	int centerX = this->parent()->x() + ( this->parent()->w() - (int) scaledImage.Width() ) / 2;
	//int centerY = this->parent()->y() + ( this->parent()->h() - (int) scaledImage.Height() ) / 2;

	fl_draw_image_mono( scaledImage.getDataPtr(), centerX, y(), (int) scaledImage.Width(), (int) scaledImage.Height() );
}


void Fl_GridImage::resize( int x, int y, int w, int h ){

	Fl_Widget::resize( x, y, w, h );

	updateScaled();

}


void Fl_GridImage::calculateScaled( void ){

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

	scaledImage = monoImage{ originalImage, (size_t) scaledWidth, (size_t) scaledHeight };

}

void Fl_GridImage::updateScaled( void ){

	calculateScaled();

	redraw();

}



Fl_GridImage_Adjust::Fl_GridImage_Adjust( int x, int y, int w, int h, const char* label ) :
	Fl_Group( x, y, w, h, label ),
	imgWidget( X( *this, 0. ), Y( *this, 0. ), W( *this, 1. ), H( *this, .85 ), "Image" ),
	lowerBound( X( *this, 0.1 ), Y( *this, 0.85 ), W( *this, .8 ), H( *this, .075 ), "Low" ),
	upperBound( X( *this, 0.1 ), Y( *this, 0.925 ), W( *this, .8 ), H( *this, .075 ), "High" ){
	
	
	Fl_Group::add( imgWidget );
	Fl_Group::add( lowerBound );
	Fl_Group::add( upperBound );

	lowerBound.align( FL_ALIGN_LEFT );
	upperBound.align( FL_ALIGN_LEFT );

	this->hide();



}

void Fl_GridImage_Adjust::changeContrast( const range bounds ){

	lowerBound.value( bounds.start < imgWidget.originalImage.minimum() ? imgWidget.originalImage.minimum() : bounds.start );
	upperBound.value( bounds.end > imgWidget.originalImage.maximum() ? imgWidget.originalImage.maximum() : bounds.end );

}

void Fl_GridImage_Adjust::assignImage( const monoImage& img ){

	imgWidget.assignImage( img );

	lowerBound.bounds( imgWidget.originalImage.minimum(), imgWidget.originalImage.maximum() );
	upperBound.bounds( imgWidget.originalImage.minimum(), imgWidget.originalImage.maximum() );

	lowerBound.value( lowerBound.minimum() );
	upperBound.value( upperBound.maximum() );


	imgWidget.originalImage.adjustContrast( range( lowerBound.value(), upperBound.value() ) );
	imgWidget.updateScaled();

	this->show();


}

bool Fl_GridImage_Adjust::handleEvents( void ){

	bool updateImage = false;

	if( lowerBound.ChangeFlag() ){
		if( lowerBound.value() >= upperBound.value() ){
			lowerBound.value( upperBound.value() - ( lowerBound.maximum() - lowerBound.minimum() ) / 200. );
		}

		updateImage = true;
	}

	if( upperBound.ChangeFlag() ){
		if( upperBound.value() <= lowerBound.value() ){
			upperBound.value( lowerBound.value() + ( upperBound.maximum() - upperBound.minimum() ) / 200. );
		}

		updateImage = true;
	}

	if( updateImage ){

		imgWidget.originalImage.adjustContrast( range( lowerBound.value(), upperBound.value() ) );
		imgWidget.updateScaled();

	}

	return updateImage;

}