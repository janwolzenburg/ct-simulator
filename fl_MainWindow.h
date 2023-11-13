#pragma once
/*********************************************************************
 * @file   fl_MainWindow.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   May 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include "FL/Fl_Window.H"
#include "FL/Fl_Group.H"
#include "FL/Fl_Button.H"

#include "fl_ModelView.h"
#include "fl_GantryCreation.h"
#include "fl_TomographyExecution.h"

#include "programState.h"
#include "widgets.h"
#include "callbackFunction.h"



 /*********************************************************************
	Definitions
 *********************************************************************/

/*!
 * @brief Class for main window with model view, gantry creation and tomography execution
*/
class Fl_MainWindow : public Fl_Window{

	public:

	/*!
	 * @brief Constructor
	 * @param w Width of window
	 * @param h Height of window
	 * @param label Window title
	*/
	Fl_MainWindow( int w, int h, const char* label );

	/*!
	 * @brief Import projections
	*/
	void ImportProjections( void );

	/*!
	 * @brief Set reset at exit flat
	*/
	void SetResetAtExit( void );


	public:

	Fl_Group menu_group_;							/*!< Group at window top for multi purpose items*/
	Fl_Button import_projections_button_;			/*!< Button for projections import*/
	Fl_Button reset_program_state_at_exit_button_;	/*!< Button to reset program state*/

	Fl_ModelView model_view_;						/*!< Model view*/
	Fl_GantryCreation gantry_creation_;				/*!< Gantry creation*/
	Fl_TomographyExecution tomography_execution_;	/*!< Tomography execution*/

	PersistingObject<FileChooser> import_projections_file_chooser_;	/*!< Persisting storage of projections import file selection*/

	CallbackFunction<Fl_MainWindow> reset_program_state_callback_;	/*!< Callback to reset program state at exit*/
	CallbackFunction<Fl_MainWindow> import_projections_callback_;	/*!< Callback to import projections*/

};