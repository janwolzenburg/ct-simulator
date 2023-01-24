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
#include "grid.h"


/*********************************************************************
   Definitions
*********************************************************************/


/*!
 * @brief Class for radon coordinates
*/
class radonCoords{

	public:
	/*!
	 * @brief Constructor
	 * @param theta_ Angle
	 * @param distance_ Distance
	*/
	radonCoords( const double theta_, const double distance_ );

	/*!
	 * @brief Constructor
	 * @param cSys Reference coordinate system
	 * @param l Line
	*/
	radonCoords( const cartCSys* const cSys, const line l );

	/*!
	 * @brief Default constructor
	*/
	radonCoords( void );


	public:
	double theta = 0;		/*!<Angle*/
	double distance = 0;	/*!<Distance*/
	
};


/*!
 * @brief Radon point
*/
class radonPoint{

	public:
	/*!
	 * @brief Cosntructor
	 * @param coordinates_ Coordinates 
	 * @param value_ Value
	*/
	radonPoint( const radonCoords coordinates_, const double value_ );
	

	public:
	radonCoords coordinates;	/*!<Coordiantes*/
	double value;				/*!<Value*/

};


/*!
 * @brief Class for a radon transformation
*/
class radonTransformed{

	public:

	/*!
	 * @brief Constructor
	 * @param detectorParameter Parameters to get the grid dimension from
	*/
	radonTransformed( const detectorRadonParameter detectorParameter );

	radonTransformed( const vector<char>& binData, vector<char>::const_iterator& it );

	/*!
	 * @brief Get grid data
	 * @return Grid data
	*/
	grid Data( void ) const;
	
	void assignData( const idx2CR index, const double value );

	/*!
	 * @brief Assign data to grid
	 * @param data Data point
	*/
	void assignData( const radonPoint data );

	size_t serialize( vector<char>& binData ) const;

	private:
	grid dataGrid;		/*!<The data grid*/
};
