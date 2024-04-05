/******************************************************************
* @file   grayscaleImage.cpp
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


GrayscaleImage::GrayscaleImage( const size_t width, const size_t height ) :
	width_( width ),
	height_( height ),
	number_of_pixel_( width_ * height_ ),
	raw_data_( number_of_pixel_, 0. ),
	image_data_( number_of_pixel_, 0 )
{}

GrayscaleImage::GrayscaleImage( const DataGrid<>& source_grid, const bool normalise ) :
	GrayscaleImage{ source_grid.size().c, source_grid.size().r }
{
	for( size_t column = 0; column < width_; column++ ){
		for( size_t row = 0; row < height_; row++ ){
			raw_data_.at( column + row * width_ ) = source_grid.operator()( GridIndex{ column, row } );
		}
	}

	if( normalise )
		AdjustContrast();
}

GrayscaleImage::GrayscaleImage( const DataGrid<VoxelData>& source_grid, const bool normalise ) :
	GrayscaleImage{ source_grid.size().c, source_grid.size().r }
{
	for( size_t column = 0; column < width_; column++ ){
		for( size_t row = 0; row < height_; row++ ){
			raw_data_.at( column + row * width_ ) = source_grid.operator()( GridIndex{ column, row } ).GetAbsorptionAtReferenceEnergy();
		}
	}
	
	if( normalise )
		AdjustContrast();
}

GrayscaleImage::GrayscaleImage( const GrayscaleImage& source_image, const size_t new_width, const size_t new_height ) :
	GrayscaleImage{ new_width, new_height }
{
	for( size_t column = 0; column < this->width(); column++ ){

		size_t source_column = static_cast<size_t>( static_cast<double>( column ) * static_cast<double>( source_image.width() ) / static_cast<double>( this->width() ));

		for( size_t row = 0; row < this->height(); row++ ){

			size_t source_row = static_cast<size_t>( static_cast<double>( row ) * static_cast<double>( source_image.height() ) / static_cast<double>( this->height() ) );

			this->operator()( column, row ) = source_image( source_column, source_row );
			SetPixelData( { column, row }, source_image.GetPixelData( source_column, source_row ) );
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

	for( size_t linear_index = 0; linear_index < number_of_pixel_; linear_index++ ){
		raw_data_.at( linear_index ) = DeSerializeBuildIn<double>( 0., binary_data, current_byte );
	}

	AdjustContrast();
}

size_t GrayscaleImage::GetIndex( const size_t column, const size_t row ) const{
	size_t linear_index = column + row * width_;
	if( linear_index >= number_of_pixel_ ){
		linear_index = number_of_pixel_ - 1;
	}
	return linear_index;
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

void GrayscaleImage::AdjustContrast( const NumberRange data_range ){

	if( raw_data_.size() == 0 ) return;

	for( size_t linear_index = 0; linear_index < number_of_pixel_; linear_index++ ){

		double difference_to_start = raw_data_.at( linear_index ) - data_range.start();
		if( difference_to_start < 0 ) difference_to_start = 0.;
		if( difference_to_start > data_range.GetDifference() ) difference_to_start = data_range.GetDifference();


		image_data_.at( linear_index ) = static_cast<unsigned char>( ( difference_to_start / ( data_range.GetDifference() ) ) * 255. );
	}
}

void GrayscaleImage::AdjustContrast( void ){

	AdjustContrast( NumberRange{
		GetMinimum(), GetMaximum()
	} );
}