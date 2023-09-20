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
#include "monoImage.h"
//#include "vectorAlgorithm.h"


/*********************************************************************
  Implementations
*********************************************************************/


const string rgbImage::FILE_PREAMBLE{ "RGB_IMAGE_FILE_PREAMBLE       " };


rgbImage::rgbImage( const size_t width_, const size_t height_ ) :
	width( width_ ),
	height( height_ ),
	numPixel( width* height ),
	imageData( numPixel, rgb_Int{ 255, 255, 0 } ){

}


rgbImage::rgbImage( void ) :
	rgbImage{ 0, 0 }{

}




rgbImage::rgbImage( const rgbImage& srcImg, const size_t newWidth, const size_t newHeight ) :
	rgbImage{ newWidth, newHeight }{


	for( size_t c = 0; c < this->Width(); c++ ){

		size_t srcC = (size_t) ( (double) c * ( (double) srcImg.Width() - 1. ) / ( (double) this->Width() - 1. ) );

		for( size_t r = 0; r < this->Height(); r++ ){

			size_t srcR = (size_t) ( (double) r * ( (double) srcImg.Height() - 1. ) / ( (double) this->Height() - 1. ) );

			this->charData( c, r ) = srcImg.charData( srcC, srcR );

		}
	}

}

rgbImage::rgbImage( const monoImage& srcImg, const size_t newWidth, const size_t newHeight ) :
	rgbImage{ newWidth, newHeight }
{


	for( size_t c = 0; c < this->Width(); c++ ){

		size_t srcC = (size_t) ( (double) c * ( (double) srcImg.Width() - 1. ) / ( (double) this->Width() - 1. ) );

		for( size_t r = 0; r < this->Height(); r++ ){

			size_t srcR = (size_t) ( (double) r * ( (double) srcImg.Height() - 1. ) / ( (double) this->Height() - 1. ) );

			this->charData( c, r ) = { srcImg.charData( srcC, srcR ), srcImg.charData( srcC, srcR ), srcImg.charData( srcC, srcR ) };

		}
	}

}


rgbImage::rgbImage( const vector<char>& binsourceData, vector<char>::const_iterator& it ) :
	width( deSerializeBuildIn( (size_t) 1, binsourceData, it ) ),
	height( deSerializeBuildIn( (size_t) 1, binsourceData, it ) ),
	numPixel( width* height ),
	imageData( numPixel, rgb_Int{ 0, 0, 0 } )
{

	for( size_t i = 0; i < numPixel; i++ ){
		imageData.at( i ).red = deSerializeBuildIn( 0, binsourceData, it );
		imageData.at( i ).green = deSerializeBuildIn( 0, binsourceData, it );
		imageData.at( i ).blue = deSerializeBuildIn( 0, binsourceData, it );
	}

}


rgbImage& rgbImage::operator=( const rgbImage& srcImg ){

	width = srcImg.width;
	height = srcImg.height;
	numPixel = srcImg.numPixel;

	imageData = srcImg.imageData;

	return *this;
}

size_t rgbImage::pixelIndex( const size_t c, const size_t r ) const{
	
	size_t idx = c + r * width;
	if( idx >= numPixel ) idx = numPixel - 1;

	return idx;
}

void rgbImage::setPixel( const idx2CR& pixel, const rgb_Int& value ){
	charData( pixel.col, pixel.row ) = value;
} 

rgb_Int rgbImage::charData( const size_t c, const size_t r ) const{

	return imageData.at( pixelIndex( c, r ) );

}

rgb_Int& rgbImage::charData( const size_t c, const size_t r ){

	return imageData.at( pixelIndex( c, r ) );

}

size_t rgbImage::serialize( vector<char>& binsourceData ) const{

	size_t numBytes = 0;
	numBytes += serializeBuildIn( FILE_PREAMBLE, binsourceData );
	numBytes += serializeBuildIn( width, binsourceData );
	numBytes += serializeBuildIn( height, binsourceData );

	for( size_t i = 0; i < numPixel; i++ ){
		numBytes += serializeBuildIn( imageData.at( i ).red, binsourceData );
		numBytes += serializeBuildIn( imageData.at( i ).green, binsourceData );
		numBytes += serializeBuildIn( imageData.at( i ).blue, binsourceData );
	}

	return numBytes;
}

