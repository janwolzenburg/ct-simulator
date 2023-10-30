/******************************************************************
* @file   Fl_Grid_Image.cpp
* @brief  Implementations
*
* @author Jan Wolzenburg
* @date   September 2023
* ********************************************************************/


/*********************************************************************
  Includes
*********************************************************************/

#include <FL/fl_draw.H>
#include <FL/Fl.H>

#include "Fl_GridImage.h"


/*********************************************************************
  Implementations
*********************************************************************/

rgb_Int getBgColor( void ){
	rgb_Int backgroundRGB{ 0, 0, 0 };
	Fl::get_color( FL_BACKGROUND_COLOR, backgroundRGB.red, backgroundRGB.green, backgroundRGB.blue );

	return backgroundRGB;
}

rgb_Int Fl_GridImage::bgColor = getBgColor();

/*
	Fl_GridImage implementation
*/

Fl_GridImage::Fl_GridImage( int x, int y, int w, int h, const char* label ) :
	Fl_Widget{ x, y, w, h, label },
	imgAssigned( false ),
	hasOverlay( false )
{}

void Fl_GridImage::assignImage( const monoImage& img ) 
{
	originalImage = img;
	imgAssigned = true;
	overlay = vector<pair<bool, rgb_Int>>( originalImage.NumPixel(), pair<bool, rgb_Int>{ false, { 0, 0, 0 } } );
	hasOverlay = false;

	updateScaled();
}

void Fl_GridImage::assignImage( const DataGrid<VoxelData>& modGrid, const bool normalise ){

	originalImage = monoImage{ modGrid.size().c, modGrid.size().r };
	overlay = vector<pair<bool, rgb_Int>>( originalImage.NumPixel(), pair<bool, rgb_Int>{ false, { 0, 0, 0 } } );

	const size_t width = originalImage.Width();
	const size_t height = originalImage.Height();
	
	hasOverlay = false;

	for( size_t c = 0; c < width; c++ ){
		for( size_t r = 0; r < height; r++ ){
			
			const GridIndex pixel( c, r );
			const VoxelData& data_ = modGrid.operator()( pixel );

			originalImage.operator()( c, r ) = data_.GetAttenuationAtReferenceEnergy();
			

			if( data_.HasSpecialProperty() ){
				
				hasOverlay = true;

				if( data_.HasSpecificProperty( VoxelData::METAL ) )
					overlay.at( originalImage.index( c, r ) ) = { true, rgb_Int{ 128, 0, 0 } };

				if( data_.HasSpecificProperty( VoxelData::UNDEFINED ) )
					overlay.at( originalImage.index( c, r ) ) = { true, bgColor };
			}

		}
	}

	if( normalise )
		originalImage.normalise();

	imgAssigned = true;
	updateScaled();

}

void Fl_GridImage::draw( void ){

	int centerX = this->parent()->x() + ( this->parent()->w() - (int) colorImage.Width() ) / 2;
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


	colorImage = rgbImage{ originalImage, (size_t) scaledWidth, (size_t) scaledHeight, overlay };

}

void Fl_GridImage::updateScaled( void ){
	calculateScaled();
	redraw();
}