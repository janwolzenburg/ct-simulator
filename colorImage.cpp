/******************************************************************
* @file   colorImage.cpp
*
* @author Jan Wolzenburg
* @date   August 2023
* ********************************************************************/



/*********************************************************************
  Includes
*********************************************************************/

#include "colorImage.h"
#include "grayscaleImage.h"
#include "serialization.h"


/*********************************************************************
  Implementations
*********************************************************************/


const string ColorImage::FILE_PREAMBLE{ "RGB_IMAGE_FILE_PREAMBLE       " };


ColorImage::ColorImage( const size_t width, const size_t height ) :
	width_( width ),
	height_( height ),
	number_of_pixel_( width * height ),
	image_data_( number_of_pixel_, RGB{ 255, 255, 0 } )
{}

ColorImage::ColorImage( const ColorImage& source_image, const size_t new_width, const size_t new_height ) :
	ColorImage{ new_width, new_height }
{
	cout << this->width() << endl;
	for( size_t column = 0; column < this->width(); column++ ){
		size_t srcC = static_cast<size_t>( static_cast<double>( column ) * ( static_cast<double>( source_image.width() ) ) / ( static_cast<double>( this->width() ) ) );
		
		cout << srcC << endl;
		for( size_t row = 0; row < this->height(); row++ ){

			size_t srcR = static_cast<size_t>( static_cast<double>( row ) * ( static_cast<double>( source_image.height() ) ) / ( static_cast<double>( this->height() ) ) );
			SetPixelData( { column, row }, source_image.GetPixelData( srcC, srcR ) );

		}
	}

	cout << endl;
}

ColorImage::ColorImage( const GrayscaleImage& source_image, const size_t new_width, const size_t new_height, const vector<pair<bool, RGB>>& overlay ) :
	ColorImage{ new_width, new_height }
{
	for( size_t column = 0; column < this->width(); column++ ){
		size_t source_column = static_cast<size_t>( static_cast<double>( column ) * static_cast<double>( source_image.width() ) / static_cast<double>( this->width() ) );
		

		for( size_t row = 0; row < this->height(); row++ ){
			size_t source_row = static_cast<size_t>( static_cast<double>( row ) * ( static_cast<double>( source_image.height() ) ) / ( static_cast<double>( this->height() ) ) );

			if( overlay.size() != source_image.number_of_pixel() ){
				this->SetPixelData( { column, row }, { source_image.GetPixelData( source_column, source_row ), source_image.GetPixelData( source_column, source_row ), source_image.GetPixelData( source_column, source_row ) } );
			}
			else if( overlay.at( source_image.GetIndex( source_column, source_row ) ).first ){
				this->SetPixelData( { column, row }, overlay.at( source_image.GetIndex( source_column, source_row ) ).second );
			}
			else{
				this->SetPixelData( { column, row }, { source_image.GetPixelData( source_column, source_row ), source_image.GetPixelData( source_column, source_row ), source_image.GetPixelData( source_column, source_row ) } );
			}
		}
	}
}


ColorImage::ColorImage( const vector<char>& binary_data, vector<char>::const_iterator& current_byte ) :
	width_( DeSerializeBuildIn<size_t>( 1, binary_data, current_byte ) ),
	height_( DeSerializeBuildIn<size_t>( 1, binary_data, current_byte ) ),
	number_of_pixel_( width_* height_ ),
	image_data_( number_of_pixel_, RGB{ 0, 0, 0 } )
{
	for( size_t i = 0; i < number_of_pixel_; i++ ){
		image_data_.at( i ).red = DeSerializeBuildIn<unsigned char>( 0, binary_data, current_byte );
		image_data_.at( i ).green = DeSerializeBuildIn<unsigned char>( 0, binary_data, current_byte );
		image_data_.at( i ).blue = DeSerializeBuildIn<unsigned char>( 0, binary_data, current_byte );
	}
}

size_t ColorImage::GetIndex( const size_t column, const size_t row ) const{
	
	size_t idx = column + row * width_;
	if( idx >= number_of_pixel_ ){
		idx = number_of_pixel_ - 1;
	}

	return idx;
}

size_t ColorImage::Serialize( vector<char>& binary_data ) const{

	size_t number_of_bytes = 0;
	number_of_bytes += SerializeBuildIn<size_t>( width_, binary_data );
	number_of_bytes += SerializeBuildIn<size_t>( height_, binary_data );

	for( size_t i = 0; i < number_of_pixel_; i++ ){
		number_of_bytes += SerializeBuildIn<unsigned char>( image_data_.at( i ).red, binary_data );
		number_of_bytes += SerializeBuildIn<unsigned char>( image_data_.at( i ).green, binary_data );
		number_of_bytes += SerializeBuildIn<unsigned char>( image_data_.at( i ).blue, binary_data );
	}

	return number_of_bytes;
}