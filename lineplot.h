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
			  const plotLimits limits_, const idx2CR imgSize_, const bool grid_ ) :
		plot( name_, xlabel_, ylabel_, limits_, imgSize_, grid_ ),
		X(), Y(){

	}

	linePlot( void ) :
		plot(),
		X(), Y(){}

	void assignData( const vector<double> X_, const vector<double> Y_ ){

		X = X_;
		Y = Y_;

		if( limits.xFactor != 1. ){
			for( auto& x : X ){
				x *= limits.xFactor;
			}
		}

		if( limits.yFactor != 1. ){
			for( auto& y : Y ){
				y *= limits.yFactor;
			}
		}

		create();
	}


	void assignData( const vectorPair XY ){

		assignData( XY.first, XY.second );

	}

	void create( void ){

		plot::reset();
		plot::plot2D.drawCurve( X, Y );
		plot::drawPlot();

	}


	private:

	vector<double> X, Y;

};
