#pragma once
/*********************************************************************
 * @file   lineplot.h
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
 * @brief Class for a 2D lineplot
*/
class lineplot : public plot{

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
	lineplot( const string name_, const string xlabel_, const string ylabel_,
			  const plotLimits limits_, const idx2CR imgSize_, const bool grid_ );

	/*!
	 * @brief Default constructor 
	*/
	lineplot( void );

	/*!
	 * @brief Assign data to plot
	 * @param X_ Vector of x values
	 * @param Y_ Vector of y values
	*/
	void assignData( const vector<double> X_, const vector<double> Y_ );

	/*!
	 * @brief Assign data to plot
	 * @param XY Pair of vectors with data
	*/
	void assignData( const vectorPair XY ){ assignData( XY.first, XY.second ); };

	/*!
	 * @brief Create the plot
	*/
	void create( void );


	protected:

	vector<double> X, Y;	/*!<Vectors with points*/

};


/*!
 * @brief Class for a 2D dot-plot
*/
class dotplot : public lineplot{

	using lineplot::lineplot;

	public:

	/*!
	 * @brief Create the plot
	*/
	void create( void );

};