/******************************************************************
* @file   fl_GrayscaleImage.cpp
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

#include "fl_GrayscaleImage.h"
#include "widgets.h"


/*********************************************************************
  Implementations
*********************************************************************/

RGB getBgColor( void ){
	RGB backgroundRGB{ 0, 0, 0 };
	Fl::get_color( FL_BACKGROUND_COLOR, backgroundRGB.red, backgroundRGB.green, backgroundRGB.blue );

	return backgroundRGB;
}

const RGB Fl_GrayscaleImage::background_color = getBgColor();
const RGB Fl_GrayscaleImage::metal_color{ 128, 0, 0 };

/*
	Fl_GridImage implementation
*/

Fl_GrayscaleImage::Fl_GrayscaleImage( int x, int y, int w, int h, const char* label ) :
	Fl_Widget{ x, y, w, h, label },
	image_assigned_( false ),
	has_overlay_( false )
{}

void Fl_GrayscaleImage::AssignImage( const GrayscaleImage& img ) 
{
	grayscale_image_ = img;
	image_assigned_ = true;
	overlay_ = vector<pair<bool, RGB>>( grayscale_image_.number_of_pixel(), pair<bool, RGB>{ false, { 0, 0, 0 } } );
	has_overlay_ = false;

	Update();
}

void Fl_GrayscaleImage::AssignImage( const DataGrid<VoxelData>& modGrid, const bool normalise ){

	grayscale_image_ = GrayscaleImage{ modGrid.size().c, modGrid.size().r };
	overlay_ = vector<pair<bool, RGB>>( grayscale_image_.number_of_pixel(), pair<bool, RGB>{ false, { 0, 0, 0 } } );

	const size_t width = grayscale_image_.width();
	const size_t height = grayscale_image_.height();
	
	has_overlay_ = false;

	for( size_t c = 0; c < width; c++ ){
		for( size_t r = 0; r < height; r++ ){
			
			const GridIndex pixel( c, r );
			const VoxelData& data_ = modGrid.operator()( pixel );

			grayscale_image_.SetData( pixel, data_.GetAttenuationAtReferenceEnergy() );
			
			if( data_.HasSpecialProperty() ){
				
				has_overlay_ = true;

				if( data_.HasSpecificProperty( VoxelData::Metal ) )
					overlay_.at( grayscale_image_.GetIndex( c, r ) ) = { true, metal_color };

				if( data_.HasSpecificProperty( VoxelData::Undefined ) )
					overlay_.at( grayscale_image_.GetIndex( c, r ) ) = { true, background_color };
			}
		}
	}
	
	if( normalise )
		grayscale_image_.AdjustContrast();

	image_assigned_ = true;
	Update();

}

optional<pair<double, RGB>> Fl_GrayscaleImage::GetValue( int x, int y ) const{

	const int x_padding = ( w() - static_cast<int>( color_image_.width() ) ) / 2;
	const int y_padding = ( h() - static_cast<int>( color_image_.height() ) ) / 2;

	if( x < x_padding || x > w() - x_padding ||
		y < y_padding || y > h() - y_padding )
		return{};

	x = ForceToMin( x - x_padding, 0 );
	y = ForceToMin( y - y_padding, 0 );

	return pair{ grayscale_image_scaled_.GetData( x, y ), color_image_.GetPixelData( x, y ) };

}

void Fl_GrayscaleImage::draw( void ){
	int centerX = this->parent()->x() + ( this->parent()->w() - static_cast<int>( color_image_.width() ) ) / 2;
	int centerY = this->parent()->y() + ( this->parent()->h() - static_cast<int>( color_image_.height() ) ) / 2;
	fl_draw_image( (unsigned char*) color_image_.GetImageData(), centerX, centerY, static_cast<int>( color_image_.width() ), static_cast<int>( color_image_.height() ) );
}

void Fl_GrayscaleImage::resize( int x, int y, int w, int h ){
	Fl_Widget::resize( x, y, w, h );
	Update();
}

void Fl_GrayscaleImage::CalculateScaled( void ){

	if( grayscale_image_.width() == 0 || grayscale_image_.height() == 0 ) return;

	double scaledWidth = static_cast<double>( w() ), scaledHeight = static_cast<double>( h() );

	const double aspectRatioWidget = static_cast<double>( w() ) / static_cast<double>( h() );
	const double aspectRatioImage = static_cast<double>( grayscale_image_.width() ) / static_cast<double>( grayscale_image_.height() );

	// Fit image vertically
	if( aspectRatioWidget > aspectRatioImage ){
		scaledHeight = static_cast<double>( h() );
		scaledWidth = scaledHeight * aspectRatioImage;
	}
	// Fit image horizontally
	else{
		scaledWidth = static_cast<double>( w() );
		scaledHeight = scaledWidth / aspectRatioImage;
	}

	color_image_ = ColorImage{ grayscale_image_, static_cast<size_t>( scaledWidth ), static_cast<size_t>( scaledHeight ), overlay_ };
	grayscale_image_scaled_ = GrayscaleImage{ grayscale_image_, static_cast<size_t>( scaledWidth ), static_cast<size_t>( scaledHeight ) };
}

void Fl_GrayscaleImage::Update( void ){
	CalculateScaled();
	redraw();
}

void Fl_GrayscaleImage::AdjustContrast( const NumberRange new_contrast){ 
	grayscale_image_.AdjustContrast( new_contrast ); 
	Update(); 
}

