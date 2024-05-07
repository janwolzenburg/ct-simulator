/******************************************************************
* @file   fl_GrayscaleImageWithAxis.cpp
*
* @author Jan Wolzenburg
* @date   November 2023
* ********************************************************************/


/*********************************************************************
  Includes
*********************************************************************/

#include <FL/fl_draw.H>
#include <FL/Fl.H>

#include "fl_GrayscaleImageWithAxis.h"
#include "widgets.h"


/*********************************************************************
  Implementations
*********************************************************************/


Fl_GrayscaleImageWithAxis::Fl_GrayscaleImageWithAxis( int x, int y, int w, int h, [[maybe_unused]] const char* label ) :
	Fl_Group{ x, y, w, h },
	Fl_GrayscaleImage{ x + axis_space_, y, w - axis_space_, h - axis_space_ }
{
	Fl_Group::add( static_cast<Fl_GrayscaleImage*>( this ) );
	Fl_Group::resizable( static_cast<Fl_GrayscaleImage*>( this ) );
}

void Fl_GrayscaleImageWithAxis::SetAxisLabel( const pair<string, string> label ){
		
	axis_label_ = label;

}

void Fl_GrayscaleImageWithAxis::SetAxis( const Tuple2D pixel_start, const Tuple2D pixel_size, const Index2D number_of_tics ){
		
	axis_tics_start_ = pixel_start;
	pixel_size_ = pixel_size;
	number_of_tics_ = number_of_tics;
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

	//const int number_of_tics = 7;
	for( unsigned int i = 0; i < number_of_tics_.x; i++ ){

		const double x_axis_tic = axis_tics_start_.x + static_cast<double>( i ) * static_cast<double>( Fl_GrayscaleImage::GetOriginalImageSize().x - 1 ) / static_cast<double>( number_of_tics_.x - 1 ) * ( pixel_size_.x );
		
		int digits_befor_comma = 0;
		for( double divided_tic = x_axis_tic ; std::abs( divided_tic ) > 1.; divided_tic /= 10 )
			digits_befor_comma++;

		string number_string = ConvertToString( x_axis_tic, 3 - digits_befor_comma );
			
		PixelCoordinates tic_position{ axis_origin.x + axis_space_ / 2 + static_cast<int>( i ) * ( x_axis_tip.x - axis_origin.x - axis_space_ ) / ( static_cast<int>( number_of_tics_.x ) - 1),  axis_origin.y };
			
		int x,y;
		fl_measure( number_string.c_str(), x, y );
		fl_draw(number_string.c_str(), tic_position.x - x/2, tic_position.y + 25 );
		fl_line( tic_position.x, tic_position.y + 8, tic_position.x, tic_position.y - 8 );
	}

	for( unsigned int i = 0; i < number_of_tics_.y; i++ ){
		const double y_axis_tic = axis_tics_start_.y + static_cast<double>( i ) * static_cast<double>( Fl_GrayscaleImage::GetOriginalImageSize().y - 1 ) / static_cast<double>( number_of_tics_.y - 1 ) * ( pixel_size_.y );
		
		int digits_befor_comma = 0;
		for( double divided_tic = y_axis_tic ; std::abs( divided_tic ) > 1.; divided_tic /= 10 )
			digits_befor_comma++;

		string number_string = ConvertToString( y_axis_tic, 3 - digits_befor_comma );
		
		PixelCoordinates tic_position{
			axis_origin.x,
			axis_origin.y - axis_space_ / 2 - static_cast<int>( i ) * ( axis_origin.y - y_axis_tip.y - axis_space_ ) / (static_cast<int>( number_of_tics_.y ) - 1) };
		
		int x,y;
		fl_measure( number_string.c_str(), x, y );
		fl_draw( 90, number_string.c_str(), tic_position.x - 25, tic_position.y + y/2 );
		fl_line( tic_position.x - 8, tic_position.y, tic_position.x + 8, tic_position.y);
	}



}