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



Fl_GrayscaleImageWithAxis::Fl_GrayscaleImageWithAxis( int x, int y, int w, int h, const char* label ) :
	Fl_Group{ x, y, w, h },
	Fl_GrayscaleImage{ x + axis_space_, y, w - axis_space_, h - axis_space_ }
{
	Fl_Group::add( static_cast<Fl_GrayscaleImage*>( this ) );
	Fl_Group::resizable( static_cast<Fl_GrayscaleImage*>( this ) );
}

void Fl_GrayscaleImageWithAxis::SetAxisLabel( const pair<string, string> label ){
		
	axis_label_ = label;

}

void Fl_GrayscaleImageWithAxis::SetAxis( const Tuple2D pixel_start, const Tuple2D pixel_size ){
		
	image_start_ = pixel_start;
	pixel_size_ = pixel_size;
}

	
void Fl_GrayscaleImageWithAxis::draw( void ){
	Fl_GrayscaleImage::draw();
		

	PixelCoordinates image_size{ Fl_GrayscaleImage::GetImageSize() };


	PixelCoordinates axis_origin{ x() + ( w() - image_size.x ) / 2 - axis_space_ / 2, y() + ( h() - image_size.y ) / 2 + image_size.y + axis_space_ / 2 };
	PixelCoordinates x_axis_tip{ axis_origin.x + image_size.x + axis_space_, axis_origin.y };
	PixelCoordinates y_axis_tip{ axis_origin.x, axis_origin.y - image_size.y - axis_space_ };

	fl_line_style( FL_SOLID, 4, 0 );

	fl_line( axis_origin.x, axis_origin.y, x_axis_tip.x, x_axis_tip.y );
	fl_line( axis_origin.x, axis_origin.y, y_axis_tip.x, y_axis_tip.y );

	fl_line( x_axis_tip.x, x_axis_tip.y, x_axis_tip.x - 10, x_axis_tip.y - 10 );
	fl_line( x_axis_tip.x, x_axis_tip.y, x_axis_tip.x - 10, x_axis_tip.y + 10 );
	fl_line( y_axis_tip.x, y_axis_tip.y, y_axis_tip.x - 10, y_axis_tip.y + 10 );
	fl_line( y_axis_tip.x, y_axis_tip.y, y_axis_tip.x + 10, y_axis_tip.y + 10 );
		
	fl_line_style( 0 );


	fl_font( FL_HELVETICA, 15 );

	fl_draw( axis_label_.first.c_str(), x_axis_tip.x + 5, x_axis_tip.y + 5 );

	int label_width, label_height;
	fl_measure( axis_label_.second.c_str(), label_width, label_height );
	fl_draw( axis_label_.second.c_str(), y_axis_tip.x - label_width / 2, y_axis_tip.y - 5);

	const int number_of_tics = 6;
	for( int i = 0; i < number_of_tics; i++ ){

		const double x_axis_tic = image_start_.x + static_cast<double>( i ) * static_cast<double>( Fl_GrayscaleImage::GetOriginalImageSize().x - 1 ) / static_cast<double>( number_of_tics - 1 ) * ( pixel_size_.x );
		
		int digits_befor_comma = 0;
		for( double divided_tic = x_axis_tic ; abs( divided_tic ) > 1.; divided_tic /= 10 )
			digits_befor_comma++;

		string number_string = ToString( x_axis_tic, 3 - digits_befor_comma );
			
		PixelCoordinates tic_position{ axis_origin.x + axis_space_ / 2 + i * ( x_axis_tip.x - axis_origin.x - axis_space_ ) / (number_of_tics - 1),  axis_origin.y };
			
		int x,y;
		fl_measure( number_string.c_str(), x, y );
		fl_draw(number_string.c_str(), tic_position.x - x/2, tic_position.y + 25 );
		fl_line( tic_position.x, tic_position.y + 8, tic_position.x, tic_position.y - 8 );
	
		const double y_axis_tic = image_start_.y + static_cast<double>( i ) * static_cast<double>( Fl_GrayscaleImage::GetOriginalImageSize().y - 1 ) / static_cast<double>( number_of_tics - 1 ) * ( pixel_size_.y );
		
		digits_befor_comma = 0;
		for( double divided_tic = y_axis_tic ; abs( divided_tic ) > 1.; divided_tic /= 10 )
			digits_befor_comma++;

		number_string = ToString( y_axis_tic, 3 - digits_befor_comma );
			
		tic_position.x = axis_origin.x;
		tic_position.y = axis_origin.y - axis_space_ / 2 - i * ( axis_origin.y - y_axis_tip.y - axis_space_ ) / (number_of_tics - 1);
			
		fl_measure( number_string.c_str(), x, y );
		fl_draw( 90, number_string.c_str(), tic_position.x - 25, tic_position.y + y/2 );
		fl_line( tic_position.x - 8, tic_position.y, tic_position.x + 8, tic_position.y);
	}



}