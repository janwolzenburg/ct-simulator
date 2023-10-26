#pragma once
/*********************************************************************
 * @file   rgbImage.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   March 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

 #include <string>
 using std::string;

 #include <vector>
 using std::vector;

#include "monoImage.fwd.h"
#include <utility>
using std::pair;

#include "generel.h"



 /*********************************************************************
	Definitions
 *********************************************************************/


/*!
 * @brief RGB value
*/
struct rgb_Int{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};

class rgbImage{

	public:

	static const string FILE_PREAMBLE;


	public:

	/*!
	 * @brief Construct empty image with given size
	 * @param width_ Width
	 * @param height_ Height
	*/
	rgbImage( const size_t width_, const size_t height_ );

	/*!
	 * @brief Default constructor
	*/
	rgbImage( void ) : rgbImage{ 0, 0 }{};

	/*!
	 * @brief Construct image from other image but different size
	 * @details Constructed image will be scaled in each direction individually
	 * @param srcImg Source image
	 * @param newWidth Width of constructed image
	 * @param newHeight Height of constucted image
	*/
	rgbImage( const rgbImage& srcImg, const size_t newWidth, const size_t newHeight );
	
	/*!
	 * @brief Construct from grayscale image with new width and overlay
	 * @param srcImg Grayscale image
	 * @param newWidth New width
	 * @param newHeight New height
	 * @param overlay Overlay to draw
	*/
	rgbImage( const monoImage& srcImg, const size_t newWidth, const size_t newHeight, const vector<pair<bool, rgb_Int>>& overlay = vector<pair<bool, rgb_Int>>( 0 ) );

	/*!
	 * @brief Construct image from binary data
	 * @param binary_data Binary data
	 * @param it Iterator to start reading from
	*/
	rgbImage( const vector<char>& binary_data, vector<char>::const_iterator& it );

	/*!
	 * @brief Get Width
	 * @return Image width
	*/
	size_t Width( void ) const{ return width; };

	/*!
	 * @brief Get height
	 * @return Image height
	*/
	size_t Height( void ) const{ return height; };

	/*!
	 * @brief Get the 1D index of grid element
	 * @param c Column index
	 * @param r Row index
	 * @return Index of Row|Column element
	*/
	size_t pixelIndex( const size_t c, const size_t r ) const;

	/*!
	 * @brief Get the image data
	 * @param c Column
	 * @param r Row
	 * @return Color at row and column
	*/
	rgb_Int charData( const size_t c, const size_t r ) const{ return imageData.at( pixelIndex( c, r ) ); };

	/*!
	 * @brief Get pointer raw image data
	 * @details Be careful when data vector changes! The returned pointer may then point to false address
	 * @return Pointer to data start in
	*/
	const rgb_Int* getDataPtr( void ){ return imageData.data(); };

	/*!
	 * @brief Set pixel value
	 * @param pixel Pixel indices
	 * @param value Value to set pixel to
	*/
	void setPixel( const GridIndex& pixel, const rgb_Int& value ){ charData( pixel.c, pixel.r ) = value; };

	/*!
	 * @brief Serialize this object
	 * @param binary_data Reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;


	private:

	size_t width;					/*!<Image width*/
	size_t height;					/*!<Image height*/
	size_t numPixel;				/*!<Amount of pixel in image*/

	vector<rgb_Int> imageData;		/*!<Data as unsigned char values*/


	private:

	/*!
	 * @brief Get reference to image data
	 * @param c Column
	 * @param r Row
	 * @return Reference to color data
	*/
	rgb_Int& charData( const size_t c, const size_t r ){ return imageData.at( pixelIndex( c, r ) ); };

};
