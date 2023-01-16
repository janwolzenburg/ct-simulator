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
#include "detector.h"


/*********************************************************************
   Definitions
*********************************************************************/

// TODO: split into files and add comments
// TODO: create radon transformed from detection result

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
	grid( const idx2CR size_, const v2CR start_, const v2CR resolution_, double defaultValue = 0 );

	grid( const range columnRange, const range rowRange, const v2CR resolution_, double defaultValue = 0);

	idx2CR Size( void ) const;
	v2CR Start( void ) const;
	v2CR Resolution( void ) const;

	bool checkIndex( const idx2CR index ) const;

	double operator()( const idx2CR index ) const;

	double& operator()( const idx2CR index );

	idx2CR getIndex( const v2CR coordinate ) const;

	v2CR getCoordinates( const idx2CR index ) const;

	double operator()( const v2CR index ) const;

	double& operator()( const v2CR index );


	private:
	void fillVectors( const double defaultValue );

	private:

	idx2CR size;
	v2CR start;
	v2CR resolution;

	vector<double> columnPoints;
	vector<double> rowPoints;
	vector<vector<double>> data;

};


class radonTransformed{

	public:
	radonTransformed( const detectorParameterSignal detectorParameter );

	grid Data( void ) const;

	void assignData( const radonPoint data );

	private:
	grid dataGrid;
};
