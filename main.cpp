/*********************************************************************
 * @file   main.cpp
 * @brief  Main file
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/

#include <iostream>
using std::cerr;  using std::endl; using std::cout;

#include "programState.h"
#include "gui.h"
//#include "test_model.h"
#include "cSysTree.h"



void modelLoad_cb( Fl_Widget*, void* loadModel ){
	PROGRAM_STATE().loadModel();

	if( PROGRAM_STATE().ModelLoaded() )
		*((bool*) loadModel) = true;

}


/*!
 * @brief Main function
 * @param  
 * @return 
*/
int main( int argc, char** argv ){

	programState& currentState = PROGRAM_STATE();

	// No automatic hierarchies
	Fl_Group::current( NULL );

	// Create resizeable main window
	Fl_Window mainWindow{ 800, 600 };
	mainWindow.resizable( mainWindow );


	// Group for model handling
	Fl_Group modelGroup{ 0, 0, 400, 600 };
	mainWindow.add_resizable( modelGroup );
	
	Fl_Box modelGroupBox{ 0, 0, 400, 600 };
	modelGroupBox.box( FL_BORDER_BOX );
	modelGroup.add_resizable( modelGroupBox );

	bool loadModel = PROGRAM_STATE().ModelLoaded();
	Fl_Button modelLoadButton{ 60, 50, 280, 50, "Load model"};
	modelLoadButton.callback( modelLoad_cb, &loadModel );
	modelGroup.add_resizable( modelLoadButton );

	Fl_Group modelViewGroup { 40, 150, 320, 320 };
	modelGroup.add_resizable( modelViewGroup );

	Fl_Box modelViewBox{ 40, 150, 320, 320, "No model loaded!"};
	modelViewBox.box( FL_BORDER_BOX );
	modelViewBox.labelsize( 15 );
	modelViewGroup.add_resizable( modelViewBox );

	Fl_Image_Widget slice{ 40, 150, (int) 320, (int) 320 };
	modelViewGroup.add_resizable( slice );
	slice.hide();

	//if( !currentState.ModelLoaded() ) currentState.loadModel();
	
	//currentState.loadModel();
	/*model& currentModel = currentState.Model();

	cartCSys* viewSystem = CSYS_TREE().addCSys("View system");

	const surfLim viewPlane{	uvec3{ v3{ 1, 0, 0 }, viewSystem },
								uvec3{ v3{ 0, 1, 0 }, viewSystem },
								pnt3{  currentModel.ModSize(), viewSystem } / 2.,
								-currentModel.ModSize().x / 2., currentModel.ModSize().x / 2.,
								-currentModel.ModSize().y / 2., currentModel.ModSize().y / 2. };

	const grid modelSlice = currentModel.getSlice( viewPlane, 1. );

	greyImage sliceImage{ modelSlice };

	vector<char> imageBinary;
	sliceImage.serialize( imageBinary );

	exportSerialized( currentState.stateStorage / "modelSliceImage.image" , imageBinary ); */

	//Fl_Window* window = new Fl_Window( (int) ( 500. * 16. / 9. ), 500 );

	
	


	//window->redraw();
	////window->end();
	mainWindow.show( argc, argv );




	while( Fl::wait() ){

		if( loadModel ){
			loadModel = false;
			slice.assignImage( currentState.Slice() );
			slice.show();
			modelViewBox.hide();
		}

	}

	currentState.saveState();

	return 0;
}
