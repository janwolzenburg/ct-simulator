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

#include "programState.h"
#include "storedObject.h"
#include "gantry.h"

class gantryEdition : public Fl_Group{

	public:

	gantryEdition( int x, int y, int w, int h ) :
		Fl_Group{ x, y, w, h },

		xRayTubeParameter{},
		storedXRayTubeParameter{ programState::getPath( "storedTubeParameter.tubeParameter" ), xRayTubeParameter },

		radonParameter{},
		storedRadonParameter{ programState::getPath( "storedRadonParameter.radonParameter" ), radonParameter },

		detectorParameter{},
		storedDetectorParameter{ programState::getPath( "storedDetectorParameter.radonParameter" ), detectorParameter }

	{};

	~gantryEdition( void ){

		storedDetectorParameter.saveObject( true );
		storedRadonParameter.saveObject( true );
		storedDetectorParameter.saveObject( true );

	}

	private:

	// Voltage, Current, material
	tubeParameter xRayTubeParameter;
	storedObject<tubeParameter> storedXRayTubeParameter;

	// Numberpoints (2D), Distance range
	detectorRadonParameter radonParameter;
	storedObject<detectorRadonParameter> storedRadonParameter;

	// Pixel arc radius, pixel size col direction, Structere, structure max angle
	detectorIndipendentParameter detectorParameter;
	storedObject<detectorIndipendentParameter> storedDetectorParameter;

};