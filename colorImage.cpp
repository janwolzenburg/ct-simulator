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
	cout << this->width() << endl;
	for( size_t c = 0; c < this->width(); c++ ){
		size_t srcC = static_cast<size_t>( static_cast<double>( c ) * ( static_cast<double>( srcImg.width() ) ) / ( static_cast<double>( this->width() ) ) );
		
		cout << srcC << endl;
		for( size_t r = 0; r < this->height(); r++ ){

			size_t srcR = static_cast<size_t>( static_cast<double>( r ) * ( static_cast<double>( srcImg.height() ) ) / ( static_cast<double>( this->height() ) ) );
			SetPixelData( { c, r }, srcImg.GetPixelData( srcC, srcR ) );

		}
	}

	cout << endl;
}

ColorImage::ColorImage( const GrayscaleImage& srcImg, const size_t newWidth, const size_t newHeight, const vector<pair<bool, RGB>>& overlay ) :
	ColorImage{ newWidth, newHeight }
{
	//cout << this->width() << endl;
	for( size_t c = 0; c < this->width(); c++ ){
		size_t srcC = static_cast<size_t>( static_cast<double>( c ) * static_cast<double>( srcImg.width() ) / static_cast<double>( this->width() ) );
		
		//cout << srcC << endl;

		for( size_t r = 0; r < this->height(); r++ ){
			size_t srcR = static_cast<size_t>( static_cast<double>( r ) * ( static_cast<double>( srcImg.height() ) ) / ( static_cast<double>( this->height() ) ) );

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


ColorImage::ColorImage( const vector<char>& binsourceData, vector<char>::const_iterator& current_byte ) :
	width_( DeSerializeBuildIn<size_t>( 1, binsourceData, current_byte ) ),
	height_( DeSerializeBuildIn<size_t>( 1, binsourceData, current_byte ) ),
	number_of_pixel_( width_* height_ ),
	image_data_( number_of_pixel_, RGB{ 0, 0, 0 } )
{
	for( size_t i = 0; i < number_of_pixel_; i++ ){
		image_data_.at( i ).red = DeSerializeBuildIn<unsigned char>( 0, binsourceData, current_byte );
		image_data_.at( i ).green = DeSerializeBuildIn<unsigned char>( 0, binsourceData, current_byte );
		image_data_.at( i ).blue = DeSerializeBuildIn<unsigned char>( 0, binsourceData, current_byte );
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

	size_t number_of_bytes = 0;
	number_of_bytes += SerializeBuildIn<size_t>( width_, binsourceData );
	number_of_bytes += SerializeBuildIn<size_t>( height_, binsourceData );

	for( size_t i = 0; i < number_of_pixel_; i++ ){
		number_of_bytes += SerializeBuildIn<unsigned char>( image_data_.at( i ).red, binsourceData );
		number_of_bytes += SerializeBuildIn<unsigned char>( image_data_.at( i ).green, binsourceData );
		number_of_bytes += SerializeBuildIn<unsigned char>( image_data_.at( i ).blue, binsourceData );
	}

	return number_of_bytes;
}