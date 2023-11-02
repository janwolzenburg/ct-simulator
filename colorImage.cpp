/******************************************************************
* @file   colorImage.cpp
* @brief  Implementations
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


ColorImage::ColorImage( const size_t width_, const size_t height_ ) :
	width_( width_ ),
	height_( height_ ),
	number_of_pixel_( width_* height_ ),
	image_data_( number_of_pixel_, RGB{ 255, 255, 0 } )
{}

ColorImage::ColorImage( const ColorImage& srcImg, const size_t newWidth, const size_t newHeight ) :
	ColorImage{ newWidth, newHeight }
{
	for( size_t c = 0; c < this->width(); c++ ){
		size_t srcC = (size_t) ( (double) c * ( (double) srcImg.width() - 1. ) / ( (double) this->width() - 1. ) );

		for( size_t r = 0; r < this->height(); r++ ){

			size_t srcR = (size_t) ( (double) r * ( (double) srcImg.height() - 1. ) / ( (double) this->height() - 1. ) );
			SetPixelData( { c, r }, srcImg.GetPixelData( srcC, srcR ) );

		}
	}
}

ColorImage::ColorImage( const GrayscaleImage& srcImg, const size_t newWidth, const size_t newHeight, const vector<pair<bool, RGB>>& overlay ) :
	ColorImage{ newWidth, newHeight }
{
	for( size_t c = 0; c < this->width(); c++ ){
		size_t srcC = (size_t) ( (double) c * ( (double) srcImg.width() - 1. ) / ( (double) this->width() - 1. ) );

		for( size_t r = 0; r < this->height(); r++ ){
			size_t srcR = (size_t) ( (double) r * ( (double) srcImg.height() - 1. ) / ( (double) this->height() - 1. ) );

			if( overlay.size() != srcImg.number_of_pixel() ){
				this->SetPixelData( { c, r }, { srcImg.GetPixelData( srcC, srcR ), srcImg.GetPixelData( srcC, srcR ), srcImg.GetPixelData( srcC, srcR ) } );
			}
			else if( overlay.at( srcImg.GetIndex( srcC, srcR ) ).first ){
				this->SetPixelData( { c, r }, overlay.at( srcImg.GetIndex( srcC, srcR ) ).second );
			}
			else{
				this->SetPixelData( { c, r }, { srcImg.GetPixelData( srcC, srcR ), srcImg.GetPixelData( srcC, srcR ), srcImg.GetPixelData( srcC, srcR ) } );
			}
		}
	}
}


ColorImage::ColorImage( const vector<char>& binsourceData, vector<char>::const_iterator& it ) :
	width_( DeSerializeBuildIn( (size_t) 1, binsourceData, it ) ),
	height_( DeSerializeBuildIn( (size_t) 1, binsourceData, it ) ),
	number_of_pixel_( width_* height_ ),
	image_data_( number_of_pixel_, RGB{ 0, 0, 0 } )
{
	for( size_t i = 0; i < number_of_pixel_; i++ ){
		image_data_.at( i ).red = DeSerializeBuildIn<unsigned char>( 0, binsourceData, it );
		image_data_.at( i ).green = DeSerializeBuildIn<unsigned char>( 0, binsourceData, it );
		image_data_.at( i ).blue = DeSerializeBuildIn<unsigned char>( 0, binsourceData, it );
	}
}

size_t ColorImage::GetIndex( const size_t c, const size_t r ) const{
	
	size_t idx = c + r * width_;
	if( idx >= number_of_pixel_ ){
		idx = number_of_pixel_ - 1;
	}

	return idx;
}

size_t ColorImage::Serialize( vector<char>& binsourceData ) const{

	size_t num_bytes = 0;
	num_bytes += SerializeBuildIn( FILE_PREAMBLE, binsourceData );
	num_bytes += SerializeBuildIn( width_, binsourceData );
	num_bytes += SerializeBuildIn( height_, binsourceData );

	for( size_t i = 0; i < number_of_pixel_; i++ ){
		num_bytes += SerializeBuildIn( image_data_.at( i ).red, binsourceData );
		num_bytes += SerializeBuildIn( image_data_.at( i ).green, binsourceData );
		num_bytes += SerializeBuildIn( image_data_.at( i ).blue, binsourceData );
	}

	return num_bytes;
}