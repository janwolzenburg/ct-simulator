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

#include "cSysTree.h"
#include "test_model.h"
#include "fileChooser.h"

/*!
 * @brief Main function
 * @param  
 * @return 
*/
int main( int argc, char** argv ){

	fileChooser modelChooser{ "Choose model", "*.model", path{ "./" } };
	path chosenModel = modelChooser.choose();
	
	if( chosenModel.empty() ) return -1;

	vector<char> binaryModel = importSerialized( chosenModel.string().c_str() );
	vector<char>::iterator binaryModelIt = binaryModel.begin();

	if( !model::validModelData( binaryModel, binaryModelIt ) ) return -1;

	model currentModel{ binaryModel, binaryModelIt };

	
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
