/*********************************************************************
 * @file   main.cpp
 * @brief  Main file
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/
#include <iostream>
using std::cerr;  using std::endl;

 #include "test_all.h"


/*!
 * @brief Main function
 * @param  
 * @return 
*/
int main( void ){

	//if( !test_linear_algebra() ) cerr << "Test for linear algebra failed!";

	//if( !test_tube() ) cerr << "Test for xRay tube failed!";

	//if (!test_detector()) cerr << "Test for detector failed!";

	//if( !test_nonUniformDetector() ) cerr << "Test for non-uniform detector failed!";

	//if( !test_gantry() ) cerr << "Test for detector failed!";

	//if( !test_testModel() ) cerr << "Test for test model failed!";

	//if( !test_modelTransmission() ) cerr << "Test for model transmission failed!";

	//if( !test_radonTransform() ) cerr << "Test for radon transformed failed!";

	//if( !test_detector_to_sinogram() ) cerr << "Test for tomography failed!";

	if( !test_Tomography() ) cerr << "Test for tomography failed!";

	//serialisedToImage();

	//if( !test_serialisation() ) cerr << "Test for serialisation failed!";
	
	//if( !test_filter() ) cerr << "Test for filters failed!";

	//if( !test_filteredProjection() ) cerr << "Test for filtered projection failed!";
	
	//if( !test_reconstruction() ) cerr << "Test for filtered projection failed!";

	return 0;
}
