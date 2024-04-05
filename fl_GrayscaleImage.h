#pragma once
/*********************************************************************
 * @file   fl_GrayscaleImage.h
 *
 * @author Jan Wolzenburg
 * @date   September 2023
 *********************************************************************/


/*********************************************************************
	Includes
*********************************************************************/

#include <FL/Fl_Widget.H>

 
#include <optional>
using std::optional;

#include "grayscaleImage.h"
#include "colorImage.h"


 /*********************************************************************
	Definitions
 *********************************************************************/


/*!
 * @brief class for a Fl_Widget to display grayscale image with optional colored overlay
*/
class Fl_GrayscaleImage : public Fl_Widget{

	public:

	static const RGB background_color;		/*!< background color for pixel without data*/
	static const RGB metal_color;					/*!< overlay color of metal*/

	/*!
	 * @brief constructor
	 * @param x x-position
	 * @param y y-position
	 * @param w width
	 * @param h height
	 * @param label label
	*/
	Fl_GrayscaleImage( int x, int y, int w, int h, const char* label = 0L );

	/*!
	 * @brief check if image is assigned
	 * @return true when an image was previously assigned
	*/
	bool image_assigned( void ) const{ return image_assigned_; };

	/*!
	 * @brief get minimum of image data
	 * @return minimum value in data
	*/
	double GetMinimum( void ) const{ return grayscale_image_.GetMinimum(); };

	/*!
	 * @brief get maximum of image data
	 * @return maximum value in data
	*/
	double GetMaximum( void ) const{ return grayscale_image_.GetMaximum(); };

	/*!
	 * @brief get value for given x and y coordinate
	 * @param x local x coordinate
	 * @param y local y coordinate
	 * @return raw and color value when x and y are inside image
	*/
	optional<pair<double, RGB>> GetValue( int x, int y ) const;

	/*!
	 * @brief assign grayscale image as new image data
	 * @param grayscale_image grayscale image
	*/
	void AssignImage( const GrayscaleImage& grayscale_image );

	/*!
	 * @brief assign gridded voxel data 
	 * @param data_grid data grid
	 * @param auto_adjust_contrast flag for normalisation
	*/
	void AssignImage( const DataGrid<VoxelData>& data_grid, const bool auto_adjust_contrast = true );

	/*!
	 * @brief change the images contrast to given range
	 * @param new_contrast range of value to show. value over or under values in range will be bounded
	*/
	void AdjustContrast( const NumberRange new_contrast);
	
	/*!
	 * @brief draw the image
	*/
	void draw( void ) override;

	/*!
	 * @brief resize the image
	 * @param x new x postion
	 * @param y new y positon
	 * @param w new width
	 * @param h new height
	*/
	void resize( int x, int y, int w, int h ) override;

	/*!
	 * @brief get size of scaled image
	 * @return width and height of scaled image
	*/
	Index2D GetImageSize( void ) const{ return {grayscale_image_scaled_.width(), grayscale_image_scaled_.height() }; };

	/*!
	 * @brief get size of original image 
	 * @return width and height of original image
	*/
	Index2D GetOriginalImageSize( void ) const{ return {grayscale_image_.width(), grayscale_image_.height() }; };


	private:

	GrayscaleImage grayscale_image_;	/*!< raw grayscale imagedata*/
	bool image_assigned_;							/*!< flag to track whether an image has been assigned*/
	
	vector<pair<bool, RGB>> overlay_;	/*!< overlay as collection of a flag and color value. At assignment initialised with an element for each pixel*/
	bool has_overlay_;								/*!< flag indicating existing overlay*/
	
	GrayscaleImage grayscale_image_scaled_;	/*!< scaled copy of original image*/
	ColorImage color_image_;								/*!< colored image resulting from grayscale image with overlayed color data_*/


	/*!
	 * @brief calculate new image when size changed
	*/
	void CalculateScaled( void );

	/*!
	 * @brief update and redraw image when size changed
	*/
	void Update( void );
};