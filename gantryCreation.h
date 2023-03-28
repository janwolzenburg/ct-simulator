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
		materialIn{			X( tubeGrp, .1 ),	Y( tubeGrp, .65 ),	W( tubeGrp, .8 ),	H( tubeGrp, .35 ),	"Material" },

		radonGrp{			X( *this, .05 ),	Y( *this, .2 ),		W( *this, .15 ),		H( *this, .15 ) },
		colPnts{			X( radonGrp, .0 ),	Y( radonGrp, 0. ),	W( radonGrp, 1. ),	H( radonGrp, .25 ),	"Angles" },
		rowPnts{			X( radonGrp, .0 ),	Y( radonGrp, .3 ),	W( radonGrp, 1. ),	H( radonGrp, .25 ),	"Pixel" },
		distRange{			X( radonGrp, .0 ),	Y( radonGrp, .6 ),	W( radonGrp, 1. ),	H( radonGrp, .25 ),	"Range" },
	
		detectorGrp{		X( *this, .2 ),			Y( *this, .2 ),			W( *this, .45 ),			H( *this, .15 ) },
		raysPerPixelIn{		X( detectorGrp, .0 ),	Y( detectorGrp, 0. ),	W( detectorGrp, 1. ),	H( detectorGrp, .25 ),	"Rays / Pix" },
		arcRadiusIn{		X( detectorGrp, .0 ),	Y( detectorGrp, .3 ),	W( detectorGrp, 1. ),	H( detectorGrp, .25 ),	"Arc Radius" },
		maxRayAngleIn{		X( detectorGrp, .0 ),	Y( detectorGrp, .6 ),	W( detectorGrp, .4 ),	H( detectorGrp, .25 ),	"Max. angle" },
		structureIn{		X( detectorGrp, .5 ),	Y( detectorGrp, .6 ),	W( detectorGrp, .4 ),	H( detectorGrp, .25 ),	"Anti scat." },


		updateGantry( false )
	{
		Fl_Group::add( tubeGrp );
		tubeGrp.add( tubeVoltageIn ); tubeGrp.add( tubeCurrentIn ); tubeGrp.add( materialIn );

		tubeVoltageIn.align( FL_ALIGN_TOP ); tubeCurrentIn.align( FL_ALIGN_TOP ); materialIn.align( FL_ALIGN_TOP );
	
		tubeVoltageIn.setProperties( 1., 200000., 0 );
		tubeCurrentIn.setProperties( .001, 10., 3 );

		tubeVoltageIn.value( PROGRAM_STATE().TubeParameter().anodeVoltage_V );
		tubeCurrentIn.value( PROGRAM_STATE().TubeParameter().anodeCurrent_A );
		
		tubeVoltageIn.callback( button_cb, &updateGantry );
		tubeCurrentIn.callback( button_cb, &updateGantry );

		vector<string> materialNames;
		for( auto& el : tubeParameter::material ) materialNames.push_back( el.second.first );
	
		materialIn.setElements( materialNames );
		MATERIAL anodeMaterial = PROGRAM_STATE().TubeParameter().anodeMaterial;
		string materialName = tubeParameter::material.at( anodeMaterial ).first;
		materialIn.value( materialName );



		Fl_Group::add( radonGrp ); radonGrp.add( colPnts ); radonGrp.add( rowPnts ); radonGrp.add( distRange );
		colPnts.align( FL_ALIGN_LEFT ); rowPnts.align( FL_ALIGN_LEFT ); distRange.align( FL_ALIGN_LEFT );
	
		colPnts.setProperties( 3, 10000, 0 );
		colPnts.value( PROGRAM_STATE().RadonParameter().numberPoints.col );

		rowPnts.setProperties( 3, 10000, 0 );
		rowPnts.value( PROGRAM_STATE().RadonParameter().numberPoints.row );

		distRange.setProperties( 1., 10000., 0 );
		distRange.value( PROGRAM_STATE().RadonParameter().distanceRange );
	
		colPnts.callback( button_cb, &updateGantry );
		rowPnts.callback( button_cb, &updateGantry );
		distRange.callback( button_cb, &updateGantry );


		Fl_Group::add( detectorGrp ); detectorGrp.add( raysPerPixelIn ); detectorGrp.add( arcRadiusIn ); detectorGrp.add( maxRayAngleIn ); detectorGrp.add( structureIn );
		raysPerPixelIn.align( FL_ALIGN_LEFT ); arcRadiusIn.align( FL_ALIGN_LEFT ); maxRayAngleIn.align( FL_ALIGN_LEFT );


		// Set up input

	};

	~gantryEdition( void ){

		//storedDetectorParameter.saveObject( true );
		//storedRadonParameter.saveObject( true );
		//storedDetectorParameter.saveObject( true );

	};


	inline bool UpdateGantry( void ){ return updateGantry ? !( updateGantry = false ) : false; };


	void handleEvents( void ){

		if( UpdateGantry() ){

			tubeParameter newTubeParameter{ tubeVoltageIn.current, tubeCurrentIn.current, tubeParameter::getEnum( materialIn.value() ) };
			detectorRadonParameter newRadonParameter{ idx2CR{ colPnts.value(), rowPnts.value() }, distRange.value() };



			// Create new parameter instances and gantry
			// Input field values may change
			int a = 1;

		}

	}

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