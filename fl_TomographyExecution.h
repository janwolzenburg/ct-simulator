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
 * @brief class for tomography execution
*/
class Fl_TomographyExecution : public Fl_Group{

	public:

	/*!
	 * @brief constructor
	 * @param x x-position
	 * @param y y-position
	 * @param w Width
	 * @param h Height
	 * @param main_window Reference to the main window
	*/
	Fl_TomographyExecution( int x, int y, int w, int h, Fl_MainWindow& main_window );

	/*!
	 * @brief set information update flag
	*/
	void UpdateInformation( ProjectionsProperties projection_properties, DetectorProperties detector_properties, XRayTube tube );

	/*!
	 * @brief export current sinogram
	*/
	void ExportProjections( void );

	/*!
	 * @brief assign projections for processing
	 * @param projections Projections
	*/
	void AssignProjections( const Projections projections );


	private:

	Fl_Box title_;				/*!< Title*/

	Fl_Group tomography_properties_group_;			/*!< Group for parameters*/
	Fl_Box properties_title_;						/*!< Title of parameter group*/
	Fl_Simple_Counter maximum_scatterings_input_;	/*!< maximum amount a Ray can be scattered*/
	Fl_Counter scattering_propability_factor_input_;/*!< scatter propability*/
	Fl_Toggle_Button disable_scattering_button_;	/*!< Toggle scattering*/
	Fl_Counter scattering_absorption_factor_input_;
	Fl_Toggle_Button use_simple_absorption_button_;/*!< simple or "real" absorption*/

	Fl_Counter simulation_quality_input_;	/*!<input for simulation quality*/

	Fl_Multiline_Output information_;				/*!< Information about tomography*/
	
	Fl_Group control_group_;						/*!< Group for control elements*/
	Fl_Input name_input_;							/*!< input for identifiaction name*/
	Fl_Button record_slice_button_;					/*!< start button for radiation*/
	Fl_Button export_projections_button_;			/*!< export button for projections*/
	
	Fl_MainWindow& main_window_;					/*!< reference to main window*/

	PersistingObject<FileChooser> export_projections_file_chooser_;		/*!< File chooser for projections export*/

	PersistingObject<TomographyProperties> tomography_properties_;		/*!< parameter of tomography*/
	Tomography tomography_;												/*!< Instance of the tomography*/
	PersistingObject<Projections> projections_;							/*!< Latest projections*/
	
	vector<std::unique_ptr<Fl_ProcessingWindow>> processing_windows_;	/*!< collection of opened processing windows*/
 
	CallbackFunction<Fl_TomographyExecution> record_slice_callback_;		/*!< callback for slice recording*/
	CallbackFunction<Fl_TomographyExecution> update_properties_callback_;	/*!< callback for propertiy update*/
	CallbackFunction<Fl_TomographyExecution> export_projections_callback_;	/*!< callback for projection export*/


	/*!
	 * @brief do tomography
	*/
	void DoTomography( void );

	/*!
	 * @brief Update tomography properties
	*/
	void UpdateProperties( void );

 };