#pragma once
/*********************************************************************
 * @file   modelView.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   March 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include <string>
using std::string;

#include <vector>
using std::vector;

#include "generel.h"

#include <FL/Fl_Group.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Window.H>
#include "widgets.h"

#include "programState.h"
#include "storedObject.h"
#include "slicePlane.h"
#include "modelViewParameter.h"


/*********************************************************************
   Definitions
*********************************************************************/


/*!
 * @brief Class for the model viewer
*/
class modelView : public Fl_Group{

	public: 

	/*!
	 * @brief Constructor
	 * @param x x-position
	 * @param y y-position
	 * @param w Width
	 * @param h Height
	*/
	modelView( int x, int y, int w, int h );

	/*!
	 * @brief Check if load button was pressed
	 * @return True when it was pressed
	*/
	bool LoadBtnPressed( void ){ return unsetFlag( loadBtnPressed ); };

	/*!
	 * @brief Check if model needs update
	 * @return True when model need update and a model is loaded
	*/
	bool ModelNeedsUpdate( void ){ return unsetFlag( updateModelFlag )&& PROGRAM_STATE().ModelLoaded(); };

	/*!
	 * @brief Check if reset button was pressed
	 * @return True when it was pressed
	*/
	bool ResetBtnPressed( void ){ return unsetFlag( resetBtnPressed ); };

	/*!
	 * @brief Load a model from persisting storage
	*/
	void loadModel( void );
	
	/*!
	 * @brief Set flag for model update
	*/
	void setUpdateFlag( void ){ updateModelFlag = true; };

	/*!
	 * @brief Update the model view
	*/
	void UpdateModel( void );
	
	/*!
	 * @brief Slice the model
	*/
	void sliceModel( void );

	/*!
	 * @brief Reset model's position
	*/
	void resetModel( void );

	/*!
	 * @brief Handle all events
	*/
	void handleEvents( void );


	private:

	Fl_Group headGrp;	/*!<Header group*/
	Fl_Button loadBtn;	/*!<Button to load model*/

	Fl_Group viewGrp;				/*!<Group to view the model*/
	Fl_Multiline_Output modelData;	/*!<Model properties*/
	Fl_Box viewBox;					/*!<Box to show test when no model is loaded*/
	Fl_GridImage_Adjust viewImg;	/*!<Image with adjustable contrast to show model slice*/
	
	Fl_Group moveGrp;	/*!<Group for the model movement*/
	Fl_Counter xRot;	/*!<Input for x-rotation*/
	Fl_Counter yRot;	/*!<Input for y-rotation*/
	Fl_Counter zTrans;	/*!<Input for z-transloation*/
	Fl_Button resetBtn;	/*!<Button to reset model to default*/

	string modelDataString;	/*!<String with model properties*/
	bool loadBtnPressed;	/*!<Flag for model load*/
	bool updateModelFlag;	/*!<Flag for model update*/
	bool resetBtnPressed;	/*!<Flag for model reset*/
	

};