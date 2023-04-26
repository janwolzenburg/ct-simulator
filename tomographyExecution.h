#pragma once
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


#include <FL/Fl_Group.H>
#include <FL/Fl_Float_Input.H>


#include "widgets.h"

class tomographyExec : Fl_Group{


	private:

	// Information

	Fl_Group tomoParameterGrp;
	Fl_Bound_Input<Fl_Float_Input, double> exposureTimeIn;

	// Control
	// Filter
	// Simulation

	// Projections

	// Filtered projection

	// Reconstructed image


 };