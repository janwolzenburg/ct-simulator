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


class geoplot : public plot{

	public:

	geoplot( const string name_, const string xlabel_, const string ylabel_,
			 const plotLimits limits_, const idx2CR imgSize_, const bool grid_ );

	geoplot( void );

	void addLine( const v2 start, const v2 end );

	void addPoint( const v2 point );

	void resetObjects( void );

	void create( void );


	private:

	vector<std::pair<v2, v2>> lines;
	vector<v2> points;

};