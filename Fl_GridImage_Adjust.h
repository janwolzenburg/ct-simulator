#pragma once
/*********************************************************************
 * @file   Fl_GridImage_Adjust.h
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
#include "Fl_GridImage.h"


 /*********************************************************************
	Definitions
 *********************************************************************/

/*!
 * @brief Class for a grayscale image with optional colored overlay and adjustable contrast
*/
class Fl_GridImage_Adjust : public Fl_Group{

	public:

	/*!
	 * @brief Constructor
	 * @param x x-position
	 * @param y y-position
	 * @param w Width
	 * @param h Height
	 * @param label Label
	*/
	Fl_GridImage_Adjust( int x, int y, int w, int h, const char* label = 0L );

	/*!
	 * @brief Change contrast
	 * @param bounds Lower and upper limit. Values below lower limit are black; values beyond upper limit are white
	*/
	void changeContrast( const NumberRange bounds );

	/*!
	 * @brief Assign grayscale image as new image data
	 * @param img Grayscale image
	*/
	void assignImage( const monoImage& img );

	/*!
	 * @brief Assign gridded voxel data 
	 * @param modGrid Data grid
	 * @param Normalise Flag for normalisation
	*/
	void assignImage( const grid<voxData>& modGrid, const bool normalise = false );

	/*!
	 * @brief Set the slider range
	 * @param newBound Raw limits
	*/
	void setSliderBounds( const NumberRange newBound );

	/*!
	 * @brief Handle events
	 * @return True when image needs update
	*/
	bool handleEvents( void );

	/*!
	 * @brief Set the contrast adjustment slider range by data values in image
	*/
	void setSliderBoundsFromImage( void );

	/*!
	 * @brief Get the current slider values 
	 * @return Current contrast
	*/
	NumberRange getContrast( void ) const{ return NumberRange{ lowerBound.value(), upperBound.value() }; };

	/*!
	 * @brief Check if an image was assigned 
	 * @return 
	*/
	bool imageAssigned( void ) const{ return imgWidget.imageAssigned(); };


	private:
	
	Fl_GridImage imgWidget;						/*!<The image*/
	Fl_OwnValuator<Fl_Hor_Slider> lowerBound;	/*!<Slider for lower value bound*/
	Fl_OwnValuator<Fl_Hor_Slider> upperBound;	/*!<Slider for upper value bound*/
	bool boundsSet;								/*!<Flag indicating whether the bounds were set before*/


};