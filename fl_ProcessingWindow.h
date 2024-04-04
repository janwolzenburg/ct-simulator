#pragma once
/*********************************************************************
 * @file   fl_ProcessingWindow.h
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
#include "FL/Fl_Toggle_Button.H"
#include "fl_Plot.h"
#include "lineplot.h"
#include "fl_BoundInput.h"
#include <FL/Fl_Float_Input.H>

#include "fileChooser.h"
#include "projections.h"
#include "persistingObject.h"
#include "filteredProjections.h"
#include "backprojection.h"
#include "widgets.h"
#include "callbackFunction.h"
#include "tomography.h"


 /*********************************************************************
	Definitions
 *********************************************************************/

/*!
 * @brief window for processing the projections
*/
class Fl_ProcessingWindow : public Fl_Window{

	public:

	/*!
	 * @brief constructor
	 * @param x x-position
	 * @param y y-position
	 * @param w width
	 * @param h height
	 * @param projections projections the window will process
	*/
	Fl_ProcessingWindow( int w, int h, const char* label, const Projections& projections );

	
	private:

	Fl_AdjustableGrayscaleImage projections_image_;			/*!< widget for sinogram display*/
	
	Fl_Button limit_values_button_;	/*!< button to activate projection value's limits*/
	Fl_Button recalculate_button_;	/*!< button to recalculate backprojection*/

	Fl_Multiline_Output information_output_;	/*!< information about the tomography*/

	Fl_Group filter_group_;										/*!< group for filter*/
	Fl_Selector filter_type_selector_;				/*!< filter selector*/
	Fl_Plot<DotPlot> filter_plot_;						/*!< filter plot*/

	Fl_Group processing_properties_group_;									/*!< group for processing properties*/
	Fl_Toggle_Button hu_mu_selection_button_;								/*!< button to switch between HU and coefficients*/
	Fl_BoundInput<Fl_Float_Input, double> mu_water_input_;	/*!< input for read coefficient value for water*/

	Fl_AdjustableGrayscaleImage filtered_projections_image_;	/*!< widget for filtered projections display*/
	Fl_Button export_filteredProjections_button_;							/*!< export button for projections*/

	Fl_AdjustableGrayscaleImage reconstructed_image_;			/*!< widget for the reconstruced image*/
	Fl_Button export_image_button_;												/*!< export button for image*/

	Projections raw_projections_;																	/*!< projection data and properties*/
	Projections limited_projections_;															/*!< projections with upper value limit*/
	PersistingObject<FilteredProjections> filtered_projections_;	/*!< current filtered projections*/
	PersistingObject<Backprojection> backprojection_;							/*!< current image reconstructed from filtered projections*/

	static PersistingObject<FileChooser> export_filteredProjections_file_chooser_;		/*!< file chooser for projections export*/
	static PersistingObject<FileChooser> export_image_chooser_;												/*!< file chooser for projections export*/

	CallbackFunction<Fl_ProcessingWindow> recalculate_callback_;							/*!< callback to recalculate*/
	CallbackFunction<Fl_ProcessingWindow> filter_change_callback_;						/*!< callback function for filter change*/
	CallbackFunction<Fl_ProcessingWindow> hu_mu_selection_changed_callback_;	/*!< callback when selection changed*/
	CallbackFunction<Fl_ProcessingWindow> hu_mu_input_changed_callback_;			/*!< callback for when mu input changed*/

	CallbackFunction<Fl_ProcessingWindow> export_filteredProjections_callback_;		/*!< callback for projection export*/
	CallbackFunction<Fl_ProcessingWindow> export_image_callback_;									/*!< callback for image export*/


		
	/*!
	 * @brief reconstruct the image from projections
	*/
	void FilterAndReconstructImage( void );

	/*!
	 * @brief update backprojected image
	*/
	void UpdateImage( void );

	/*!
	 * @brief handle change of Houndsfield Unit calibration value
	*/
	void HandleMUChange( void );

	/*!
	 * @brief export the filtered projections
	*/
	void ExportFilteredProjections( void );

	/*!
	 * @brief export the backprojection image 
	*/
	void ExportBackprojections( void );

 };