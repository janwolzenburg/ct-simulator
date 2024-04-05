#pragma once
/*********************************************************************
 * @file   colorImage.h
 *
 * @author Jan Wolzenburg
 * @date   March 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include <utility>
using std::pair;

#include "generel.h"
#include "grayscaleImage.h"


 /*********************************************************************
	Definitions
 *********************************************************************/


/*!
 * @brief rgb value
*/
struct RGB{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};


/*!
 * @brief class for a color image
*/
class ColorImage{

	public:

	static const string FILE_PREAMBLE; /*!< string to prepend to file when storing as file*/

	/*!
	 * @brief construct empty image with given size
	 * @param width Width
	 * @param height Height
	*/
	ColorImage( const size_t width, const size_t height );

	/*!
	 * @brief default constructor
	*/
	ColorImage( void ) : ColorImage{ 0, 0 }{};

	/*!
	 * @brief construct image from other image but different size
	 * @details Constructed image will be scaled in each direction individually
	 * @param source_image Source image
	 * @param new_width Width of constructed image
	 * @param new_height Height of constucted image
	*/
	ColorImage( const ColorImage& source_image, const size_t new_width, const size_t new_height );
	
	/*!
	 * @brief construct from grayscale image with new width and overlay
	 * @param source_image Grayscale image
	 * @param new_width new width
	 * @param newHeight new height
	 * @param overlay Overlay to draw
	*/
	ColorImage( const GrayscaleImage& source_image, const size_t new_width, const size_t new_height, const vector<pair<bool, RGB>>& overlay = vector<pair<bool, RGB>>( 0 ) );

	/*!
	 * @brief construct image from binary data
	 * @param binary_data Binary data
	 * @param current_byte iterator to start reading from
	*/
	ColorImage( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	/*!
	 * @brief serialize this object
	 * @param binary_data reference to vector where data will be appended
	 * @return written bytes
	*/
	size_t Serialize( vector<char>& binary_data ) const;

	/*!
	 * @brief get width
	 * @return image's width
	*/
	size_t width( void ) const{ return width_; };

	/*!
	 * @brief get height
	 * @return image's height
	*/
	size_t height( void ) const{ return height_; };
	
	/*!
	 * @brief get amount of pixel
	 * @return amount of pixel
	*/
	size_t number_of_pixel( void ) const{ return number_of_pixel_; };

	/*!
	 * @brief get pointer raw image data
	 * @details be careful when data vector changes! the returned pointer may then point to false address
	 * @return pointer to raw data
	*/
	const RGB* GetImageData( void ){ return image_data_.data(); };

	/*!
	 * @brief get the 1D index of grid element
	 * @param column column index
	 * @param row row index
	 * @return index of Row|Column element
	*/
	size_t GetIndex( const size_t column, const size_t row ) const;

	/*!
	 * @brief get the image data
	 * @param c column
	 * @param row row
	 * @return color at row and column
	*/
	RGB GetPixelData( const size_t column, const size_t row ) const{ return image_data_.at( GetIndex( column, row ) ); };


	private:

	size_t width_;							/*!< image width*/
	size_t height_;							/*!< image height*/
	size_t number_of_pixel_;		/*!< amount of pixel in image*/

	vector<RGB> image_data_;		/*!< data as unsigned char values*/


	/*!
	 * @brief get reference to image data
	 * @param column column
	 * @param row row
	 * @return reference to color data
	*/
	void SetPixelData( const GridIndex pixel, const RGB new_data ){ image_data_.at( GetIndex( pixel.c, pixel.r ) ) = new_data; };

};
