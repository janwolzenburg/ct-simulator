#pragma once
/*********************************************************************
 * @file   fl_GrayscaleImageWithAxis.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   November 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

 #include <FL/Fl_Group.H>

 #include <optional>
 using std::optional;

#include "fl_GrayscaleImage.h"


 /*********************************************************************
	Definitions
 *********************************************************************/


/*!
 * @brief grayscale image with axis
*/
class Fl_GrayscaleImageWithAxis : public Fl_Group, public Fl_GrayscaleImage{


	public:

	/*!
	 * @brief constructor
	 * @param x x-position
	 * @param y y-position
	 * @param w Width
	 * @param h Height
	 * @param label Label
	*/
	Fl_GrayscaleImageWithAxis( int x, int y, int w, int h, const char* label = 0L );

	int x( void )const { return Fl_Group::x(); };

	int y( void )const { return Fl_Group::y(); };

	int w( void )const { return Fl_Group::w(); };

	int h( void )const { return Fl_Group::h(); };

	/*!
	 * @brief set label of axis
	 * @param label x and y label
	*/
	void SetAxisLabel( const pair<string, string> label );

	/*!
	 * @brief set axis tics
	 * @param pixel_start Start of tics
	 * @param pixel_size Amount of tics
	*/
	void SetAxis( const Tuple2D pixel_start, const Tuple2D pixel_size, const Index2D number_of_tics );

	/*!
	 * @brief draw the image
	*/
	void draw( void ) override;
	
	/*!
	 * @brief get value for given x and y coordinate
	 * @param x Local x coordinate
	 * @param y Local y coordinate
	 * @return Raw and color value when x and y are inside image
	*/
	optional<pair<double, RGB>> GetValue( int x, int y ) const{ 
		return Fl_GrayscaleImage::GetValue( x - axis_space_ / 2, y - axis_space_ / 2); };


	private:

	constexpr static int axis_space_ = 30;	/*!< amount of pixel the axis are spaced from image*/
	
	pair<string, string> axis_label_;		/*!< Label of axis*/
	Tuple2D axis_tics_start_;	/*!< start value of tics*/
	Tuple2D pixel_size_;		/*!< what is one pixel's size in reality*/
	Index2D number_of_tics_;
};