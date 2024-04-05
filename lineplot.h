#pragma once
/*********************************************************************
 * @file   linePlot.h
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
	 * @param name name of plot
	 * @param x_label x-axis label
	 * @param y_label y-axis label
	 * @param limits plot limits
	 * @param image_size desired image size
	 * @param enable_grid flag to show grid
	*/
	LinePlot( const string name, const string x_label, const string y_label,
			  const PlotLimits limits, const GridIndex image_size, const bool enable_grid );

	/*!
	 * @brief default constructor 
	*/
	LinePlot( void );

	/*!
	 * @brief assign data to plot
	 * @param x_values vector with x-values
	 * @param y_values vector with y-values
	*/
	void AssignData( const vector<double> x_values, const vector<double> y_values );

	/*!
	 * @brief assign data to plot
	 * @param x_y_values pair of vectors with x and y values
	*/
	void AssignData( const VectorPair x_y_values ){ AssignData( x_y_values.first, x_y_values.second ); };

	/*!
	 * @brief create the plot
	*/
	virtual void CreatePlot( void );


	protected:

	vector<double> x_values;	/*!< x values of points*/
	vector<double> y_values;	/*!< y values of points*/

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