#pragma once
/*********************************************************************
 * @file   fl_ProcessingWindow.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   May 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include "FL/Fl_Window.H"
#include "fl_AdjustableGrayscaleImage.h"
#include "fl_Selector.h"
#include "fl_Plot.h"
#include "lineplot.h"

#include "fileChooser.h"
#include "projections.h"
#include "persistingObject.h"
#include "filteredProjections.h"
#include "backprojection.h"
#include "widgets.h"
#include "callbackFunction.h"


 /*********************************************************************
	Definitions
 *********************************************************************/

/*!
 * @brief Window for processing the projections
*/
class Fl_ProcessingWindow : public Fl_Window{

	public:

	/*!
	 * @brief Constructor
	 * @param x x-position
	 * @param y y-position
	 * @param w Width
	 * @param h Height
	 * @param projections Projections the window will process
	*/
	Fl_ProcessingWindow( int w, int h, const char* label, Projections projections );

	
	private:

	Fl_AdjustableGrayscaleImage projections_image_;				/*!< Widget for sinogram display*/
	
	Fl_Group filter_group_;										/*!< Group for filter*/
	Fl_Selector filter_type_selector_;							/*!< Filter selector*/
	Fl_Plot<DotPlot> filter_plot_;								/*!< Filter plot*/

	Fl_AdjustableGrayscaleImage filtered_projections_image_;	/*!< Widget for filtered projections display*/
	Fl_Button export_filteredProjections_button_;			/*!< Export button for projections*/

	Fl_AdjustableGrayscaleImage reconstructed_image_;			/*!< Widget for the reconstruced image*/
	Fl_Button export_image_button_;			/*!< Export button for image*/

	Projections projections_;
	PersistingObject<FilteredProjections> filtered_projections_;					/*!< Current filtered projections*/
	PersistingObject<Backprojection> backprojection_;								/*!< Current image reconstructed from filtered projections*/

	static PersistingObject<FileChooser> export_filteredProjections_file_chooser_;		/*!< File chooser for projections export*/
	static PersistingObject<FileChooser> export_image_chooser_;							/*!< File chooser for projections export*/

	CallbackFunction<Fl_ProcessingWindow> filter_change_callback_;			/*!< Callback function for filter change*/
	
	CallbackFunction<Fl_ProcessingWindow> export_filteredProjections_callback_;		/*!< Callback for projection export*/
	CallbackFunction<Fl_ProcessingWindow> export_image_callback_;		/*!< Callback for image export*/


		
	/*!
	 * @brief Reconstruct the image from projections
	*/
	void ReconstructImage( void );

	void ExportFilteredProjections( void );

	void ExportBackprojections( void );

 };