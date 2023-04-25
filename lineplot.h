#pragma once
/*********************************************************************
 * @file   linePlot.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   April 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

 #include "plots.h"


class linePlot : public plot{

	public:

	linePlot( const string name_, const string xlabel_, const string ylabel_,
			  const plotLimits limits_, const idx2CR imgSize_, const bool grid_ );

	linePlot( void );

	void assignData( const vector<double> X_, const vector<double> Y_ );

	void assignData( const vectorPair XY );

	void create( void );


	private:

	vector<double> X, Y;

};
