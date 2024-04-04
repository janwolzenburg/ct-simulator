#pragma once
/*********************************************************************
 * @file   fl_AdjustableGrayscaleImage.h
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
#include "fl_GrayscaleImageWithAxis.h"


 /*********************************************************************
	Definitions
 *********************************************************************/

/*!
 * @brief class for a grayscale image with optional colored overlay and adjustable contrast
*/
class Fl_AdjustableGrayscaleImage : public Fl_Group{

	public:

	/*!
	 * @brief constructor
	 * @param x x-position
	 * @param y y-position
	 * @param w width
	 * @param h height
	 * @param label label
	*/
	Fl_AdjustableGrayscaleImage( int x, int y, int w, int h, const char* label = 0L );

	/*!
	 * @brief get the current slider values 
	 * @return current contrast
	*/
	NumberRange GetContrast( void ) const{ return NumberRange{ lower_bound_.value() * pow( 10., -common_power_ ), upper_bound_.value()* pow( 10., -common_power_ ) }; };

	/*!
	 * @brief check if an image was assigned 
	 * @return true when an image has been assigned
	*/
	bool image_assigned( void ) const{ return image_widget_.image_assigned(); };

	/*!
	 * @brief check if contrast changed and unset the flag
	 * @return true when contrast changed
	*/
	bool DidContrastChange( void ){ return UnsetFlag( contrast_changed_ );  };

	/*!
	 * @brief reset slider bounds
	*/
	void ResetBounds( void ){ bounds_set_ = false; };

	/*!
	 * @brief handle FLTK events
	 * @param event event number
	 * @return 1 if handled, 0 otheriwse
	*/
	int handle( int event );

	/*!
	 * @brief assign grayscale image as new image data
	 * @param grayscale_image grayscale image
	*/
	void AssignImage( const GrayscaleImage& grayscale_image );

	/*!
	 * @brief assign gridded voxel data 
	 * @param data_grid data grid to make into the image
	*/
	void AssignImage( const DataGrid<VoxelData>& data_grid );

	/*!
	 * @brief change slider values
	 * @param new_contrast lower and upper limit. values below lower limit are black; values beyond upper limit are white
	*/
	void ChangeSliderValues( const NumberRange new_contrast );

	/*!
	 * @brief set the slider range
	 * @param new_slider_bounds raw limits
	*/
	void SetSliderBounds( const NumberRange new_slider_bounds );

	/*!
	 * @brief set the contrast adjustment slider range by data values in image
	*/
	void SetSliderBoundsFromImage( void );

	/*!
	 * @brief set the tooltip text for the value
	 * @param tooltip_text new text
	*/
	void SetValueTip( const string tooltip_text );

	/*!
	 * @brief set axis label
	 * @param label x and y label
	*/
	void Setlabel( const pair<string, string> label ){ image_widget_.SetAxisLabel( label ); };

	/*!
	 * @brief set axis tics
	 * @param pixel_start start of tics
	 * @param pixel_size amount of tics
	*/
	void SetAxis( const Tuple2D pixel_start, const Tuple2D pixel_size, const Index2D number_of_tics ){ image_widget_.SetAxis( pixel_start, pixel_size, number_of_tics ); };


	private:

	Fl_GrayscaleImageWithAxis image_widget_;	/*!< the image widget*/
	Fl_Hor_Value_Slider lower_bound_;					/*!< low bound*/
	Fl_Hor_Value_Slider upper_bound_;					/*!< high bound*/
			
	Fl_Box current_value_text_;			/*!< output for common factor*/
	Fl_Box current_value_tip_;			/*!< output for common factor*/
	Fl_Box common_factor_text_;			/*!< output for common factor*/

	int common_power_;				/*!< common power of ten*/
	bool bounds_set_;					/*!< flag indicating whether the bounds were set before*/
	bool contrast_changed_;		/*!< flag indicating constrast change*/


	/*!
	 * @brief callback for value change
	 * @param widget pointer to widget that triggered the callback
	*/
	static void HandleValueChange( Fl_Widget* widgetPtr, void* image_widget );

};