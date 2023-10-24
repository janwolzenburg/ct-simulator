#pragma once
/*********************************************************************
 * @file   processingWindow.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   May 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include "FL/Fl_Window.H"
#include "FL/Fl_Button.H"
#include "FL/fl_ask.H"

#include "programState.h"

#include "modelView.h"
#include "gantryCreation.h"
#include "tomographyExecution.h"

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

	/*!
	 * @brief Deactivate everything in window
	*/
	void deactivate( void );

	/*!
	 * @brief Activate everything in window
	*/
	void activate( void );

	/*!
	 * @brief Handle event in main window
	 * @param  
	*/
	void handleEvents( void );


	public:

	Fl_Group menu;						/*!<Group at window top for multi purpose items*/
	Fl_Button importSinogramBtn;		/*!<Button for sinogram import*/
	bool importSinogramFlag;			/*!<Flag for sinogram import*/
	Fl_Button resetProgramStateBtn;		/*!<Button to reset program state*/
	bool resetButtonPressed;			/*!<Flag for reset*/

	modelView modView;						/*!<Model view*/
	gantryEdition gantryBuild;				/*!<Gantry creation*/
	tomographyExec tomographyExecution;		/*!<Tomography execution*/


};