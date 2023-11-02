#pragma once
/*********************************************************************
 * @file   fl_AdjustableGrayscaleImage.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   October 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include <FL/Fl_Group.H>
#include <FL/Fl_Hor_Slider.H>
#include "Fl_OwnValuator.h"
#include "fl_GrayscaleImage.h"


 /*********************************************************************
	Definitions
 *********************************************************************/

/*!
 * @brief Class for a grayscale image with optional colored overlay and adjustable contrast
*/
class Fl_AdjustableGrayscaleImage : public Fl_Group{

	public:

	/*!
	 * @brief Constructor
	 * @param x x-position
	 * @param y y-position
	 * @param w Width
	 * @param h Height
	 * @param label Label
	*/
	Fl_AdjustableGrayscaleImage( int x, int y, int w, int h, const char* label = 0L );

	/*!
	 * @brief Get the current slider values 
	 * @return Current contrast
	*/
	NumberRange GetContrast( void ) const{ return NumberRange{ lower_bound_.value(), upper_bound_.value() }; };

	/*!
	 * @brief Check if an image was assigned 
	 * @return True when an image has been assigned
	*/
	bool image_assigned( void ) const{ return image_widget_.image_assigned(); };
	
	/*!
	 * @brief Assign grayscale image as new image data
	 * @param grayscale_image Grayscale image
	*/
	void AssignImage( const GrayscaleImage& grayscale_image );

	/*!
	 * @brief Assign gridded voxel data 
	 * @param data_grid Data grid
	 * @param Normalise Flag for normalisation
	*/
	void AssignImage( const DataGrid<VoxelData>& data_grid );

	/*!
	 * @brief Change slider values
	 * @param bounds Lower and upper limit. Values below lower limit are black; values beyond upper limit are white
	*/
	void ChangeSliderValues( const NumberRange new_contrast );

	/*!
	 * @brief Set the slider range
	 * @param newBound Raw limits
	*/
	void SetSliderBounds( const NumberRange new_slider_bounds );

	/*!
	 * @brief Set the contrast adjustment slider range by data values in image
	*/
	void SetSliderBoundsFromImage( void );

	/*!
	 * @brief Handle events
	 * @return True when image needs update
	*/
	bool HandleEvents( void );


	private:
	
	Fl_GrayscaleImage image_widget_;				/*!<The image widget*/
	Fl_OwnValuator<Fl_Hor_Slider> lower_bound_;		/*!<Slider for lower value bound*/
	Fl_OwnValuator<Fl_Hor_Slider> upper_bound_;		/*!<Slider for upper value bound*/
	bool bounds_set_;								/*!<Flag indicating whether the bounds were set before*/

	/*!
	 * @brief Update slider bounds
	 * @return True when bounds changed an image need update
	*/
	bool UpdateSliderBounds( void );

};