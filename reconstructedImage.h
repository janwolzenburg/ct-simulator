#pragma once
/*********************************************************************
 * @file   reconstructedImage.h
 * @brief  Class for the reconstructed image
 *
 * @author Jan Wolzenburg
 * @date   October 2023
 *********************************************************************/


 /*********************************************************************
   Includes
*********************************************************************/

#include "dataGrid.h"
#include "filteredProjections.h"
#include "progress.h"



/*********************************************************************
  Definitions
*********************************************************************/

/*!
 * @brief Class to store reconstructed image
*/
class ReconstrucedImage : private DataGrid<> {

	public:

	/*!
	 * @brief Default constructor
	*/
	ReconstrucedImage( void ) : DataGrid{} {};

	/*!
	 * @brief Constructor
	 * @param filtered_projections Filtered projections 
	 * @param progress_window FL window to track progress
	*/
	ReconstrucedImage( const FilteredProjections filtered_projections, Fl_Progress_Window* progress_window = nullptr );

	/*!
	 * @brief Get gridded data
	 * @return Gridded data
	*/
	 DataGrid<> getGrid( void ) const{ return static_cast<DataGrid<>>( *this ); };


	 private:

	/*!
	 * @brief Reconstruct image column wise
	 * @param current_x_index Current x-index
	 * @param current_x_index_mutex Mutex for x-index
	 * @param reconstructed_image Reference to image
	 * @param reconstructed_image_mutex Mutex for image
	 * @param progress_window Progress window
	 * @param progress_window_mutex Mutex for progress
	 * @param filtered_projections Projections
	*/
	static void ReconstructImageColumn(	size_t& current_x_index,				mutex& current_x_index_mutex, 
										ReconstrucedImage& reconstructed_image,	mutex& reconstructed_image_mutex, 
										Fl_Progress_Window* progress_window,	mutex& progress_window_mutex, 
										const FilteredProjections filtered_projections );

};
