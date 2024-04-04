#pragma once
/*********************************************************************
 * @file   filteredProjections.h
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
	 * @param projections unfiltered projections 
	 * @param filter_type type of filter to apply
	 * @param progress_window FL_window to track progress
	*/
	FilteredProjections( const Projections& projections, const BackprojectionFilter::TYPE filter_type, Fl_Progress_Window* progress_window = nullptr );

	/*!
	 * @brief constructor from serialized data
	 * @param binary_data vector with serialized data
	 * @param current_byte Start of data for this object
	*/
	FilteredProjections( const vector<char>& binary_data, vector<char>::const_iterator& current_byte ) :
	DataGrid<>{ binary_data, current_byte }{};

	/*!
	 * @brief serialize this object
	 * @param binary_data reference to vector where data will be appended
	 * @return written bytes
	*/
	size_t Serialize( vector<char>& binary_data ) const{ return DataGrid<>::Serialize( binary_data ); };

	/*!
	 * @brief get gridded data
	 * @return gridded data
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
	 * @return resolution of axis
	*/
	GridCoordinates resolution( void ) const{ return DataGrid::resolution(); };

	/*!
	 * @brief return used filter
	 * @return filter used in backprojections
	*/
	BackprojectionFilter filter( void ) const { return filter_; };

	/*!
	 * @brief get value at angle index at given distance
	 * @param angle_index angle index
	 * @param distance distance
	 * @return filtered value
	*/
	double GetValue( const size_t angle_index, const double distance ) const;



	private:

	BackprojectionFilter filter_;		/*!< filter used in backprojection*/

};