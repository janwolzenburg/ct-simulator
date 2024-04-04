#pragma once
/*********************************************************************
 * @file   filteredProjections.h
 * @brief  classes for backprojections
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
#include "fl_ProgressWindow.h"



/*********************************************************************
  Definitions
*********************************************************************/

/*!
 * @brief class for storing the filtered projections
*/
class FilteredProjections : private DataGrid<> {

	public:

	static const string FILE_PREAMBLE; /*!< string to prepend to file when storing as file*/

	/*!
	 * @brief default constructor
	*/
	FilteredProjections( void ) : DataGrid{} {};

	/*!
	 * @brief constructor
	 * @param projections Unfiltered projections 
	 * @param filter_type Type of filter to apply
	 * @param progress_window FL window to track progress
	*/
	FilteredProjections( const Projections& projections, const BackprojectionFilter::TYPE filter_type, Fl_Progress_Window* progress_window = nullptr );
	
	FilteredProjections( const vector<char>& binary_data, vector<char>::const_iterator& current_byte ) :
	DataGrid<>{ binary_data, current_byte }{};
	
	size_t Serialize( vector<char>& binary_data ) const{ return DataGrid<>::Serialize( binary_data ); };

	/*!
	 * @brief get gridded data
	 * @return Gridded data
	*/
	DataGrid<> data_grid( void ) const{ return static_cast<DataGrid<>>( *this ); };

	/*!
	 * @brief get size of projections
	 * @return size of projections
	*/
	GridIndex size( void ) const{ return DataGrid::size(); };

	/*!
	 * @brief get starts of axis
	 * @return start of axis
	*/
	GridCoordinates start( void ) const{ return DataGrid::start(); };

	/*!
	 * @brief get resolution of axis
	 * @return GetResolution of axis
	*/
	GridCoordinates resolution( void ) const{ return DataGrid::resolution(); };

	/*!
	 * @brief return used filter
	 * @return Filter used in backprojections
	*/
	BackprojectionFilter filter( void ) const { return filter_; };

	/*!
	 * @brief get value at angle index at given distance
	 * @param angle_index Angle index
	 * @param distance Distance
	 * @return Filtered value
	*/
	double GetValue( const size_t angle_index, const double distance ) const;



	private:

	BackprojectionFilter filter_;		/*!< Filter used in backprojection*/

};