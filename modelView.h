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



/*********************************************************************
   Definitions
*********************************************************************/


class modelView : public Fl_Group{

	public: 

	modelView( int x, int y, int w, int h );

	void loadModel( void );

	void handleEvents( void );

	/*!
	 * @brief Check load button press and reset when pressed
	 * @return True when it was pressed
	*/
	inline bool LoadBtnPressed( void ){ return loadBtnPressed ? !( loadBtnPressed = false ) : false; };

	inline bool ModelNeedsUpdate( void ){ return updateModelFlag && PROGRAM_STATE().ModelLoaded() ? !( updateModelFlag = false ) : false; };

	inline bool ResetBtnPressed( void ){ return resetBtnPressed ? !( resetBtnPressed = false ) : false; };

	void sliceModel( void );

	void resetModel( void );

	void UpdateModel( void );

	inline void setUpdateFlag( void ){ updateModelFlag = true; };


	private:

	Fl_Group headGrp;
	Fl_Button loadBtn;

	Fl_Group viewGrp;
	Fl_Multiline_Output modelData;
	Fl_Box viewBox;
	Fl_Image_Widget viewImg;
	
	Fl_Group moveGrp;
	Fl_Counter xRot;
	Fl_Counter yRot;
	Fl_Counter zTrans;
	Fl_Button resetBtn;

	string modelDataString;

	bool loadBtnPressed;

	bool updateModelFlag;
	bool resetBtnPressed;


};