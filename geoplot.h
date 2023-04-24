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


class geoPlot : public plot{

	public:

	geoPlot( const string name_, const string xlabel_, const string ylabel_,
			 const plotLimits limits_, const idx2CR imgSize_, const bool grid_ ) :
		plot( name_, xlabel_, ylabel_, limits_, imgSize_, grid_ ){

	}

	geoPlot( void ) :
		plot(){}


	void addLine( const v2 start, const v2 end ){

		lines.emplace_back( v2( limits.xFactor * start.x, limits.yFactor * start.y ), v2( limits.xFactor * end.x, limits.yFactor * end.y ) );

	}

	void addPoint( const v2 point ){

		points.emplace_back( v2( limits.xFactor * point.x, limits.yFactor * point.y ) );

	}

	void resetObjects( void ){

		lines.clear();
		points.clear();

	}

	void create( void ){

		plot::reset();

		for( const auto& line : lines ){

			vector<double> X{ line.first.x, line.second.x };
			vector<double> Y{ line.first.y, line.second.y };

			plot::plot2D.drawCurve( X, Y );
		}

		for( const auto& point : points ){

			vector<double> X{ point.x };
			vector<double> Y{ point.y };

			plot::plot2D.drawPoints( X, Y );
		}

		plot::drawPlot();

	}


	private:

	vector<std::pair<v2, v2>> lines;
	vector<v2> points;

};
