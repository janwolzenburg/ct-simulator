/******************************************************************
* @file   rgbImage.cpp
* @brief  Implementations
*
* @author Jan Wolzenburg
* @date   August 2023
* ********************************************************************/



/*********************************************************************
  Includes
*********************************************************************/

#include "rgbImage.h"
#include "grayscaleImage.h"
#include "serialization.h"


/*********************************************************************
  Implementations
*********************************************************************/


const string rgbImage::FILE_PREAMBLE{ "RGB_IMAGE_FILE_PREAMBLE       " };


rgbImage::rgbImage( const size_t width_, const size_t height_ ) :
	width( width_ ),
	height( height_ ),
	numPixel( width* height ),
	imageData( numPixel, rgb_Int{ 255, 255, 0 } )
{}

rgbImage::rgbImage( const rgbImage& srcImg, const size_t newWidth, const size_t newHeight ) :
	rgbImage{ newWidth, newHeight }
{
	for( size_t c = 0; c < this->Width(); c++ ){

		size_t srcC = (size_t) ( (double) c * ( (double) srcImg.Width() - 1. ) / ( (double) this->Width() - 1. ) );

		for( size_t r = 0; r < this->Height(); r++ ){

			size_t srcR = (size_t) ( (double) r * ( (double) srcImg.Height() - 1. ) / ( (double) this->Height() - 1. ) );

			this->charData( c, r ) = srcImg.charData( srcC, srcR );

		}
	}
}

rgbImage::rgbImage( const GrayscaleImage& srcImg, const size_t newWidth, const size_t newHeight, const vector<pair<bool, rgb_Int>>& overlay ) :
	rgbImage{ newWidth, newHeight }
{
	for( size_t c = 0; c < this->Width(); c++ ){

		size_t srcC = (size_t) ( (double) c * ( (double) srcImg.width() - 1. ) / ( (double) this->Width() - 1. ) );

		for( size_t r = 0; r < this->Height(); r++ ){

			size_t srcR = (size_t) ( (double) r * ( (double) srcImg.height() - 1. ) / ( (double) this->Height() - 1. ) );

			if( overlay.size() != srcImg.number_of_pixel() ){
				this->charData( c, r ) = { srcImg.GetImageData( srcC, srcR ), srcImg.GetImageData( srcC, srcR ), srcImg.GetImageData( srcC, srcR ) };
			}
			else if( overlay.at( srcImg.GetIndex( srcC, srcR ) ).first ){
				this->charData( c, r ) = overlay.at( srcImg.GetIndex( srcC, srcR ) ).second;
			}
			else{
				this->charData( c, r ) = { srcImg.GetImageData( srcC, srcR ), srcImg.GetImageData( srcC, srcR ), srcImg.GetImageData( srcC, srcR ) };
			}
		}
	}
}


rgbImage::rgbImage( const vector<char>& binsourceData, vector<char>::const_iterator& it ) :
	width( DeSerializeBuildIn( (size_t) 1, binsourceData, it ) ),
	height( DeSerializeBuildIn( (size_t) 1, binsourceData, it ) ),
	numPixel( width* height ),
	imageData( numPixel, rgb_Int{ 0, 0, 0 } )
{
	for( size_t i = 0; i < numPixel; i++ ){
		imageData.at( i ).red = DeSerializeBuildIn<unsigned char>( 0, binsourceData, it );
		imageData.at( i ).green = DeSerializeBuildIn<unsigned char>( 0, binsourceData, it );
		imageData.at( i ).blue = DeSerializeBuildIn<unsigned char>( 0, binsourceData, it );
	}
}

size_t rgbImage::pixelIndex( const size_t c, const size_t r ) const{
	
	size_t idx = c + r * width;
	if( idx >= numPixel ){
		idx = numPixel - 1;
	}

	return idx;
}

size_t rgbImage::Serialize( vector<char>& binsourceData ) const{

	size_t num_bytes = 0;
	num_bytes += SerializeBuildIn( FILE_PREAMBLE, binsourceData );
	num_bytes += SerializeBuildIn( width, binsourceData );
	num_bytes += SerializeBuildIn( height, binsourceData );

	for( size_t i = 0; i < numPixel; i++ ){
		num_bytes += SerializeBuildIn( imageData.at( i ).red, binsourceData );
		num_bytes += SerializeBuildIn( imageData.at( i ).green, binsourceData );
		num_bytes += SerializeBuildIn( imageData.at( i ).blue, binsourceData );
	}

	return num_bytes;
}