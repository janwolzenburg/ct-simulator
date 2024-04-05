#pragma once
/*********************************************************************
 * @file   geometryPlot.h
 *
 * @author Jan Wolzenburg
 * @date   April 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include "plot.h"

/*!
 * @brief class for a 2D plot for lines and points
*/
class Geometryplot : public Plot{

	public:

	/*!
	 * @brief constructor
	 * @param name name of plot
	 * @param x_label x-axis label
	 * @param y_label y-axis label
	 * @param limits plot limits
	 * @param image_size desired image size
	 * @param enable_grid flag to show grid
	*/
	Geometryplot( const string name, const string x_label, const string y_label,
				  const PlotLimits limits, const GridIndex image_size, const bool enable_grid ) :
			Plot{ name, x_label, y_label, limits, image_size, enable_grid }, 
			lines_( 0, std::pair<Tuple2D, Tuple2D>{} ), points_( 0, { Tuple2D{}, string{} } ){};

	/*!
	 * @brief default constructor
	*/
	Geometryplot( void ) : Plot{}{};

	/*!
	 * @brief add a line to plot
	 * @param start start of Line
	 * @param end end of Line
	*/
	void AddLine( const Tuple2D start, const Tuple2D end ){
		lines_.emplace_back( Tuple2D{ limits_.xFactor * start.x, limits_.yFactor * start.y }, Tuple2D{ limits_.xFactor * end.x, limits_.yFactor * end.y } ); };

	/*!
	 * @brief add point to plot
	 * @param point point coordiantes
	 * @param name name of the point
	*/
	void AddPoint( const Tuple2D point, const string name = string{} ){
		points_.emplace_back( Tuple2D{ limits_.xFactor * point.x, limits_.yFactor * point.y }, name ); };

	/*!
	 * @brief remove all lines and points
	*/
	void ResetObjects( void );

	/*!
	 * @brief create the plot
	*/
	void CreatePlot( void );


	private:

	vector<std::pair<Tuple2D, Tuple2D>> lines_;			/*!< lines in plot*/
	vector<pair<Tuple2D, string>> points_;					/*!< points in plot*/

};
