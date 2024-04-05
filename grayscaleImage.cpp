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

#include "grayscaleImage.h"
#include "vectorAlgorithm.h"
#include "serialization.h"


/*********************************************************************
  Implementations
*********************************************************************/


const string GrayscaleImage::FILE_PREAMBLE{ "GREY_IMAGE_FILE_PREAMBLE       " };


GrayscaleImage::GrayscaleImage( const size_t width_, const size_t height_ ) :
	width_( width_ ),
	height_( height_ ),
	number_of_pixel_( width_ * height_ ),
	raw_data_( number_of_pixel_, 0. ),
	image_data_( number_of_pixel_, 0 )
{}

GrayscaleImage::GrayscaleImage( const DataGrid<>& source, const bool normaliseImg ) :
	GrayscaleImage{ source.size().c, source.size().r }
{
	for( size_t c = 0; c < width_; c++ ){
		for( size_t r = 0; r < height_; r++ ){
			raw_data_.at( c + r * width_ ) = source.operator()( GridIndex{ c, r } );
		}
	}

	if( normaliseImg )
		AdjustContrast();
}

GrayscaleImage::GrayscaleImage( const DataGrid<VoxelData>& source, const bool normaliseImg ) :
	GrayscaleImage{ source.size().c, source.size().r }
{
	for( size_t c = 0; c < width_; c++ ){
		for( size_t r = 0; r < height_; r++ ){
			raw_data_.at( c + r * width_ ) = source.operator()( GridIndex{ c, r } ).GetAbsorptionAtReferenceEnergy();
		}
	}
	
	if( normaliseImg )
		AdjustContrast();
}

GrayscaleImage::GrayscaleImage( const GrayscaleImage& srcImg, const size_t newWidth, const size_t newHeight ) :
	GrayscaleImage{ newWidth, newHeight }
{
	for( size_t c = 0; c < this->width(); c++ ){

		size_t srcC = static_cast<size_t>( static_cast<double>( c ) * static_cast<double>( srcImg.width() ) / static_cast<double>( this->width() ));

		for( size_t r = 0; r < this->height(); r++ ){

			size_t srcR = static_cast<size_t>( static_cast<double>( r ) * static_cast<double>( srcImg.height() ) / static_cast<double>( this->height() ) );

			this->operator()( c, r ) = srcImg( srcC, srcR );
			SetPixelData( { c, r }, srcImg.GetPixelData( srcC, srcR ) );
		}
	}

	AdjustContrast();
}


GrayscaleImage::GrayscaleImage( const vector<char>& binary_data, vector<char>::const_iterator& current_byte ) :
	width_( DeSerializeBuildIn<size_t>( 1, binary_data, current_byte ) ),
	height_( DeSerializeBuildIn<size_t>( 1, binary_data, current_byte ) ),
	number_of_pixel_( width_* height_ ),
	raw_data_( number_of_pixel_, 0. ),
	image_data_( number_of_pixel_, 0 ){

	for( size_t i = 0; i < number_of_pixel_; i++ ){
		raw_data_.at( i ) = DeSerializeBuildIn<double>( 0., binary_data, current_byte );
	}

	AdjustContrast();
}

size_t GrayscaleImage::GetIndex( const size_t c, const size_t r ) const{
	size_t idx = c + r * width_;
	if( idx >= number_of_pixel_ ){
		idx = number_of_pixel_ - 1;
	}
	return idx;
}

size_t GrayscaleImage::Serialize( vector<char>& binary_data ) const{

	size_t number_of_bytes = 0;
	number_of_bytes += SerializeBuildIn<size_t>( width_, binary_data );
	number_of_bytes += SerializeBuildIn<size_t>( height_, binary_data );

	for( size_t i = 0; i < number_of_pixel_; i++ ){
		number_of_bytes += SerializeBuildIn<double>( raw_data_.at( i ), binary_data );
	}

	return number_of_bytes;
}

void GrayscaleImage::AdjustContrast( const NumberRange dataRange ){

	if( raw_data_.size() == 0 ) return;

	for( size_t i = 0; i < number_of_pixel_; i++ ){

		double diffToStart = raw_data_.at( i ) - dataRange.start();
		if( diffToStart < 0 ) diffToStart = 0.;
		if( diffToStart > dataRange.GetDifference() ) diffToStart = dataRange.GetDifference();


		image_data_.at( i ) = static_cast<unsigned char>( ( diffToStart / ( dataRange.GetDifference() ) ) * 255. );
	}
}

void GrayscaleImage::AdjustContrast( void ){

	AdjustContrast( NumberRange{
		GetMinimum(), GetMaximum()
	} );
}