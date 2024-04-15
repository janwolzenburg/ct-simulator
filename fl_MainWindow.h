#pragma once
/*********************************************************************
 * @file   fl_MainWindow.h
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

#include "fl_ModelCreator.h"
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
 * @brief class for main window with model view, gantry creation and tomography execution
*/
class Fl_MainWindow : public Fl_Window{

	public:

	/*!
	 * @brief constructor
	 * @param w width of window
	 * @param h height of window
	 * @param label window title
	*/
	Fl_MainWindow( int w, int h, const char* label );

	/*!
	 * @brief import projections
	*/
	void ImportProjections( void );

	/*!
	 * @brief set reset at exit flat
	*/
	void SetResetAtExit( void );

	/*!
	 * @brief create a  model
	*/
	void CreateModel( void );


	public:

	Fl_Group menu_group_;														/*!< group at window top for multi purpose items*/
	Fl_Button model_creator_button_;								/*!< button for model creation*/
	Fl_Button import_projections_button_;						/*!< button for projections import*/
	Fl_Button reset_program_state_at_exit_button_;	/*!< button to reset program state*/
	Fl_Box author_name_;														/*!< the program author's name*/
	Fl_Box git_repository_;													/*!< URL to the git repo*/

	Fl_ModelView model_view_;												/*!< model view*/
	Fl_GantryCreation gantry_creation_;							/*!< gantry creation*/
	Fl_TomographyExecution tomography_execution_;		/*!< tomography execution*/

	PersistingObject<FileChooser> import_projections_file_chooser_;		/*!< persisting storage of projections import file selection*/

	CallbackFunction<Fl_MainWindow> reset_program_state_callback_;		/*!< callback to reset program state at exit*/
	CallbackFunction<Fl_MainWindow> import_projections_callback_;			/*!< callback to import projections*/
	CallbackFunction<Fl_MainWindow> create_model_callback_;						/*!< callback to create model*/

	vector<std::unique_ptr<Fl_ModelCreator>> creator_windows_;				/*!< storage for creator windows*/
};