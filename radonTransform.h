#pragma once
/*********************************************************************
 * @file   radonTransform.h
 * @brief  Classes radon transformation
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/



 /*********************************************************************
  Includes
*********************************************************************/

#include "line.h"


/*********************************************************************
   Definitions
*********************************************************************/


class radonCoords{

	public:
	radonCoords( const double theta_, const double distance_ );

	radonCoords( const cartCSys* const cSys, const line l );

	radonCoords( void );

	double theta = 0;
	double distance = 0;
	
};

class radonPoint{

	radonPoint( const radonCoords coordinates_, const double value_ );
	
	public:
	radonCoords coordinates;
	double value;

};


class grid {

	public:
	grid( const idx2RC size_, const v2RC start_, const v2RC resolution_ );

	bool checkIndex( const idx2RC index ) const;

	double operator()( const idx2RC index ) const;

	double& operator()( const idx2RC index );

	double operator()( const v2RC index ) const;

	double& operator()( const v2RC index );

	private:

	idx2RC size;
	v2RC start;
	v2RC resolution;

	vector<double> columnPoints;
	vector<double> rowPoints;
	vector<vector<double>> data;

};
