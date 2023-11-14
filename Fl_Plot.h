#pragma once
/*********************************************************************
 * @file   Fl_Plot.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   April 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

 #include <memory>

#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Widget.H>
#include "plot.h"

/*!
 * @brief A plot widget
 * @tparam plotType Type of plot
*/
template< class P >
class Fl_Plot : public Fl_Widget{

	static_assert( std::is_base_of_v<Plot, P> );


	public:

	/*!
	 * @brief Constructor
	 * @param x x-position
	 * @param y y-position
	 * @param w Width
	 * @param h Height
	 * @param label Label
	*/
	Fl_Plot( int x, int y, int w, int h,  const char* label = 0L );

	/*!
	 * @brief Initialise plot
	 * @param image_path Storage path for plot image
	 * @param x_label x-axis label
	 * @param y_label y-axis label
	 * @param limits Plot limits
	 * @param x_format x-ticks format string
	 * @param y_format y-ticks format string
	 * @param are_axis_equal Flag to make axis equally scaled
	 * @param enable_grid Flag to show grid
	*/
	void Initialise( const path image_path, const string x_label, const string y_label, const PlotLimits limits, 
					 const string x_format, const string y_format, const bool are_axis_equal, const bool enable_grid );

	/*!
	 * @brief Set new axis limits
	 * @param newLimits New limits
	*/
	void SetLimits( const PlotLimits limits ){ plot_.SetLimits( limits ); };

	/*!
	 * @brief Get reference to plot
	 * @return Underlying plot
	*/
	P& plot( void ){ return plot_; };

	/*!
	 * @brief Assign image data from persisting storage
	*/
	void AssignData( void ){ AssignImage( plot_.image_path() ); };

	/*!
	 * @brief Draw plot as image
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

	/*!
	 * @brief Calculate scaled plot image
	*/
	void CalculateScaled( void );


	private:

	string label_;				/*!< Label*/
	P plot_;		/*!< A plot*/
	std::unique_ptr<Fl_PNG_Image> raw_image_;	/*!< Raw image*/
	std::unique_ptr<Fl_Image> image_;			/*!< Scaled FL-image*/


	/*!
	 * @brief Assign plot image
	 * @param filename Path to image
	*/
	void AssignImage( const path filename );

};


#include "fl_Plot.hpp"