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


class plotLimits{

	public:

	bool autoXRange = true;
	bool autoYRange = true;

	range xRange = range{ 0., 1. };
	range yRange = range{ 0., 1. };

	double xFactor = 1.;
	double yFactor = 1.;

 };


class plot{

	public:

	inline string getImgPath( void ) const { return imagePath.string(); };

	void initialize( const path path_, const string label_, const string xlabel_, const string ylabel_,
					 const plotLimits limits_, const idx2CR imgSize_, const string xFormat_, const string yFormat_, const bool axisEqual_, const bool grid_ );

	void setSize( const idx2CR size );

	idx2CR getSize( void ) const;

	void redraw( void );


	protected:

	plot( const path imgPath_, const string xlabel_, const string ylabel_,
		  const plotLimits limits_, const idx2CR imgSize_, const bool grid_ );

	plot( void );

	void reset( void );

	void drawPlot( void );
	

	protected:
	
	string label;
	path imagePath;
	string xlabel;
	string ylabel;

	bool axisEqual = false;
	bool grid = false;

	string xFormat;
	string yFormat;

	plotLimits limits;

	idx2CR imgSize;
	sciplot::Plot2D plot2D;

 };
