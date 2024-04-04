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
	 * @param X vector of x values
	 * @param Y vector of y values
	*/
	void AssignData( const vector<double> X, const vector<double> Y );

	/*!
	 * @brief assign data to plot
	 * @param XY pair of vectors with data
	*/
	void AssignData( const VectorPair XY ){ AssignData( XY.first, XY.second ); };

	/*!
	 * @brief create the plot
	*/
	virtual void CreatePlot( void );


	protected:

	vector<double> X, Y;	/*!< vectors with points*/

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