/*********************************************************************
 * @file   main.cpp
 * @brief  Main file
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/
#include <iostream>
using std::cerr;  using std::endl; using std::cout;

 //#include "test_all.h"
//#include <FL/Fl.H>
//#include <FL/Fl_Window.H>
//#include <FL/Fl_Box.H>

#include "programState.h"
#include "cSysTree.h"
#include "surf.h"

/*!
 * @brief Main function
 * @param  
 * @return 
*/
int main( int argc, char** argv ){

	programState& currentState = PROGRAM_STATE();

	if( !currentState.ModelLoaded() )currentState.loadModel();


	model& currentModel = currentState.Model();

	const surfLim viewPlane{	uvec3{ v3{ 1, 0, 0 }, GLOBAL_CSYS() }, 
								uvec3{ v3{ 0, 1, 0 }, GLOBAL_CSYS() },
								pnt3{  currentModel.ModSize(), GLOBAL_CSYS() } / 2.,
								-currentModel.ModSize().x / 2., currentModel.ModSize().x / 2.,
								-currentModel.ModSize().y / 2., currentModel.ModSize().y / 2. };

	const grid modelSlice = currentModel.getSlice( viewPlane, 1. );

	currentState.saveState();

	return 0;
}
