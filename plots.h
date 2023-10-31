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
struct plotLimits{

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
class plot{

	public:

	/*!
	 * @brief Get string of path where plot image is stored
	 * @return String with path
	*/
	string getImgPath( void ) const { return imagePath.string(); };

	/*!
	 * @brief Initialise the plot
	 * @param path_ Storage path for plot image
	 * @param label_ Figure label
	 * @param xlabel_ x-axis label
	 * @param ylabel_ y-axis label
	 * @param limits_ Plot limits
	 * @param imgSize_ Desired size of image
	 * @param xFormat_ x-ticks format string
	 * @param yFormat_ y-ticks format string
	 * @param axisEqual_ Flag to make axis equally scaled
	 * @param grid_ Flag to show grid
	*/
	void initialise( const path path_, const string label_, const string xlabel_, const string ylabel_,
					 const plotLimits limits_, const GridIndex imgSize_, const string xFormat_, const string yFormat_, const bool axisEqual_, const bool grid_ );

	/*!
	 * @brief Set plot limits
	 * @param newLimits New limits
	*/
	void setLimits( const plotLimits newLimits ){ limits = newLimits; };

	/*!
	 * @brief Set image size
	 * @param size New size of image
	*/
	void setSize( const GridIndex size ){ imgSize = size; };

	/*!
	 * @brief Get the image size
	 * @return Current image size
	*/
	GridIndex getSize( void ) const{ return imgSize; };

	/*!
	 * @brief Redraw the plot
	*/
	void redraw( void ){ reset(); drawPlot(); };


	protected:

	/*!
	 * @brief Constructor
	 * @param imgPath_ Storage path for plot image
	 * @param xlabel_ x-axis label
	 * @param ylabel_ y-axis label
	 * @param limits_ Plot limits
	 * @param imgSize_ Desired image size
	 * @param grid_ Flag to show grid
	*/
	plot( const path imgPath_, const string xlabel_, const string ylabel_,
		  const plotLimits limits_, const GridIndex imgSize_, const bool grid_ );

	/*!
	 * @brief Default constructor
	*/
	plot( void ) : plot{ "Dummy", "X", "Y", plotLimits{}, GridIndex{ 5, 5 }, false }{};

	/*!
	 * @brief Reset the plot
	*/
	void reset( void );

	/*!
	 * @brief Draw the plot
	 * @details Draws and stored plot image at given path
	*/
	void drawPlot( void );
	

	protected:
	
	string label;	/*!<Figure label*/
	path imagePath;	/*!<Path to image. Plots are exported to this direction*/
	string xlabel;	/*!<Label of x-axis*/
	string ylabel;	/*!<Lable of y-axis*/

	bool axisEqual = false;	/*!<Flag to make axis equally scaled*/
	bool grid = false;		/*!<Flag to make grid visible*/

	string xFormat;		/*!<Format string for x-axis tick values*/
	string yFormat;		/*!<Format string for y-axis tick values*/
	plotLimits limits;	/*!<Limits of plot*/

	GridIndex imgSize;				/*!<Size of image*/
	sciplot::Plot2D plot2D;		/*!<Instance of sciplot 2D-plot*/

 };
