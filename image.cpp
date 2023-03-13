/******************************************************************
* @file   image.cpp
* @brief  Implementations
*
* @author Jan Wolzenburg
* @date   March 2023
* ********************************************************************/



/*********************************************************************
  Includes
*********************************************************************/

#include "image.h"
#include "vectorAlgorithm.h"


/*********************************************************************
  Implementations
*********************************************************************/



greyImage::greyImage( const size_t width_, const size_t height_ ) :
	width( width_ ),
	height( height_ ),
	numPixel( width* height ),
	data( numPixel, 0. ),
	imData( numPixel, 0 )
{

}


greyImage::greyImage( void ) :
	greyImage{ 0, 0 }
{

}


greyImage::greyImage( const grid source ) :
	greyImage{ source.Size().col, source.Size().row }{

	for( size_t c = 0; c < width; c++ ){
		for( size_t r = 0; r < height; r++ ){
			data.at( c + r * width ) = source.operator()( idx2CR{ c, r } );
		}
	}

	normalize();
}


greyImage::greyImage( const greyImage& srcImg, const size_t newWidth, const size_t newHeight ) :
	greyImage{ newWidth, newHeight }{


	for( size_t c = 0; c < this->Width(); c++ ){

		size_t srcC = (size_t) ( (double) c * ( (double) srcImg.Width() - 1. ) / ( (double) this->Width() - 1. ) );

		for( size_t r = 0; r < this->Height(); r++ ){

			size_t srcR = (size_t) ( (double) r * ( (double) srcImg.Height() - 1. ) / ( (double) this->Height() - 1. ) );

			this->operator()( c, r ) = srcImg( srcC, srcR );

		}
	}

	normalize();
}


greyImage::greyImage( const vector<char>& binData, vector<char>::const_iterator& it ) :
	width( deSerializeBuildIn( (size_t) 1, binData, it ) ),
	height( deSerializeBuildIn( (size_t) 1, binData, it ) ),
	numPixel( width* height ),
	data( numPixel, 0. ),
	imData( numPixel, 0 ){

	for( size_t i = 0; i < numPixel; i++ ){
		data.at( i ) = deSerializeBuildIn( 0., binData, it );
	}

	normalize();
}


greyImage& greyImage::operator=( const greyImage& srcImg ){

	width = srcImg.width;
	height = srcImg.height;
	numPixel = srcImg.numPixel;

	data = srcImg.data;
	imData = srcImg.imData;

	return *this;
}


double greyImage::operator()( const size_t c, const size_t r ) const{

	size_t idx = c + r * width;
	if( idx >= numPixel ) idx = numPixel - 1;

	return data.at( idx );

}


double& greyImage::operator()( const size_t c, const size_t r ){

	size_t idx = c + r * width;
	if( idx >= numPixel ) idx = numPixel - 1;

	return data.at( idx );
}


const unsigned char* greyImage::getDataPtr( void ){
	return imData.data();
}


/*!
 * @brief Serialize this object
 * @param binData Reference to vector where data will be appended
*/
size_t greyImage::serialize( vector<char>& binData ) const{

	size_t numBytes = 0;
	numBytes += serializeBuildIn( width, binData );
	numBytes += serializeBuildIn( height, binData );

	for( size_t i = 0; i < numPixel; i++ ){
		numBytes += serializeBuildIn( data.at( i ), binData );
	}

	return numBytes;
}


void greyImage::normalize( void ){

	if( data.size() == 0 ) return;

	const double maxVal = Max( data );
	const double minVal = Min( data );

	for( size_t i = 0; i < numPixel; i++ ){
		imData.at( i ) = (unsigned char) ( ( ( data.at( i ) - minVal ) / ( maxVal - minVal ) ) * 255. );
	}
}