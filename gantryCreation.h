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

#include <FL/Fl_Group.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Tooltip.H>

#include "programState.h"
#include "storedObject.h"
#include "gantry.h"


class gantryEdition : public Fl_Group{

	public:

	gantryEdition( int x, int y, int w, int h );

	inline bool UpdateGantry( void ){ return updateGantry ? !( updateGantry = false ) : false; };

	void handleEvents( void );


	private:

	Fl_Group tubeGrp;
	Fl_Bound_Input<Fl_Float_Input, double> tubeVoltageIn;
	Fl_Bound_Input<Fl_Float_Input, double> tubeCurrentIn;
	selector materialIn;

	Fl_Group radonGrp;
	Fl_Bound_Input<Fl_Int_Input, size_t> colPnts;
	Fl_Bound_Input<Fl_Int_Input, size_t> rowPnts;
	Fl_Bound_Input<Fl_Float_Input, double> distRange;

	Fl_Group detectorGrp;
	Fl_Bound_Input<Fl_Int_Input, int> raysPerPixelIn;
	Fl_Bound_Input<Fl_Float_Input, double> arcRadiusIn;
	Fl_Toggle_Button structureIn;
	Fl_Bound_Input<Fl_Float_Input, double> maxRayAngleIn;


	bool updateGantry;

};