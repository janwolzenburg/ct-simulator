#pragma once

/*********************************************************************
 * @file   tomographyExecution.h
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
#include "persistingObject.h"
#include "widgets.h"
#include "mainWindow.fwd.h"
#include "callbackFunction.h"
#include "fl_ProcessingWindow.h"
#include "tomography.h"
#include "fileChooser.h"


/*!
 * @brief Class for tomography execution
*/
class tomographyExec : public Fl_Group{

	public:

	/*!
	 * @brief Constructor
	 * @param x x-position
	 * @param y y-position
	 * @param w Width
	 * @param h Height
	*/
	tomographyExec( int x, int y, int w, int h, mainWindow& main_window );

	/*!
	 * @brief Set information update flag
	*/
	void updateInformation( ProjectionsProperties projection_properties, DetectorProperties detector_properties, XRayTube tube );

	/*!
	 * @brief Export current sinogram
	*/
	void exportSinogram( void );

	void AssignProjections( const Projections projections );

	private:

	mainWindow& main_window_;

	Fl_Box title;				/*!< Title*/

	Fl_Group tomoParameterGrp;	/*!< Group for parameters*/
	Fl_Box parameterTitle;		/*!< Title of parameter group*/
	
	Fl_Simple_Counter radiationLoopsIn;		/*!< Maximum amount a Ray can be scattered*/
	Fl_BoundInput<Fl_Float_Input, double> scatterPropabilityIn;	/*!< Scatter propability*/
	Fl_Toggle_Button scatteringOnOff;	/*!< Toggle scattering*/

	Fl_Multiline_Output information;	/*!< Information about tomography*/
	
	Fl_Group controlGrp;		/*!< Group for control elements*/
	Fl_Button radiationButton;	/*!< Start button for radiation*/
	Fl_Button exportButton;		/*!< Export button for sinogram*/

	
	PersistingObject<FileChooser> exportChooserInstance;	/*!< File chooser for sinogram export*/

		
	Tomography tomographyInstance;				/*!< Instance of the tomography*/
	PersistingObject<TomographyProperties> tomographyParamerters;	/*!< Parameter of tomography*/


	PersistingObject<Projections> currentProjection;
	vector<std::unique_ptr<Fl_ProcessingWindow>> processing_windows_;
 
	
	CallbackFunction<tomographyExec> radiateCB;
	CallbackFunction<tomographyExec> updateCB;
	CallbackFunction<tomographyExec> exportCB;

 
	void radiate( void );

	void updateProperties( void );

 };