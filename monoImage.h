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


	/*!
	 * @brief Construct empty image with given size
	 * @param width_ Width
	 * @param height_ Height
	*/
	monoImage( const size_t width_, const size_t height_ );

	/*!
	 * @brief Default constructor
	*/
	monoImage( void )  : monoImage{ 0, 0 }{};

	/*!
	 * @brief Construct image from grid
	 * @param source Source grid
	 * @param Normalise Flag for normalisation
	*/
	monoImage( const grid<>& source, const bool normalise = false );

	/*!
	 * @brief Construct image from voxel-data grid
	 * @param source Source grid with voxel data
	 * @param Normalise Flag for normalisation
	*/
	monoImage( const grid<voxData>& source, const bool normalise = false );

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
	 * @param binary_data Binary data
	 * @param it Iterator to start reading from
	*/
	monoImage( const vector<char>& binary_data, vector<char>::const_iterator& it );
	
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
	 * @brief Get amount of pixel
	 * @return Amount of pixel
	*/
	size_t NumPixel( void ) const{ return numPixel; };

	/*!
	 * @brief Get the 1D index of grid element
	 * @param c Column index
	 * @param r Row index
	 * @return Index of Row|Column element
	*/
	size_t index( const size_t c, const size_t r ) const;

	/*!
	 * @brief Acces operator
	 * @param c Column
	 * @param r Row
	 * @return Value at ( c, r )
	*/
	double operator()( const size_t c, const size_t r ) const{ return data.at( index( c, r ) ); };

	/*!
	 * @brief Acces operator
	 * @param c Column
	 * @param r Row
	 * @return Reference to value at ( c, r )
	*/
	double& operator()( const size_t c, const size_t r ){ return data.at( index( c, r ) ); };

	/*!
	 * @brief Get character data for image drawing
	 * @param c Column
	 * @param r Row
	 * @return Grayscale value
	*/
	unsigned char charData( const size_t c, const size_t r ) const{ return imData.at( index( c, r ) ); };

	/*!
	 * @brief Get pointer raw image data
	 * @details Be careful when data vector changes! The returned pointer may then point to false address
	 * @return Pointer to data start in
	*/
	const unsigned char* getDataPtr( void ){ return imData.data(); };

	/*!
	 * @brief Get minimum of image data
	 * @return Minimum value in data
	*/
	double minimum( void ) const{ return MinElement( data ); };

	/*!
	 * @brief Get maximum of image data
	 * @return maximum value in data
	*/
	double maximum( void ) const{ return MaxElement( data ); };

	/*!
	 * @brief Change the images contrast to given range
	 * @param dataRange Range of value to show. Value over or under values in range will be bounded
	*/
	void adjustContrast( const NumberRange dataRange );

	/*!
	* @brief Normalise unsigned char data
	* @details Converts double data to unsigned char. 0 will correspond to min( data ) and 255 to max( data )
	*/
	void normalise( void );

	/*!
	 * @brief Serialize this object
	 * @param binary_data Reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;


	private:

	size_t width;					/*!<Image width*/
	size_t height;					/*!<Image height*/
	size_t numPixel;				/*!<Amount of pixel in image*/

	vector<double> data;			/*!<Double data*/
	vector<unsigned char> imData;	/*!<Data as unsigned char values*/


	private:
	
	/*!
	 * @brief Get reference to image date
	 * @param c Column
	 * @param r Row
	 * @return Reference to pixel value
	*/
	unsigned char& charData( const size_t c, const size_t r ){ return imData.at( index( c, r ) ); };

};