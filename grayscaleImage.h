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
 * @brief class for a grayscale image
*/
class GrayscaleImage{

	public:

	static const string FILE_PREAMBLE; /*!< string to prepend to file when storing as file*/


	/*!
	 * @brief construct empty image with given size
	 * @param width Width
	 * @param height Height
	*/
	GrayscaleImage( const size_t width, const size_t height );

	/*!
	 * @brief default constructor
	*/
	GrayscaleImage( void )  : GrayscaleImage{ 0, 0 }{};

	/*!
	 * @brief construct image from grid
	 * @param source_grid Source grid
	 * @param Normalise Flag for normalisation
	*/
	GrayscaleImage( const DataGrid<>& source_grid, const bool normalise = true );

	/*!
	 * @brief construct image from voxel-data grid
	 * @param source_grid Source grid with voxel data
	 * @param Normalise Flag for normalisation
	*/
	GrayscaleImage( const DataGrid<VoxelData>& source_grid, const bool normalise = true );

	/*!
	 * @brief construct image from other image but different size
	 * @details Constructed image will be scaled in each direction individually
	 * @param source_image Source image
	 * @param newWidth Width of constructed image
	 * @param newHeight Height of constucted image
	*/
	GrayscaleImage( const GrayscaleImage& source_image, const size_t new_width, const size_t new_height );

	/*!
	 * @brief construct image from binary data
	 * @param binary_data Binary data
	 * @param current_byte iterator to start reading from
	*/
	GrayscaleImage( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );
	
	/*!
	 * @brief serialize this object
	 * @param binary_data reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;

	/*!
	 * @brief get Width
	 * @return image width
	*/
	size_t width( void ) const{ return width_; };
	
	/*!
	 * @brief get height
	 * @return image height
	*/
	size_t height( void ) const{ return height_; };
	
	/*!
	 * @brief get amount of pixel
	 * @return amount of pixel
	*/
	size_t number_of_pixel( void ) const{ return number_of_pixel_; };

	/*!
	 * @brief get pointer raw image data
	 * @details Be careful when data vector changes! The returned pointer may then point to false address
	 * @return pointer to raw data
	*/
	const unsigned char* GetImageData( void ){ return image_data_.data(); };

	/*!
	 * @brief get minimum of image data
	 * @return minimum value in data
	*/
	double GetMinimum( void ) const{ return GetMinElement( raw_data_ ); };

	/*!
	 * @brief get maximum of image data
	 * @return maximum value in data
	*/
	double GetMaximum( void ) const{ return GetMaxElement( raw_data_ ); };

	/*!
	 * @brief get the 1D index of grid element
	 * @param column Column index
	 * @param row Row index
	 * @return index of Row|Column element
	*/
	size_t GetIndex( const size_t column, const size_t row ) const;

	/*!
	 * @brief get character data for image drawing
	 * @param column Column
	 * @param row Row
	 * @return Grayscale value
	*/
	unsigned char GetPixelData( const size_t column, const size_t row ) const{ return image_data_.at( GetIndex( column, row ) ); };

	/*!
	 * @brief get raw data
	 * @param column Column
	 * @param row Row
	 * @return raw value
	*/
	double GetData( const size_t column, const size_t row ) const{ return raw_data_.at( GetIndex( column, row ) ); } ;

	/*!
	 * @brief set pixel data
	 * @param pixel Pixel indices
	 * @param data New data
	*/
	void SetData( const GridIndex pixel, const double data ){ this->operator()( pixel.c, pixel.r ) = data; };

	/*!
	 * @brief change the images contrast to given range
	 * @param data_range Range of value to show. Value over or under values in range will be bounded
	*/
	void AdjustContrast( const NumberRange data_range );

	/*!
	 * @brief adjust contrast to fit complete data
	*/
	void AdjustContrast( void );


	private:

	size_t width_;					/*!< Image width*/
	size_t height_;					/*!< Image height*/
	size_t number_of_pixel_;		/*!< amount of pixel in image*/

	vector<double> raw_data_;			/*!< double data*/
	vector<unsigned char> image_data_;	/*!< data as unsigned char values*/

	/*!
	 * @brief acces operator
	 * @param c Column
	 * @param row Row
	 * @return reference to value at ( column, row )
	*/
	double& operator()( const size_t column, const size_t row ){ return raw_data_.at( GetIndex( column, row ) ); };
	
	/*!
	 * @brief acces operator
	 * @param column Column
	 * @param row Row
	 * @return value at ( column, row )
	*/
	double operator()( const size_t column, const size_t row ) const{ return raw_data_.at( GetIndex( column, row ) ); };

	/*!
	 * @brief set image data
	 * @param pixel Pixel indices
	 * @param data New data
	*/
	void SetPixelData( const GridIndex pixel, const unsigned char data ){ image_data_.at( GetIndex( pixel.c, pixel.r ) ) = data; };
};