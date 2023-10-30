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
	reconstrucedImage( const FilteredProjections projections, Fl_Progress_Window* progress = nullptr );

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
									const FilteredProjections& projections );

	/*!
	 * @brief Get base  grid
	 * @return Grid
	*/
	 DataGrid<> getGrid( void ) const{ return (DataGrid<>) * this; };

};
