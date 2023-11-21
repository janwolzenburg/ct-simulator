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
#include <FL/Fl_Hor_Value_Slider.H>
#include <FL/Fl_Box.H>
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
	NumberRange GetContrast( void ) const{ return NumberRange{ lower_bound_.value() * pow( 10., -common_power_ ), upper_bound_.value()* pow( 10., -common_power_ ) }; };

	/*!
	 * @brief Check if an image was assigned 
	 * @return True when an image has been assigned
	*/
	bool image_assigned( void ) const{ return image_widget_.image_assigned(); };

	/*!
	 * @brief Check if contrast changed and unset the flag
	 * @return True when contrast changed
	*/
	bool DidContrastChange( void ){ return UnsetFlag( contrast_changed_ );  };

	/*!
	 * @brief Reset slider bounds
	*/
	void ResetBounds( void ){ bounds_set_ = false; };

	/*!
	 * @brief Handle FLTK events
	 * @param event Eventnumber
	 * @return 1 if handled, 0 otheriwse
	*/
	int handle( int event );

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
	 * @brief Set the tooltip text for the value
	 * @param tooltip_text new text
	*/
	void SetValueTooltip( const string tooltip_text );

	private:

	Fl_GrayscaleImage image_widget_;	/*!< The image widget*/
	Fl_Hor_Value_Slider lower_bound_;	/*!< Low bound*/
	Fl_Hor_Value_Slider upper_bound_;	/*!< High bound*/
			
	Fl_Box current_value_text_;			/*!< Output for common factor*/
	Fl_Box common_factor_text_;			/*!< Output for common factor*/

	int common_power_;					/*!< Common power of ten*/
	bool bounds_set_;					/*!< Flag indicating whether the bounds were set before*/
	bool contrast_changed_;				/*!< Flag indicating constrast change*/


	/*!
	 * @brief Callback for value change
	 * @param widget Pointer to widget that triggered the callback
	*/
	static void HandleValueChange( Fl_Widget* widgetPtr, void* image_widget );

};