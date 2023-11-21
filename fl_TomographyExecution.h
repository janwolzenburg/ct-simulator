#pragma once

/*********************************************************************
 * @file   fl_TomographyExecution.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   April 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include <FL/Fl_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Simple_Counter.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Toggle_Button.H>
#include <memory>

#include "fl_BoundInput.h"
#include "fl_ProcessingWindow.h"
#include "fl_MainWindow.fwd.h"

#include "persistingObject.h"
#include "widgets.h"
#include "callbackFunction.h"
#include "tomography.h"
#include "fileChooser.h"


/*!
 * @brief Class for tomography execution
*/
class Fl_TomographyExecution : public Fl_Group{

	public:

	/*!
	 * @brief Constructor
	 * @param x x-position
	 * @param y y-position
	 * @param w Width
	 * @param h Height
	 * @param main_window Reference to the main window
	*/
	Fl_TomographyExecution( int x, int y, int w, int h, Fl_MainWindow& main_window );

	/*!
	 * @brief Set information update flag
	*/
	void UpdateInformation( ProjectionsProperties projection_properties, DetectorProperties detector_properties, XRayTube tube );

	/*!
	 * @brief Export current sinogram
	*/
	void ExportProjections( void );

	/*!
	 * @brief Assign projections for processing
	 * @param projections Projections
	*/
	void AssignProjections( const Projections projections );


	private:

	Fl_Box title_;				/*!< Title*/

	Fl_Group tomography_properties_group_;			/*!< Group for parameters*/
	Fl_Box properties_title_;						/*!< Title of parameter group*/
	Fl_Simple_Counter maximum_scatterings_input_;	/*!< Maximum amount a Ray can be scattered*/
	Fl_Counter scattering_propability_factor_input_;/*!< Scatter propability*/
	Fl_Toggle_Button disable_scattering_button_;	/*!< Toggle scattering*/
	Fl_Toggle_Button use_simple_attenuation_button_;/*!< Simple or "real" attenuation*/

	Fl_Multiline_Output information_;				/*!< Information about tomography*/
	
	Fl_Group control_group_;						/*!< Group for control elements*/
	Fl_Button record_slice_button_;					/*!< Start button for radiation*/
	Fl_Button export_projections_button_;			/*!< Export button for projections*/
	
	Fl_MainWindow& main_window_;					/*!< Reference to main window*/

	PersistingObject<FileChooser> export_projections_file_chooser_;		/*!< File chooser for projections export*/

	PersistingObject<TomographyProperties> tomography_properties_;		/*!< Parameter of tomography*/
	Tomography tomography_;												/*!< Instance of the tomography*/
	PersistingObject<Projections> projections_;							/*!< Latest projections*/
	
	vector<std::unique_ptr<Fl_ProcessingWindow>> processing_windows_;	/*!< Collection of opened processing windows*/
 
	CallbackFunction<Fl_TomographyExecution> record_slice_callback_;		/*!< Callback for slice recording*/
	CallbackFunction<Fl_TomographyExecution> update_properties_callback_;	/*!< Callback for propertiy update*/
	CallbackFunction<Fl_TomographyExecution> export_projections_callback_;	/*!< Callback for projection export*/


	/*!
	 * @brief Do tomography
	*/
	void DoTomography( void );

	/*!
	 * @brief Update tomography properties
	*/
	void UpdateProperties( void );

 };