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
		tubeGrp{			X( *this, .05 ),	Y( *this, .05 ),	W( *this, .5 ),		H( *this, .1 ) },
		tubeVoltageIn{		X( tubeGrp, .0 ),	Y( tubeGrp, 0. ),	W( tubeGrp, .4 ),	H( tubeGrp, .35 ),	"Voltage V" },
		tubeCurrentIn{		X( tubeGrp, .6 ),	Y( tubeGrp, 0. ),	W( tubeGrp, .4 ),	H( tubeGrp, .35 ),	"Current A" },
		materialIn{			X( tubeGrp, .1 ),	Y( tubeGrp, .65 ),	W( tubeGrp, .8 ),	H( tubeGrp, .35 ),	"Material" }

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
		
		
		vector<string> materialNames;
		for( auto& el : tubeParameter::material ){
			materialNames.push_back( el.second.first );
		}

		materialIn.setElements( materialNames );

		MATERIAL anodeMaterial = PROGRAM_STATE().TubeParameter().anodeMaterial;
		string materialName = tubeParameter::material.at( anodeMaterial ).first;
		
		materialIn.value( materialName );

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
	selector materialIn;



};