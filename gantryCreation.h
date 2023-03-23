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

#include "programState.h"
#include "storedObject.h"
#include "gantry.h"

class gantryEdition : public Fl_Group{

	public:

	gantryEdition( int x, int y, int w, int h ) :
		Fl_Group{ x, y, w, h },
		tubeGrp{			X( *this, 0. ),		Y( *this, 0. ),		W( *this, 1. ),		H( *this, .15 ) },
		tubeVoltageIn{		X( tubeGrp, .05 ),	Y( tubeGrp, 0. ),	W( tubeGrp, .3 ),	H( tubeGrp, .2 ),	"Voltage" },
		tubeCurrentIn{		X( tubeGrp, .3 ),	Y( tubeGrp, 0. ),	W( tubeGrp, .3 ),	H( tubeGrp, .2 ),	"Current" },
		materialIn{			X( tubeGrp, .75 ),	Y( tubeGrp, 0. ),	W( tubeGrp, .2 ),	H( tubeGrp, .2 ),	"Material" }

	{
		Fl_Group::add( tubeGrp );

		tubeGrp.add( tubeVoltageIn );
		tubeGrp.add( tubeCurrentIn );
		tubeGrp.add( materialIn );

		tubeVoltageIn.align( FL_ALIGN_TOP );
		tubeCurrentIn.align( FL_ALIGN_TOP );
		materialIn.align( FL_ALIGN_TOP );
	
		tubeVoltageIn.value( toString( PROGRAM_STATE().TubeParameter().anodeVoltage_V, 1 ).c_str() );
		tubeCurrentIn.value( toString( PROGRAM_STATE().TubeParameter().anodeCurrent_A, 3 ).c_str() );
		materialIn.value( toString( PROGRAM_STATE().TubeParameter().anodeAtomicNumber, 0 ).c_str() );

	};

	~gantryEdition( void ){

		//storedDetectorParameter.saveObject( true );
		//storedRadonParameter.saveObject( true );
		//storedDetectorParameter.saveObject( true );

	};

	private:

	Fl_Group tubeGrp;
	Fl_Float_Input tubeVoltageIn;
	Fl_Float_Input tubeCurrentIn;
	Fl_Int_Input materialIn;



};