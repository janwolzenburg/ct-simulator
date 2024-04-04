#pragma once
/*********************************************************************
 * @file   grayscaleImage.h
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
	 * @param width width
	 * @param height height
	*/
	GrayscaleImage( const size_t width, const size_t height );

	/*!
	 * @brief default constructor
	*/
	GrayscaleImage( void )  : GrayscaleImage{ 0, 0 }{};

	/*!
	 * @brief construct image from grid
	 * @param source_grid source grid
	 * @param Normalise flag for normalisation
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
	 * @details constructed image will be scaled in each direction individually
	 * @param source_image source image
	 * @param new_width width of constructed image
	 * @param new_height height of constucted image
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
	 * @return written bytes
	*/
	size_t Serialize( vector<char>& binary_data ) const;

	/*!
	 * @brief get width
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
	 * @details be careful when data vector changes! The returned pointer may then point to false address
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
	 * @param column column index
	 * @param row row index
	 * @return index of row|column element
	*/
	size_t GetIndex( const size_t column, const size_t row ) const;

	/*!
	 * @brief get character data for image drawing
	 * @param column column
	 * @param row row
	 * @return grayscale value
	*/
	unsigned char GetPixelData( const size_t column, const size_t row ) const{ return image_data_.at( GetIndex( column, row ) ); };

	/*!
	 * @brief get raw data
	 * @param column column
	 * @param row row
	 * @return raw value
	*/
	double GetData( const size_t column, const size_t row ) const{ return raw_data_.at( GetIndex( column, row ) ); } ;

	/*!
	 * @brief set pixel data
	 * @param pixel Pixel indices
	 * @param data new data
	*/
	void SetData( const GridIndex pixel, const double data ){ this->operator()( pixel.c, pixel.r ) = data; };

	/*!
	 * @brief change the images contrast to given range
	 * @param data_range range of value to show. value over or under values in range will be bounded
	*/
	void AdjustContrast( const NumberRange data_range );

	/*!
	 * @brief adjust contrast to fit complete data
	*/
	void AdjustContrast( void );


	private:

	size_t width_;							/*!< image width*/
	size_t height_;							/*!< image height*/
	size_t number_of_pixel_;		/*!< amount of pixel in image*/

	vector<double> raw_data_;		/*!< double data*/
	vector<unsigned char> image_data_;	/*!< data as unsigned char values*/

	/*!
	 * @brief acces operator
	 * @param column column
	 * @param row row
	 * @return reference to value at ( column, row )
	*/
	double& operator()( const size_t column, const size_t row ){ return raw_data_.at( GetIndex( column, row ) ); };
	
	/*!
	 * @brief acces operator
	 * @param column column
	 * @param row row
	 * @return value at ( column, row )
	*/
	double operator()( const size_t column, const size_t row ) const{ return raw_data_.at( GetIndex( column, row ) ); };

	/*!
	 * @brief set image data
	 * @param pixel pixel indices
	 * @param data new data
	*/
	void SetPixelData( const GridIndex pixel, const unsigned char data ){ image_data_.at( GetIndex( pixel.c, pixel.r ) ) = data; };
};