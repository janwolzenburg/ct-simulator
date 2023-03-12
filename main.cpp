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
//#include <FL/Fl_Image.H>

//#include <FL/Fl_Box.H>

#include "programState.h"
#include "cSysTree.h"
#include "surf.h"
#include "image.h"

/*!
 * @brief Main function
 * @param  
 * @return 
*/
int main( int argc, char** argv ){

	programState& currentState = PROGRAM_STATE();

	if( !currentState.ModelLoaded() )currentState.loadModel();


	model& currentModel = currentState.Model();

	cartCSys* viewSystem = CSYS_TREE().addCSys("View system");

	const surfLim viewPlane{	uvec3{ v3{ 1, 0, 0 }, viewSystem },
								uvec3{ v3{ 0, 1, 0 }, viewSystem },
								pnt3{  currentModel.ModSize(), viewSystem } / 2.,
								-currentModel.ModSize().x / 2., currentModel.ModSize().x / 2.,
								-currentModel.ModSize().y / 2., currentModel.ModSize().y / 2. };

	const grid modelSlice = currentModel.getSlice( viewPlane, 1. );

	image sliceImage{ modelSlice };

	//Fl_Window* window = new Fl_Window( (int) ( 500. * 16. / 9. ), 500 );

	//Fl_Bitmap slice{ modelSlice.Size().col, modelSlice.Size().row };

	//window->end();
	//window->show( argc, argv );

	//while( Fl::wait() );

	return 0;
}
