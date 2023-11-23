#pragma once
/*********************************************************************
 * @file   fl_GrayscaleImage.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   September 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

 #include <FL/Fl_Widget.H>
 #include <FL/Fl_Group.H>

 #include <optional>
 using std::optional;

#include "grayscaleImage.h"
#include "colorImage.h"

 /*********************************************************************
	Definitions
 *********************************************************************/


/*!
 * @brief Class for a Fl_Widget to display grayscale image with optional colored overlay
*/
class Fl_GrayscaleImage : public Fl_Widget{

	public:

	static const RGB background_color;		/*!< Background color for pixel without data*/
	static const RGB metal_color;			/*!< Overlay color of metal*/

	/*!
	 * @brief Constructor
	 * @param x x-position
	 * @param y y-position
	 * @param w Width
	 * @param h Height
	 * @param label Label
	*/
	Fl_GrayscaleImage( int x, int y, int w, int h, const char* label = 0L );

	/*!
	 * @brief Check if image is assigned
	 * @return True when an image was previously assigned
	*/
	bool image_assigned( void ) const{ return image_assigned_; };

	/*!
	 * @brief Get minimum of image data
	 * @return Minimum value in data
	*/
	double GetMinimum( void ) const{ return grayscale_image_.GetMinimum(); };

	/*!
	 * @brief Get maximum of image data
	 * @return maximum value in data
	*/
	double GetMaximum( void ) const{ return grayscale_image_.GetMaximum(); };

	/*!
	 * @brief Get value for given x and y coordinate
	 * @param x Local x coordinate
	 * @param y Local y coordinate
	 * @return Raw and color value when x and y are inside image
	*/
	optional<pair<double, RGB>> GetValue( int x, int y ) const;

	/*!
	 * @brief Assign grayscale image as new image data
	 * @param grayscale_image Grayscale image
	*/
	void AssignImage( const GrayscaleImage& grayscale_image );

	/*!
	 * @brief Assign gridded voxel data 
	 * @param data_grid Data grid
	 * @param auto_adjust_contrast Flag for normalisation
	*/
	void AssignImage( const DataGrid<VoxelData>& data_grid, const bool auto_adjust_contrast = true );

	/*!
	 * @brief Change the images contrast to given range
	 * @param data_range Range of value to show. Value over or under values in range will be bounded
	*/
	void AdjustContrast( const NumberRange new_contrast);
	
	/*!
	 * @brief Draw the image
	*/
	void draw( void ) override;

	/*!
	 * @brief Resize the image
	 * @param x New x postion
	 * @param y New y positon
	 * @param w New width
	 * @param h New height
	*/
	void resize( int x, int y, int w, int h ) override;

	Index2D GetImageSize( void ) const{ return {grayscale_image_scaled_.width(), grayscale_image_scaled_.height() }; };

	Index2D GetOriginalImageSize( void ) const{ return {grayscale_image_.width(), grayscale_image_.height() }; };


	private:

	GrayscaleImage grayscale_image_;	/*!< Raw grayscale imagedata*/
	bool image_assigned_;				/*!< Flag to track whether an image has been assigned*/
	
	vector<pair<bool, RGB>> overlay_;	/*!< Overlay as collection of a flag and color value. At assignment initialised with an element for each pixel*/
	bool has_overlay_;					/*!< Flag indicating existing overlay*/
	
	GrayscaleImage grayscale_image_scaled_;	/*!< Scaled copy of original image*/
	ColorImage color_image_;			/*!< Colored image resulting from grayscale image with overlayed color data_*/


	/*!
	 * @brief Calculate new image when size changed
	*/
	void CalculateScaled( void );

	/*!
	 * @brief Update and redraw image when size changed
	*/
	void Update( void );
};


class Fl_GrayscaleImageWithAxis : public Fl_Group, public Fl_GrayscaleImage{

	public:

	Fl_GrayscaleImageWithAxis( int x, int y, int w, int h, const char* label = 0L );

	int x( void )const { return Fl_Group::x(); };

	int y( void )const { return Fl_Group::y(); };

	int w( void )const { return Fl_Group::w(); };

	int h( void )const { return Fl_Group::h(); };

	void SetAxisLabel( const pair<string, string> label );

	void SetAxis( const Tuple2D pixel_start, const Tuple2D pixel_size );

	/*!
	 * @brief Draw the image
	*/
	void draw( void ) override;
	
	/*!
	 * @brief Get value for given x and y coordinate
	 * @param x Local x coordinate
	 * @param y Local y coordinate
	 * @return Raw and color value when x and y are inside image
	
	*/
	optional<pair<double, RGB>> GetValue( int x, int y ) const{ 
		return Fl_GrayscaleImage::GetValue( x - axis_space_ / 2, y - axis_space_ / 2); };


	private:

	constexpr static int axis_space_ = 30;
	
	pair<string, string> axis_label_;
	Tuple2D image_start_;
	Tuple2D pixel_size_;

};