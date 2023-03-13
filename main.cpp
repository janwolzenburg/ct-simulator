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
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Draw.H>
#include <FL/Fl_Box.H>

#include "programState.h"
#include "cSysTree.h"
#include "surf.h"
#include "image.h"

class Fl_Image_Widget : public Fl_Widget{

	public:

	Fl_Image_Widget( void );

	

};

/*!
 * @brief Main function
 * @param  
 * @return 
*/
int main( int argc, char** argv ){

	programState& currentState = PROGRAM_STATE();

	if( !currentState.ModelLoaded() ) currentState.loadModel();


	//model& currentModel = currentState.Model();

	//cartCSys* viewSystem = CSYS_TREE().addCSys("View system");

	//const surfLim viewPlane{	uvec3{ v3{ 1, 0, 0 }, viewSystem },
	//							uvec3{ v3{ 0, 1, 0 }, viewSystem },
	//							pnt3{  currentModel.ModSize(), viewSystem } / 2.,
	//							-currentModel.ModSize().x / 2., currentModel.ModSize().x / 2.,
	//							-currentModel.ModSize().y / 2., currentModel.ModSize().y / 2. };

	//const grid modelSlice = currentModel.getSlice( viewPlane, 1. );

	//image sliceImage{ modelSlice };

	//vector<char> imageBinary;
	//sliceImage.serialize( imageBinary );

	//exportSerialized( "modelSliceImage.image", imageBinary ); 

	Fl_Window* window = new Fl_Window( (int) ( 500. * 16. / 9. ), 500 );

	image& sliceImage = currentState.ModelSlice();
	vector<unsigned char> imageBufferMono = sliceImage.getImage();
	
	Fl_Box* sliceImageBox = new Fl_Box( 0, 0, (int) sliceImage.Width(), (int) sliceImage.Heigth() );
	window->add( sliceImageBox );
	//
	Fl_RGB_Image* slice = new Fl_RGB_Image( imageBufferMono.data(), (int) sliceImage.Width(), (int) sliceImage.Heigth(), 1 );

	////fl_draw_image_mono( imageBufferMono.data(), 0, 0, (int) modelSlice.Size().col, (int) modelSlice.Size().row,);
	//slice->draw(0, 0, (int) modelSlice.Size().col, (int) modelSlice.Size().row );

	sliceImageBox->image( slice );


	window->end();
	window->show( argc, argv );

	return Fl::run();
}
