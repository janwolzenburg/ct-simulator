#pragma once
/*********************************************************************
 * @file   mainWindow.h
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

#include "programState.h"
#include "modelView.h"
#include "gantryCreation.h"
#include "tomographyExecution.h"
#include "widgets.h"
#include "callbackFunction.h"



 /*********************************************************************
	Definitions
 *********************************************************************/

/*!
 * @brief Class for main window with model view, gantry creation and tomography execution
*/
class mainWindow : public Fl_Window{

	public:

	/*!
	 * @brief Constructor
	 * @param w Width of window
	 * @param h Height of window
	 * @param label Window title
	*/
	mainWindow( int w, int h, const char* label );

	~mainWindow( void );

	void importSinogram( void );

	void resetAtExit( void );


	public:

	Fl_Group menu;						/*!< Group at window top for multi purpose items*/
	Fl_Button importSinogramBtn;		/*!< Button for sinogram import*/
	Fl_Button resetProgramStateBtn;		/*!< Button to reset program state*/


	FileChooser importChooserInstance;				/*!< File chooser for sinogram import*/
	PersistingObject<FileChooser> storedImportChooser;	/*!< Persisting storage of sinogram import file selection*/

	ModelView modView;						/*!< Model view*/
	gantryEdition gantryBuild;				/*!< Gantry creation*/
	tomographyExec tomographyExecution;		/*!< Tomography execution*/

	CallbackFunction<mainWindow> setresetAtExitCB;
	CallbackFunction<mainWindow> importSGCB;

};