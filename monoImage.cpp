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

#include "monoImage.h"
#include "vectorAlgorithm.h"
#include "serialization.h"


/*********************************************************************
  Implementations
*********************************************************************/


const string monoImage::FILE_PREAMBLE{ "GREY_IMAGE_FILE_PREAMBLE       " };


monoImage::monoImage( const size_t width_, const size_t height_ ) :
	width( width_ ),
	height( height_ ),
	numPixel( width * height ),
	data( numPixel, 0. ),
	imData( numPixel, 0 )
{}

monoImage::monoImage( const grid<>& source, const bool normaliseImg ) :
	monoImage{ source.Size().c, source.Size().r }
{

	for( size_t c = 0; c < width; c++ ){
		for( size_t r = 0; r < height; r++ ){
			data.at( c + r * width ) = source.operator()( GridIndex{ c, r } );
		}
	}

	if( normaliseImg )
		normalise();
}

monoImage::monoImage( const grid<voxData>& source, const bool normaliseImg ) :
	monoImage{ source.Size().c, source.Size().r }
{
	for( size_t c = 0; c < width; c++ ){
		for( size_t r = 0; r < height; r++ ){

			

			data.at( c + r * width ) = source.operator()( GridIndex( c, r ) ).attenuationAtRefE();
		}
	}

	if( normaliseImg )
		normalise();
}

monoImage::monoImage( const monoImage& srcImg, const size_t newWidth, const size_t newHeight ) :
	monoImage{ newWidth, newHeight }
{
	for( size_t c = 0; c < this->Width(); c++ ){

		size_t srcC = (size_t) ( (double) c * ( (double) srcImg.Width() - 1. ) / ( (double) this->Width() - 1. ) );

		for( size_t r = 0; r < this->Height(); r++ ){

			size_t srcR = (size_t) ( (double) r * ( (double) srcImg.Height() - 1. ) / ( (double) this->Height() - 1. ) );

			this->operator()( c, r ) = srcImg( srcC, srcR );
			this->charData( c, r ) = srcImg.charData( srcC, srcR );

		}
	}
}


monoImage::monoImage( const vector<char>& binary_data, vector<char>::const_iterator& it ) :
	width( DeSerializeBuildIn( (size_t) 1, binary_data, it ) ),
	height( DeSerializeBuildIn( (size_t) 1, binary_data, it ) ),
	numPixel( width* height ),
	data( numPixel, 0. ),
	imData( numPixel, 0 ){

	for( size_t i = 0; i < numPixel; i++ ){
		data.at( i ) = DeSerializeBuildIn( 0., binary_data, it );
	}
}

size_t monoImage::index( const size_t c, const size_t r ) const{
	size_t idx = c + r * width;
	if( idx >= numPixel ){
		idx = numPixel - 1;
	}
	return idx;
}

size_t monoImage::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;
	num_bytes += SerializeBuildIn( FILE_PREAMBLE, binary_data );
	num_bytes += SerializeBuildIn( width, binary_data );
	num_bytes += SerializeBuildIn( height, binary_data );

	for( size_t i = 0; i < numPixel; i++ ){
		num_bytes += SerializeBuildIn( data.at( i ), binary_data );
	}

	return num_bytes;
}

void monoImage::normalise( void ){

	if( data.size() == 0 ) return;

	const double maxVal = GetMaxElement( data );
	const double minVal = GetMinElement( data );

	for( size_t i = 0; i < numPixel; i++ ){
		imData.at( i ) = (unsigned char) ( ( ( data.at( i ) - minVal ) / ( maxVal - minVal ) ) * 255. );
	}
}

void monoImage::adjustContrast( const NumberRange dataRange ){

	if( data.size() == 0 ) return;

	for( size_t i = 0; i < numPixel; i++ ){

		double diffToStart = data.at( i ) - dataRange.start();
		if( diffToStart < 0 ) diffToStart = 0.;
		if( diffToStart > dataRange.GetDifference() ) diffToStart = dataRange.GetDifference();


		imData.at( i ) = (unsigned char) ( ( diffToStart / ( dataRange.GetDifference() ) ) * 255. );
	}
}