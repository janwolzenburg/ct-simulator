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


class lineplot : public plot{

	public:

	lineplot( const string name_, const string xlabel_, const string ylabel_,
			  const plotLimits limits_, const idx2CR imgSize_, const bool grid_ );

	lineplot( void );

	void assignData( const vector<double> X_, const vector<double> Y_ );

	void assignData( const vectorPair XY );

	virtual void create( void );


	protected:

	vector<double> X, Y;

};


class dotplot : public lineplot{

	using lineplot::lineplot;

	public:
	void create( void ) override;

};