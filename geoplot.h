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
			 const plotLimits limits_, const idx2CR imgSize_, const bool grid_ ) :
			plot{ name_, xlabel_, ylabel_, limits_, imgSize_, grid_ }, 
			lines( 0, std::pair<v2, v2>{} ), points( 0, v2{} ){};

	/*!
	 * @brief Default constructor
	*/
	geoplot( void ) : plot{}{};

	/*!
	 * @brief Add a line to plot
	 * @param start Start of line
	 * @param end End of line
	*/
	void addLine( const v2 start, const v2 end ){
		lines.emplace_back( v2{ limits.xFactor * start.x, limits.yFactor * start.y }, v2{ limits.xFactor * end.x, limits.yFactor * end.y } ); };

	/*!
	 * @brief Add point to plot
	 * @param point Point coordiantes
	*/
	void addPoint( const v2 point ){
		points.emplace_back( v2{ limits.xFactor * point.x, limits.yFactor * point.y } ); };

	/*!
	 * @brief Remove all lines and points
	*/
	void resetObjects( void );

	/*!
	 * @brief Create the plot
	*/
	void create( void );


	private:

	vector<std::pair<v2, v2>> lines;	/*!<Lines in plot*/
	vector<v2> points;					/*!<Points in plot*/

};
