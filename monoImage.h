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

#include "dataGrid.h"
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
	 * @brief Construct empty image with given size_
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
	monoImage( const DataGrid<>& source, const bool normalise = false );

	/*!
	 * @brief Construct image from voxel-data_ grid
	 * @param source Source grid with voxel data_
	 * @param Normalise Flag for normalisation
	*/
	monoImage( const DataGrid<VoxelData>& source, const bool normalise = false );

	/*!
	 * @brief Construct image from other image but different size_
	 * @details Constructed image will be scaled in each direction individually
	 * @param srcImg Source image
	 * @param newWidth Width of constructed image
	 * @param newHeight Height of constucted image
	*/
	monoImage( const monoImage& srcImg, const size_t newWidth, const size_t newHeight );

	/*!
	 * @brief Construct image from binary data_
	 * @param binary_data Binary data_
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
	 * @param direction_ Row index
	 * @return Index of Row|Column element
	*/
	size_t index( const size_t c, const size_t r ) const;

	/*!
	 * @brief Acces operator
	 * @param c Column
	 * @param direction_ Row
	 * @return Value at ( c, direction_ )
	*/
	double operator()( const size_t c, const size_t r ) const{ return data_.at( index( c, r ) ); };

	/*!
	 * @brief Acces operator
	 * @param c Column
	 * @param direction_ Row
	 * @return Reference to value at ( c, direction_ )
	*/
	double& operator()( const size_t c, const size_t r ){ return data_.at( index( c, r ) ); };

	/*!
	 * @brief Get character data_ for image drawing
	 * @param c Column
	 * @param direction_ Row
	 * @return Grayscale value
	*/
	unsigned char charData( const size_t c, const size_t r ) const{ return imData.at( index( c, r ) ); };

	/*!
	 * @brief Get pointer raw image data_
	 * @details Be careful when data_ vector changes! The returned pointer may then point to false address
	 * @return Pointer to data_ start in
	*/
	const unsigned char* getDataPtr( void ){ return imData.data(); };

	/*!
	 * @brief Get minimum of image data_
	 * @return Minimum value in data_
	*/
	double minimum( void ) const{ return GetMinElement( data_ ); };

	/*!
	 * @brief Get maximum of image data_
	 * @return maximum value in data_
	*/
	double maximum( void ) const{ return GetMaxElement( data_ ); };

	/*!
	 * @brief Change the images contrast to given range
	 * @param dataRange Range of value to show. Value over or under values in range will be bounded
	*/
	void adjustContrast( const NumberRange dataRange );

	/*!
	* @brief Normalise unsigned char data_
	* @details Converts double data_ to unsigned char. 0 will correspond to min( data_ ) and 255 to max( data_ )
	*/
	void normalise( void );

	/*!
	 * @brief Serialize this object
	 * @param binary_data Reference to vector where data_ will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;


	private:

	size_t width;					/*!<Image width*/
	size_t height;					/*!<Image height*/
	size_t numPixel;				/*!<Amount of pixel in image*/

	vector<double> data_;			/*!<Double data_*/
	vector<unsigned char> imData;	/*!<Data as unsigned char values*/


	private:
	
	/*!
	 * @brief Get reference to image date
	 * @param c Column
	 * @param direction_ Row
	 * @return Reference to pixel value
	*/
	unsigned char& charData( const size_t c, const size_t r ){ return imData.at( index( c, r ) ); };

};