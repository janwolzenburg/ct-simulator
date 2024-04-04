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

 #include "plot.h"

/*!
 * @brief class for a 2D lineplot
*/
class LinePlot : public Plot{

	public:

	/*!
	 * @brief constructor
	 * @param name Name of plot
	 * @param x_label x-axis label
	 * @param y_label y-axis label
	 * @param limits Plot limits
	 * @param image_size Desired image size_
	 * @param enable_grid Flag to show grid
	*/
	LinePlot( const string name, const string x_label, const string y_label,
			  const PlotLimits limits, const GridIndex image_size, const bool enable_grid );

	/*!
	 * @brief default constructor 
	*/
	LinePlot( void );

	/*!
	 * @brief assign data to plot
	 * @param X_ Vector of x values
	 * @param Y_ Vector of y values
	*/
	void AssignData( const vector<double> X_, const vector<double> Y_ );

	/*!
	 * @brief assign data to plot
	 * @param XY Pair of vectors with data_
	*/
	void AssignData( const VectorPair XY ){ AssignData( XY.first, XY.second ); };

	/*!
	 * @brief create the plot
	*/
	virtual void CreatePlot( void );


	protected:

	vector<double> X, Y;	/*!< Vectors with points*/

};


/*!
 * @brief class for a 2D dot-plot
*/
class DotPlot : public LinePlot{

	using LinePlot::LinePlot;

	public:

	/*!
	 * @brief create the plot
	*/
	void CreatePlot( void ) override;

};