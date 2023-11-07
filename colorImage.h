#pragma once
/*********************************************************************
 * @file   colorImage.h
 * @brief
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
 * @brief RGB value
*/
struct RGB{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};


/*!
 * @brief Class for a color image
*/
class ColorImage{

	public:

	static const string FILE_PREAMBLE; /*!<String to prepend to file when storing as file*/

	/*!
	 * @brief Construct empty image with given size
	 * @param width Width
	 * @param height Height
	*/
	ColorImage( const size_t width, const size_t height );

	/*!
	 * @brief Default constructor
	*/
	ColorImage( void ) : ColorImage{ 0, 0 }{};

	/*!
	 * @brief Construct image from other image but different size
	 * @details Constructed image will be scaled in each direction individually
	 * @param source_image Source image
	 * @param new_width Width of constructed image
	 * @param new_height Height of constucted image
	*/
	ColorImage( const ColorImage& source_image, const size_t new_width, const size_t new_height );
	
	/*!
	 * @brief Construct from grayscale image with new width and overlay
	 * @param source_image Grayscale image
	 * @param new_width New width
	 * @param newHeight New height
	 * @param overlay Overlay to draw
	*/
	ColorImage( const GrayscaleImage& source_image, const size_t new_width, const size_t new_height, const vector<pair<bool, RGB>>& overlay = vector<pair<bool, RGB>>( 0 ) );

	/*!
	 * @brief Construct image from binary data
	 * @param binary_data Binary data
	 * @param current_byte Iterator to start reading from
	*/
	ColorImage( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	/*!
	 * @brief Serialize this object
	 * @param binary_data Reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;

	/*!
	 * @brief Get Width
	 * @return Image width
	*/
	size_t width( void ) const{ return width_; };

	/*!
	 * @brief Get height
	 * @return Image height
	*/
	size_t height( void ) const{ return height_; };
	
	/*!
	 * @brief Get amount of pixel
	 * @return Amount of pixel
	*/
	size_t number_of_pixel( void ) const{ return number_of_pixel_; };

	/*!
	 * @brief Get pointer raw image data
	 * @details Be careful when data vector changes! The returned pointer may then point to false address
	 * @return Pointer to raw data
	*/
	const RGB* GetImageData( void ){ return image_data_.data(); };

	/*!
	 * @brief Get the 1D index of grid element
	 * @param column Column index
	 * @param row Row index
	 * @return Index of Row|Column element
	*/
	size_t GetIndex( const size_t column, const size_t row ) const;

	/*!
	 * @brief Get the image data_
	 * @param c Column
	 * @param row Row
	 * @return Color at row and column
	*/
	RGB GetPixelData( const size_t column, const size_t row ) const{ return image_data_.at( GetIndex( column, row ) ); };


	private:

	size_t width_;					/*!<Image width*/
	size_t height_;					/*!<Image height*/
	size_t number_of_pixel_;		/*!<Amount of pixel in image*/

	vector<RGB> image_data_;		/*!<Data as unsigned char values*/


	/*!
	 * @brief Get reference to image data
	 * @param column Column
	 * @param row Row
	 * @return Reference to color data
	*/
	void SetPixelData( const GridIndex pixel, const RGB new_data ){ image_data_.at( GetIndex( pixel.c, pixel.r ) ) = new_data; };

};
