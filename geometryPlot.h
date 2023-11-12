#pragma once
/*********************************************************************
 * @file   geoplot.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   April 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include "plot.h"

/*!
 * @brief Class for a 2D plot for lines and points
*/
class Geometryplot : public Plot{

	public:

	/*!
	 * @brief Constructor
	 * @param name Name of plot
	 * @param x_label x-axis label
	 * @param y_label y-axis label
	 * @param limits Plot limits
	 * @param image_size Desired image size_
	 * @param enable_grid Flag to show grid
	*/
	Geometryplot( const string name, const string x_label, const string y_label,
				  const PlotLimits limits, const GridIndex image_size, const bool enable_grid ) :
			Plot{ name, x_label, y_label, limits, image_size, enable_grid }, 
			lines_( 0, std::pair<Tuple2D, Tuple2D>{} ), points_( 0, Tuple2D{} ){};

	/*!
	 * @brief Default constructor
	*/
	Geometryplot( void ) : Plot{}{};

	/*!
	 * @brief Add a Line to plot
	 * @param start Start of Line
	 * @param end End of Line
	*/
	void AddLine( const Tuple2D start, const Tuple2D end ){
		lines_.emplace_back( Tuple2D{ limits_.xFactor * start.x, limits_.yFactor * start.y }, Tuple2D{ limits_.xFactor * end.x, limits_.yFactor * end.y } ); };

	/*!
	 * @brief Add point to plot
	 * @param point Point coordiantes
	*/
	void AddPoint( const Tuple2D point ){
		points_.emplace_back( Tuple2D{ limits_.xFactor * point.x, limits_.yFactor * point.y } ); };

	/*!
	 * @brief Remove all lines and points
	*/
	void ResetObjects( void );

	/*!
	 * @brief Create the plot
	*/
	void CreatePlot( void );


	private:

	vector<std::pair<Tuple2D, Tuple2D>> lines_;	/*!< Lines in plot*/
	vector<Tuple2D> points_;					/*!< Points in plot*/

};
