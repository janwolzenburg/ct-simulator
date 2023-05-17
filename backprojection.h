#pragma once
/*********************************************************************
 * @file   backprojection.h
 * @brief  Classes for backprojections
 *
 * @author Jan Wolzenburg
 * @date   Febuary 2023
 *********************************************************************/


 /*********************************************************************
   Includes
*********************************************************************/

#include "generel.h"
#include "filter.h"
#include "radonTransform.h"
#include "grid.h"



/*********************************************************************
  Definitions
*********************************************************************/

/*!
 * @brief Class for storing the filtered projections
*/
class filteredProjections : private grid {

	public:

	filteredProjections( void );

	/*!
	 * @brief Constructor
	 * @param projections UNfiltered projections 
	 * @param filterType Type of filter to apply
	*/
	filteredProjections( const radonTransformed projections, const discreteFilter::TYPE filterType );
	
	/*!
	 * @brief Get value at angle index at given distance
	 * @param angleIdx Angle index
	 * @param distance Distance
	 * @return Filtered value
	*/
	double getValue( const size_t angleIdx, const double distance ) const;

	/*!
	 * @brief Get base  grid
	 * @return Grid
	*/
	inline grid getGrid( void ) const { return (grid) *this; };

	/*!
	 * @brief Get size of projections
	 * @return Size of projections
	*/
	inline idx2CR Size( void ) const{ return grid::Size(); };

	/*!
	 * @brief Get starts of axis
	 * @return Start of axis
	*/
	inline v2CR Start( void ) const{ return grid::Start(); };

	/*!
	 * @brief Get resolution of axis
	 * @return Resolution of axis
	*/
	inline v2CR Resolution( void ) const{ return grid::Resolution(); };

	inline discreteFilter Filter( void ) const { return filter; };

	private:

	discreteFilter filter;

};


/*!
 * @brief Class to store reconstructed image
*/
class reconstrucedImage : private grid {

	public:

	reconstrucedImage( void );

	/*!
	 * @brief Constructor
	 * @param projections Filtered projections 
	*/
	reconstrucedImage( const filteredProjections projections );

	/*!
	 * @brief Get base  grid
	 * @return Grid
	*/
	inline grid getGrid( void ) const{ return (grid) *this; };



};
