/*********************************************************************
 * @file   main.cpp
 * @brief  Main file
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/
#include <iostream>
using std::cerr;  using std::endl;

 //#include "test_all.h"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
/*!
 * @brief Main function
 * @param  
 * @return 
*/
int main( int argc, char** argv ){

	Fl_Window* window = new Fl_Window( 340, 180 );
	Fl_Box* box = new Fl_Box( 20, 40, 300, 100, "Hello, World!" );
	box->box( FL_UP_BOX );
	box->labelfont( FL_BOLD + FL_ITALIC );
	box->labelsize( 36 );
	box->labeltype( FL_SHADOW_LABEL );
	window->end();
	window->show( argc, argv );
	return Fl::run();
	

	//if( !test_linear_algebra() ) cerr << "Test for linear algebra failed!";

	//if( !test_tube() ) cerr << "Test for xRay tube failed!";

	//if (!test_detector()) cerr << "Test for detector failed!";

	//if( !test_modifiedDetector() ) cerr << "Test for non-uniform detector failed!";

	//if( !test_gantry() ) cerr << "Test for detector failed!";

	//if( !test_testModel() ) cerr << "Test for test model failed!";

	//if( !test_modelTransmission() ) cerr << "Test for model transmission failed!";

	//if( !test_radonTransform() ) cerr << "Test for radon transformed failed!";

	//if( !test_detector_to_sinogram() ) cerr << "Test for tomography failed!";

	//if( !test_Tomography() ) cerr << "Test for tomography failed!";

	//save_testModel();

	//serialisedToImage();

	//if( !test_serialisation() ) cerr << "Test for serialisation failed!";
	
	//if( !test_filter() ) cerr << "Test for filters failed!";

	//if( !test_filteredProjection() ) cerr << "Test for filtered projection failed!";
	
	//if( !test_reconstruction() ) cerr << "Test for filtered projection failed!";

	//test_scattered_angle_propabilities();


	 //test_ray_scattering(  );

	return 0;
}
