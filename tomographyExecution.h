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

#include "Fl_Bound_Input.h"
#include "programState.h"

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
	tomographyExec( int x, int y, int w, int h );

	/*! 
	 * @brief Destructor
	*/
	~tomographyExec( void );

	/*!
	 * @brief Set information update flag
	*/
	void updateInformation( void ){ informationUpdateFlag = true; };

	/*!
	 * @brief Handle events
	*/
	void handleEvents( void );

	/*!
	 * @brief Export current sinogram
	*/
	void exportSinogram( void );

	void AssignProjections( const Projections projections );

	private:

	Fl_Box title;				/*!<Title*/

	Fl_Group tomoParameterGrp;	/*!<Group for parameters*/
	Fl_Box parameterTitle;		/*!<Title of parameter group*/
	
	Fl_Simple_Counter radiationLoopsIn;		/*!<Maximum amount a Ray can be scattered*/
	Fl_Bound_Input<Fl_Float_Input, double> scatterPropabilityIn;	/*!<Scatter propability*/
	Fl_Toggle_Button scatteringOnOff;	/*!<Toggle scattering*/

	Fl_Multiline_Output information;	/*!<Information about tomography*/
	
	Fl_Group controlGrp;		/*!<Group for control elements*/
	Fl_Button radiationButton;	/*!<Start button for radiation*/
	Fl_Button exportButton;		/*!<Export button for sinogram*/

	
	FileChooser exportChooserInstance;				/*!<File chooser for sinogram export*/
	PersistingObject<FileChooser> storedExportChooser;	/*!<Persisting storage of sinogram export file selection*/

	bool radiateFlag;				/*!<Flag for radiation start*/
	bool exportFlag;				/*!<Flag for sinogram export*/
	bool updateFlag;				/*!<Flag to update tomography parameter*/
	bool informationUpdateFlag;		/*!<Flag for information text update*/

	bool projections_loaded;
	Projections currentProjection;
	vector<processingWindow*> processing_windows_;
 };