#pragma once
/*********************************************************************
 * @file   image.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   March 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include "grid.h"



/*********************************************************************
   Definitions
*********************************************************************/

/*!
 * @brief Class for a primitev image
*/
class greyImage{

	public:

	/*!
	 * @brief Construct empty image with given size
	 * @param width_ Width
	 * @param height_ Height
	*/
	greyImage( const size_t width_, const size_t height_ );

	/*!
	 * @brief Default constructor
	*/
	greyImage( void );

	/*!
	 * @brief Construct image from grif
	 * @param source Source grid
	*/
	greyImage( const grid source );

	/*!
	 * @brief Construct image from other image but different size
	 * @details Constructed image will be scaled in each direction individually
	 * @param srcImg Source image
	 * @param newWidth Width of constructed image
	 * @param newHeight Height of constucted image
	*/
	greyImage( const greyImage& srcImg, const size_t newWidth, const size_t newHeight );

	/*!
	 * @brief Construct image from binary data
	 * @param binData Binary data
	 * @param it Iterator to start reading from
	*/
	greyImage( const vector<char>& binData, vector<char>::const_iterator& it );

	/*!
	 * @brief Assignment operator
	 * @param srcImg Source image
	 * @return Reference to this
	*/
	greyImage& operator=( const greyImage& srcImg );

	/*!
	 * @brief Acces operator
	 * @param c Column
	 * @param r Row
	 * @return Value at ( c, r )
	*/
	double operator()( const size_t c, const size_t r ) const;

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
	inline const unsigned char* getDataPtr( void ){ return imData.data(); };

	/*!
	 * @brief Serialize this object
	 * @param binData Reference to vector where data will be appended
	*/
	size_t serialize( vector<char>& binData ) const;


	private:

	size_t width;					/*!<Image width*/
	size_t height;					/*!<Image height*/
	size_t numPixel;				/*!<Amount of pixel in image*/

	vector<double> data;			/*!<Double data*/
	vector<unsigned char> imData;	/*!<Data as unsigned char values*/


	private:

	/*!
	 * @brief Acces operator
	 * @param c Column
	 * @param r Row
	 * @return Reference to value at ( c, r )
	*/
	double& operator()( const size_t c, const size_t r );

	/*!
	 * @brief Normalize unsigned char data
	 * @details Converts double data to unsigned char. 0 will correspond to min( data ) and 255 to max( data ) 
	*/
	void normalize( void );

};
