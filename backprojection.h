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
#include "radonTransformation.h"
#include "dataGrid.h"
#include "progress.h"



/*********************************************************************
  Definitions
*********************************************************************/

/*!
 * @brief Class for storing the filtered projections
*/
class filteredProjections : private DataGrid<> {

	public:

	/*!
	 * @brief Default construnctor
	 * @param  
	*/
	filteredProjections( void ) : DataGrid{} {};

	/*!
	 * @brief Constructor
	 * @param projections UNfiltered projections 
	 * @param filterType Type of filter to apply
	*/
	filteredProjections( const RadonTransformation projections, const BackprojectionFilter::TYPE filterType, Fl_Progress_Window* progress = nullptr );

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
	DataGrid<> getGrid( void ) const{ return ( DataGrid<> ) *this; };

	/*!
	 * @brief Get size_ of projections
	 * @return Size of projections
	*/
	GridIndex Size( void ) const{ return DataGrid::size(); };

	/*!
	 * @brief Get starts of axis
	 * @return Start of axis
	*/
	GridCoordinates Start( void ) const{ return DataGrid::start(); };

	/*!
	 * @brief Get resolution of axis
	 * @return GetResolution of axis
	*/
	GridCoordinates Resolution( void ) const{ return DataGrid::resolution(); };

	/*!
	 * @brief Return used filter
	 * @return Filter used in backprojections
	*/
	BackprojectionFilter Filter( void ) const { return filter; };


	private:

	BackprojectionFilter filter;		/*!<Filter used in backprojection*/

};


/*!
 * @brief Class to store reconstructed image
*/
class reconstrucedImage : private DataGrid<> {

	public:

	/*!
	 * @brief Default constructor
	*/
	reconstrucedImage( void ) : DataGrid{} {};

	/*!
	 * @brief Constructor
	 * @param projections Filtered projections 
	*/
	reconstrucedImage( const filteredProjections projections, Fl_Progress_Window* progress = nullptr );

	/*!
	 * @brief Reconstruct image column wise
	 * @param currentX Current x-index
	 * @param currentXMutex Mutex for x-index
	 * @param image Reference to image
	 * @param imageMutex Mutex for image
	 * @param progress Progress window
	 * @param progressMutex Mutex for progress
	 * @param projections Projections
	*/
	static void reconstructColumn(  size_t& currentX, mutex& currentXMutex, reconstrucedImage& image, 
									mutex& imageMutex, Fl_Progress_Window* progress, mutex& progressMutex, 
									const filteredProjections& projections );

	/*!
	 * @brief Get base  grid
	 * @return Grid
	*/
	 DataGrid<> getGrid( void ) const{ return (DataGrid<>) * this; };

};
