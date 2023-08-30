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

#include "grid.h"
#include "vectorAlgorithm.h"
#include "monoImage.h"


 /*********************************************************************
	Definitions
 *********************************************************************/



struct rgb_Double{
	double red;
	double green;
	double blue;
 };

struct rgb_Int{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};

enum color : unsigned char{
	R = 0,
	G = 1,
	B = 2
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
	rgbImage( void );

	//rgbImage( const monoImage& greyImage );

	/*!
	 * @brief Construct image from grid
	 * @param source Source grid
	*/
	rgbImage( const grid source, const bool normalize = false );

	/*!
	 * @brief Construct image from other image but different size
	 * @details Constructed image will be scaled in each direction individually
	 * @param srcImg Source image
	 * @param newWidth Width of constructed image
	 * @param newHeight Height of constucted image
	*/
	rgbImage( const rgbImage& srcImg, const size_t newWidth, const size_t newHeight );

	/*!
	 * @brief Construct image from binary data
	 * @param binData Binary data
	 * @param it Iterator to start reading from
	*/
	rgbImage( const vector<char>& binData, vector<char>::const_iterator& it );


	size_t index( const size_t c, const size_t r, const color col ) const;

	/*!
	 * @brief Assignment operator
	 * @param srcImg Source image
	 * @return Reference to this
	*/
	rgbImage& operator=( const rgbImage& srcImg );

	/*!
	 * @brief Access operator
	 * @param c Column
	 * @param r Row
	 * @return Value at ( c, r )
	*/
	rgb_Double operator()( const size_t c, const size_t r ) const;

	rgb_Int charData( const size_t c, const size_t r ) const;

	/*!
	 * @brief Get Width
	 * @return Image width
	*/
	inline size_t Width( void ) const{ return width; };

	/*!
	 * @brief Get height
	 * @return Image height
	*/
	inline size_t Height( void ) const{ return height; };

	/*!
	 * @brief Get pointer raw image data
	 * @details Be careful when data vector changes! The returned pointer may then point to false address
	 * @return Pointer to data start in
	*/
	inline const rgb_Int* getDataPtr( void ){ return imageData.data(); };

	/*!
	 * @brief Serialize this object
	 * @param binData Reference to vector where data will be appended
	*/
	size_t serialize( vector<char>& binData ) const;

	double minimum( void ) const;

	double maximum( void ) const;

	void adjustContrast( const range dataRange );



	private:

	size_t width;					/*!<Image width*/
	size_t height;					/*!<Image height*/
	size_t numPixel;				/*!<Amount of pixel in image*/

	vector<rgb_Double> sourceData;		/*!<Source data*/
	vector<rgb_Int> imageData;			/*!<Data as unsigned char values*/


	private:

	/*!
	 * @brief Acces operator
	 * @param c Column
	 * @param r Row
	 * @return Reference to value at ( c, r )
	*/
	rgb_Double& operator()( const size_t c, const size_t r );

	rgb_Int& charData( const size_t c, const size_t r );

	/*!
	 * @brief Normalize unsigned char data
	 * @details Converts double data to unsigned char. 0 will correspond to min( data ) and 255 to max( data )
	*/
	void normalize( void );

};