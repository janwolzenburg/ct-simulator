#pragma once
/*********************************************************************
 * @file   grayscaleImage.h
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
 * @brief Class for a grayscale image
*/
class Image{

	public:

	static const string FILE_PREAMBLE; /*!<String to prepend to file when storing as file*/


	/*!
	 * @brief Construct empty image with given size
	 * @param width Width
	 * @param height Height
	*/
	Image( const size_t width, const size_t height );

	/*!
	 * @brief Default constructor
	*/
	Image( void )  : Image{ 0, 0 }{};

	/*!
	 * @brief Construct image from grid
	 * @param source_grid Source grid
	 * @param Normalise Flag for normalisation
	*/
	Image( const DataGrid<>& source_grid, const bool normalise = false );

	/*!
	 * @brief Construct image from voxel-data grid
	 * @param source_grid Source grid with voxel data
	 * @param Normalise Flag for normalisation
	*/
	Image( const DataGrid<VoxelData>& source_grid, const bool normalise = false );

	/*!
	 * @brief Construct image from other image but different size
	 * @details Constructed image will be scaled in each direction individually
	 * @param source_image Source image
	 * @param newWidth Width of constructed image
	 * @param newHeight Height of constucted image
	*/
	Image( const Image& source_image, const size_t new_width, const size_t new_height );

	/*!
	 * @brief Construct image from binary data
	 * @param binary_data Binary data
	 * @param current_byte Iterator to start reading from
	*/
	Image( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );
	
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
	const unsigned char* GetImageData( void ){ return image_data_.data(); };

	/*!
	 * @brief Get minimum of image data
	 * @return Minimum value in data
	*/
	double GetMinimum( void ) const{ return GetMinElement( raw_data_ ); };

	/*!
	 * @brief Get maximum of image data
	 * @return maximum value in data
	*/
	double GetMaximum( void ) const{ return GetMaxElement( raw_data_ ); };

	/*!
	 * @brief Get the 1D index of grid element
	 * @param column Column index
	 * @param row Row index
	 * @return Index of Row|Column element
	*/
	size_t GetIndex( const size_t column, const size_t row ) const;

	/*!
	 * @brief Get character data for image drawing
	 * @param c Column
	 * @param row Row
	 * @return Grayscale value
	*/
	unsigned char GetImageData( const size_t column, const size_t row ) const{ return image_data_.at( GetIndex( column, row ) ); };

	/*!
	 * @brief Set pixel data
	 * @param pixel Pixel indices
	 * @param data New data
	*/
	void SetData( const GridIndex pixel, const double data ){ this->operator()( pixel.c, pixel.r ) = data; };

	/*!
	 * @brief Change the images contrast to given range
	 * @param data_range Range of value to show. Value over or under values in range will be bounded
	*/
	void AdjustContrast( const NumberRange data_range );

	/*!
	* @brief Normalise unsigned char data
	* @details Converts double data to unsigned char. 0 will correspond to min( data ) and 255 to max( data )
	*/
	void Normalise( void );


	private:

	size_t width_;					/*!<Image width*/
	size_t height_;					/*!<Image height*/
	size_t number_of_pixel_;		/*!<Amount of pixel in image*/

	vector<double> raw_data_;			/*!<Double data*/
	vector<unsigned char> image_data_;	/*!<Data as unsigned char values*/

	/*!
	 * @brief Acces operator
	 * @param c Column
	 * @param row Row
	 * @return Reference to value at ( column, row )
	*/
	double& operator()( const size_t column, const size_t row ){ return raw_data_.at( GetIndex( column, row ) ); };
	
	/*!
	 * @brief Acces operator
	 * @param column Column
	 * @param row Row
	 * @return Value at ( column, row )
	*/
	double operator()( const size_t column, const size_t row ) const{ return raw_data_.at( GetIndex( column, row ) ); };

	/*!
	 * @brief Set image data
	 * @param pixel Pixel indices
	 * @param data New data
	*/
	void SetImageData( const GridIndex pixel, const unsigned char data ){ image_data_.at( GetIndex( pixel.c, pixel.r ) ) = data; };
};