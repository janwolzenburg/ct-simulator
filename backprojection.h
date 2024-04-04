#pragma once
/*********************************************************************
 * @file   backprojection.h
 * @brief  class for the reconstructed image
 *
 * @author Jan Wolzenburg
 * @date   October 2023
 *********************************************************************/


 /*********************************************************************
   Includes
*********************************************************************/

#include "dataGrid.h"
#include "filteredProjections.h"
#include "fl_ProgressWindow.h"



/*********************************************************************
  Definitions
*********************************************************************/

/*!
 * @brief class to store reconstructed image
*/
class Backprojection : private DataGrid<> {

	public:
	
	static const string FILE_PREAMBLE; /*!< string to prepend to file when storing as file*/

	/*!
	 * @brief default constructor
	*/
	Backprojection( void ) : DataGrid{} {};

	/*!
	 * @brief constructor
	 * @param filtered_projections filtered projections 
	 * @param progress_window FL window to track progress
	*/
	Backprojection( const FilteredProjections filtered_projections, Fl_Progress_Window* progress_window = nullptr );

	/*!
	 * @brief constructor from serialized data
	 * @param binary_data reference to vector with binary data
	 * @param current_byte iterator to start of data in vector
	*/
	Backprojection( const vector<char>& binary_data, vector<char>::const_iterator& current_byte ) :
	DataGrid<>{ binary_data, current_byte }{};

	/*!
	 * @brief get gridded data
	 * @return gridded data
	*/
	DataGrid<> getGrid( void ) const{ return static_cast<DataGrid<>>( *this ); };

	/*!
	 * @brief serialize this object
	 * @param binary_data reference to vector where data will be appended
	 * @return written bytes
	*/
	size_t Serialize( vector<char>& binary_data ) const{ return DataGrid<>::Serialize( binary_data ); };


	private:

	/*!
	 * @brief reconstruct image column wise
	 * @param current_x_index Current x-index
	 * @param current_x_index_mutex Mutex for x-index
	 * @param reconstructed_image Reference to image
	 * @param reconstructed_image_mutex Mutex for image
	 * @param progress_window Progress window
	 * @param progress_window_mutex Mutex for progress
	 * @param filtered_projections Projections
	*/
	static void ReconstructImageColumn(	size_t& current_x_index,				mutex& current_x_index_mutex, 
										Backprojection& reconstructed_image,	mutex& reconstructed_image_mutex, 
										Fl_Progress_Window* progress_window,	mutex& progress_window_mutex, 
										const FilteredProjections& filtered_projections );

};
