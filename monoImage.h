#pragma once
/*********************************************************************
 * @file   monoImage.h
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


/*********************************************************************
   Definitions
*********************************************************************/

/*!
 * @brief Class for a primitev image
*/
class monoImage{

	public:

	static const string FILE_PREAMBLE;


	public:

	/*!
	 * @brief Construct empty image with given size
	 * @param width_ Width
	 * @param height_ Height
	*/
	monoImage( const size_t width_, const size_t height_ );

	/*!
	 * @brief Default constructor
	*/
	monoImage( void );

	/*!
	 * @brief Construct image from grid
	 * @param source Source grid
	*/
	monoImage( const grid<>& source, const bool normalize = false );

	monoImage( const grid<voxData>& source, const bool normalize = false );



	/*!
	 * @brief Construct image from other image but different size
	 * @details Constructed image will be scaled in each direction individually
	 * @param srcImg Source image
	 * @param newWidth Width of constructed image
	 * @param newHeight Height of constucted image
	*/
	monoImage( const monoImage& srcImg, const size_t newWidth, const size_t newHeight );

	/*!
	 * @brief Construct image from binary data
	 * @param binData Binary data
	 * @param it Iterator to start reading from
	*/
	monoImage( const vector<char>& binData, vector<char>::const_iterator& it );


	size_t index( const size_t c, const size_t r ) const;

	/*!
	 * @brief Assignment operator
	 * @param srcImg Source image
	 * @return Reference to this
	*/
	monoImage& operator=( const monoImage& srcImg );

	/*!
	 * @brief Acces operator
	 * @param c Column
	 * @param r Row
	 * @return Value at ( c, r )
	*/
	double operator()( const size_t c, const size_t r ) const;

	unsigned char charData( const size_t c, const size_t r ) const;

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
	
	size_t NumPixel( void ) const{ return numPixel; };
	
	/*!
	 * @brief Get pointer raw image data
	 * @details Be careful when data vector changes! The returned pointer may then point to false address
	 * @return Pointer to data start in
	*/
	const unsigned char* getDataPtr( void ){ return imData.data(); };

	/*!
	 * @brief Serialize this object
	 * @param binData Reference to vector where data will be appended
	*/
	size_t serialize( vector<char>& binData ) const;

	double minimum( void ) const;

	double maximum( void ) const;

	void adjustContrast( const range dataRange );
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

	private:

	size_t width;					/*!<Image width*/
	size_t height;					/*!<Image height*/
	size_t numPixel;				/*!<Amount of pixel in image*/

	vector<double> data;			/*!<Double data*/
	vector<unsigned char> imData;	/*!<Data as unsigned char values*/


	private:



	unsigned char& charData( const size_t c, const size_t r );



};

