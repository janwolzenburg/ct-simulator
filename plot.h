#pragma once
/*********************************************************************
 * @file   guiPlot.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   April 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

 #include "generel.h"
 #include "sciplot/sciplot.hpp"

/*!
 * @brief Class to store property of axis limtis
*/
struct PlotLimits{

	bool autoXRange = true;				/*!<Enable or disable automatic x-axis limits*/
	bool autoYRange = true;				/*!<Enable or disable automatic y-axis limits*/

	NumberRange xRange = NumberRange{ 0., 1. };		/*!<x-range*/
	NumberRange yRange = NumberRange{ 0., 1. };		/*!<y-range*/

	double xFactor = 1.;				/*!<Factor for x-axis sclaing*/
	double yFactor = 1.;				/*!<Factor for y-axis sclaing*/

 };


/*!
 * @brief Class for a scientific plot using sciplot library 
*/
class Plot{

	public:

	/*!
	 * @brief Get string of path where plot image is stored
	 * @return String with path
	*/
	path image_path( void ) const { return image_path_; };

	/*!
	 * @brief Get the image size
	 * @return Current image size
	*/
	GridIndex image_size( void ) const{ return image_size_; };

	/*!
	 * @brief Initialise plot
	 * @param image_path Storage path for plot image
	 * @param label Plot label
	 * @param x_label x-axis label
	 * @param y_label y-axis label
	 * @param limits Plot limits
	 * @param image_size Size of image in pixel
	 * @param x_format x-ticks format string
	 * @param y_format y-ticks format string
	 * @param are_axis_equal Flag to make axis equally scaled
	 * @param enable_grid Flag to show grid
	*/
	void Initialise( const path image_path, const string label, const string x_label, const string y_label, const PlotLimits limits, const GridIndex image_size,
					 const string x_format, const string y_format, const bool are_axis_equal, const bool enable_grid );
	/*!
	 * @brief Set plot limits
	 * @param newLimits New limits
	*/
	void SetLimits( const PlotLimits newLimits ){ limits_ = newLimits; };

	/*!
	 * @brief Set image size
	 * @param size New size of image
	*/
	void SetSize( const GridIndex size ){ image_size_ = size; };

	/*!
	 * @brief Redraw the plot
	*/
	void redraw( void ){ reset(); DrawPlot(); };


	protected:

	/*!
	 * @brief Constructor
	 * @param image_path Storage path for plot image
	 * @param x_label x-axis label
	 * @param y_label y-axis label
	 * @param limits Plot limits
	 * @param image_size Desired image size
	 * @param enable_grid Flag to show grid
	*/
	Plot( const path image_path, const string x_label, const string y_label,
		  const PlotLimits limits, const GridIndex image_size, const bool enable_grid );

	/*!
	 * @brief Default constructor
	*/
	Plot( void ) : Plot{ "Dummy", "X", "Y", PlotLimits{}, GridIndex{ 5, 5 }, false }{};

	/*!
	 * @brief Reset the plot
	*/
	void reset( void );

	/*!
	 * @brief Draw the plot
	 * @details Draws and stored plot image at given path
	*/
	void DrawPlot( void );
	

	protected:
	
	string label_;	/*!<Figure label*/
	path image_path_;	/*!<Path to image. Plots are exported to this direction*/
	string x_label_;	/*!<Label of x-axis*/
	string y_label_;	/*!<Lable of y-axis*/

	bool are_axis_equal_ = false;	/*!<Flag to make axis equally scaled*/
	bool enable_grid_ = false;		/*!<Flag to make grid visible*/

	string x_format_;		/*!<Format string for x-axis tick values*/
	string y_format_;		/*!<Format string for y-axis tick values*/
	PlotLimits limits_;	/*!<Limits of plot*/

	GridIndex image_size_;				/*!<Size of image*/
	sciplot::Plot2D plot_2D_;		/*!<Instance of sciplot 2D-plot*/

 };
