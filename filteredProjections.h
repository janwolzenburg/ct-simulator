#pragma once
/*********************************************************************
 * @file   filteredProjections.h
 * @brief  Classes for backprojections
 *
 * @author Jan Wolzenburg
 * @date   Febuary 2023
 *********************************************************************/


 /*********************************************************************
   Includes
*********************************************************************/

#include "generel.h"
#include "backprojectionFilter.h"
#include "projections.h"
#include "dataGrid.h"
#include "progress.h"



/*********************************************************************
  Definitions
*********************************************************************/

/*!
 * @brief Class for storing the filtered projections
*/
class FilteredProjections : private DataGrid<> {

	public:

	/*!
	 * @brief Default constructor
	*/
	FilteredProjections( void ) : DataGrid{} {};

	/*!
	 * @brief Constructor
	 * @param projections Unfiltered projections 
	 * @param filter_type Type of filter to apply
	 * @param progress_window FL window to track progress
	*/
	FilteredProjections( const Projections projections, const BackprojectionFilter::TYPE filter_type, Fl_Progress_Window* progress_window = nullptr );

	/*!
	 * @brief Get gridded data
	 * @return Gridded data
	*/
	DataGrid<> data_grid( void ) const{ return static_cast<DataGrid<>>( *this ); };

	/*!
	 * @brief Get size of projections
	 * @return Size of projections
	*/
	GridIndex size( void ) const{ return DataGrid::size(); };

	/*!
	 * @brief Get starts of axis
	 * @return Start of axis
	*/
	GridCoordinates start( void ) const{ return DataGrid::start(); };

	/*!
	 * @brief Get resolution of axis
	 * @return GetResolution of axis
	*/
	GridCoordinates resolution( void ) const{ return DataGrid::resolution(); };

	/*!
	 * @brief Return used filter
	 * @return Filter used in backprojections
	*/
	BackprojectionFilter filter( void ) const { return filter_; };

	/*!
	 * @brief Get value at angle index at given distance
	 * @param angle_index Angle index
	 * @param distance Distance
	 * @return Filtered value
	*/
	double GetValue( const size_t angle_index, const double distance ) const;



	private:

	BackprojectionFilter filter_;		/*!<Filter used in backprojection*/

};