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
#include <FL/Fl.H>

#include "widgets.h"
#include "Fl_GridImage.h"



/*********************************************************************
  Implementations
*********************************************************************/



/*
	Fl_GridImage implementation
*/

rgb_Int getBgColor( void ){

	rgb_Int backgroundRGB{ 0, 0, 0 };
	Fl::get_color( FL_BACKGROUND_COLOR, backgroundRGB.red, backgroundRGB.green, backgroundRGB.blue );

	return backgroundRGB;
}

rgb_Int Fl_GridImage::bgColor = getBgColor();

Fl_GridImage::Fl_GridImage( int x, int y, int w, int h, const char* label ) :
	Fl_Widget{ x, y, w, h, label },
	imgAssigned( false ),
	hasOverlay( false )
{

};

void Fl_GridImage::assignImage( const monoImage& img ) 
{
	originalImage = img;
	imgAssigned = true;
	overlay = vector<pair<bool, rgb_Int>>( originalImage.NumPixel(), pair<bool, rgb_Int>{ false, { 0, 0, 0 } } );
	hasOverlay = false;

	updateScaled();
}



void Fl_GridImage::assignImage( const grid<voxData>& modGrid, const bool normalize ){
	


	originalImage = monoImage( modGrid.Size().col, modGrid.Size().row );
	overlay = vector<pair<bool, rgb_Int>>( originalImage.NumPixel(), pair<bool, rgb_Int>{ false, { 0, 0, 0 } } );

	const size_t width = originalImage.Width();
	const size_t height = originalImage.Height();
	
	hasOverlay = false;

	for( size_t c = 0; c < width; c++ ){
		for( size_t r = 0; r < height; r++ ){
			
			const idx2CR pixel( c, r );
			const voxData& data = modGrid.operator()( pixel );

			originalImage.operator()( c, r ) = data.attenuationAtRefE();
			

			if( data.hasSpecialProperty() ){
				
				hasOverlay = true;

				if( data.hasSpecificProperty( voxData::METAL ) )
					overlay.at( originalImage.index( c, r ) ) = { true, rgb_Int{ 128, 0, 0 } };

				if( data.hasSpecificProperty( voxData::UNDEFINED ) )
					overlay.at( originalImage.index( c, r ) ) = { true, bgColor };
			}

		}
	}

	if( normalize )
		originalImage.normalize();


	imgAssigned = true;


	updateScaled();

}

void Fl_GridImage::draw( void ){


	int centerX = this->parent()->x() + ( this->parent()->w() - (int) colorImage.Width() ) / 2;
	//int centerY = this->parent()->y() + ( this->parent()->h() - (int) scaledImage.Height() ) / 2;

	//fl_rectf( this->parent()->x(), this->parent()->y(), this->parent()->w(), this->parent()->h(), 216 );

	fl_draw_image( (unsigned char*) colorImage.getDataPtr(), centerX, y(), (int) colorImage.Width(), (int) colorImage.Height() );
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


	colorImage = rgbImage( originalImage, (size_t) scaledWidth, (size_t) scaledHeight, overlay );

}

void Fl_GridImage::updateScaled( void ){

	calculateScaled();

	redraw();

}



Fl_GridImage_Adjust::Fl_GridImage_Adjust( int x, int y, int w, int h, const char* label ) :
	Fl_Group( x, y, w, h, label ),
	imgWidget( X( *this, 0. ), Y( *this, 0. ), W( *this, 1. ), H( *this, .9 ), "Image" ),
	lowerBound( X( *this, 0.2 ), Y( *this, 0.91 ), W( *this, .6 ), H( *this, .04 ), "Low" ),
	upperBound( X( *this, 0.2 ), Y( *this, 0.96 ), W( *this, .6 ), H( *this, .04 ), "High" ),
	boundsSet( false ){
	
	
	Fl_Group::add( imgWidget );
	Fl_Group::add( lowerBound );
	Fl_Group::add( upperBound );

	lowerBound.align( FL_ALIGN_LEFT );
	upperBound.align( FL_ALIGN_LEFT );

	lowerBound.ValueToLabel( true );
	upperBound.ValueToLabel( true );

	this->hide();



}

void Fl_GridImage_Adjust::changeContrast( const range bounds ){

	lowerBound.value( Frange( bounds.start, lowerBound.minimum(), lowerBound.maximum() ) );
	upperBound.value( Frange( bounds.end, upperBound.minimum(), upperBound.maximum() ) );

}

void Fl_GridImage_Adjust::assignImage( const monoImage& img ){

	imgWidget.assignImage( img );

	if( !boundsSet )
		setSliderBoundsFromImage();

	imgWidget.originalImage.adjustContrast( range( lowerBound.value(), upperBound.value() ) );
	imgWidget.updateScaled();

	this->show();


}

void Fl_GridImage_Adjust::assignImage( const grid<voxData>& modGrid, const bool normalize ){

	imgWidget.assignImage( modGrid, normalize );

	if( !boundsSet )
		setSliderBoundsFromImage();

	imgWidget.originalImage.adjustContrast( range( lowerBound.value(), upperBound.value() ) );
	imgWidget.updateScaled();

	this->show();

}

void Fl_GridImage_Adjust::setSliderBoundsFromImage( void ){

	lowerBound.bounds( imgWidget.originalImage.minimum(), imgWidget.originalImage.maximum() );
	upperBound.bounds( imgWidget.originalImage.minimum(), imgWidget.originalImage.maximum() );

	lowerBound.value( lowerBound.minimum() );
	upperBound.value( upperBound.maximum() );

	lowerBound.step( imgWidget.originalImage.maximum() - imgWidget.originalImage.minimum(), 200 );
	upperBound.step( imgWidget.originalImage.maximum() - imgWidget.originalImage.minimum(), 200 );

	boundsSet = true;
}

void Fl_GridImage_Adjust::setSliderBounds( const range newBound ){

	lowerBound.bounds( newBound.start, newBound.end );
	upperBound.bounds( newBound.start, newBound.end );

	lowerBound.value( newBound.start );
	upperBound.value( newBound.end );

	lowerBound.step( newBound.Diff() / 100. );
	upperBound.step( newBound.Diff() / 100. );

	changeContrast( newBound ); 

	boundsSet = true;
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