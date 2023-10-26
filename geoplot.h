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

#include "plots.h"

/*!
 * @brief Class for a 2D plot for lines and points
*/
class geoplot : public plot{

	public:

	/*!
	 * @brief Constructor
	 * @param name_ Name of plot
	 * @param xlabel_ x-axis label
	 * @param ylabel_ y-axis label
	 * @param limits_ Plot limits
	 * @param imgSize_ Desired image size
	 * @param grid_ Flag to show grid
	*/
	geoplot( const string name_, const string xlabel_, const string ylabel_,
			 const plotLimits limits_, const GridIndex imgSize_, const bool grid_ ) :
			plot{ name_, xlabel_, ylabel_, limits_, imgSize_, grid_ }, 
			lines( 0, std::pair<Tuple2D, Tuple2D>{} ), points( 0, Tuple2D{} ){};

	/*!
	 * @brief Default constructor
	*/
	geoplot( void ) : plot{}{};

	/*!
	 * @brief Add a line to plot
	 * @param start Start of line
	 * @param end End of line
	*/
	void addLine( const Tuple2D start, const Tuple2D end ){
		lines.emplace_back( Tuple2D{ limits.xFactor * start.x, limits.yFactor * start.y }, Tuple2D{ limits.xFactor * end.x, limits.yFactor * end.y } ); };

	/*!
	 * @brief Add point to plot
	 * @param point Point coordiantes
	*/
	void addPoint( const Tuple2D point ){
		points.emplace_back( Tuple2D{ limits.xFactor * point.x, limits.yFactor * point.y } ); };

	/*!
	 * @brief Remove all lines and points
	*/
	void resetObjects( void );

	/*!
	 * @brief Create the plot
	*/
	void create( void );


	private:

	vector<std::pair<Tuple2D, Tuple2D>> lines;	/*!<Lines in plot*/
	vector<Tuple2D> points;					/*!<Points in plot*/

};
