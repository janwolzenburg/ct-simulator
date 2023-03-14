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
	*((bool*) loadModel) = true;

}


/*!
 * @brief Main function
 * @param  
 * @return 
*/
int main( int argc, char** argv ){

	programState& currentState = PROGRAM_STATE();

	bool loadModel = PROGRAM_STATE().ModelLoaded();

	// No automatic hierarchies
	Fl_Group::current( NULL );

	window mainWindow{ 1280, 720 };


	/* ------------------------- Model group ------------------------- */
	
	Fl_Group& modelGroup = mainWindow.add<Fl_Group>( relPosition{ 0.02, 0.02, .3, 0.96 } );
	Fl_Box& modelBox = mainWindow.add<Fl_Box>( relPosition{0., 0., 1., 1. }, modelGroup );
	modelBox.box( FL_BORDER_BOX );


	Fl_Group& loadButtonGroup = mainWindow.add<Fl_Group>( relPosition_Hor{ 0.05, 1., 0.05 }, modelGroup );

	Fl_Button& loadButton = mainWindow.add<Fl_Button>( relPosition{ 0., 0., 0.5, 1. }, loadButtonGroup, "Load model" );
	loadButton.labelsize( 20 ); loadButton.callback( modelLoad_cb, &loadModel );

	Fl_Box& loadButtonBox = mainWindow.add<Fl_Box>( relPosition{ 0.5, 0., 0.5, 1. }, loadButtonGroup );
	loadButtonGroup.resizable( loadButtonBox );


	Fl_Group& modelImageGroup = mainWindow.add<Fl_Group>( relPosition{ 0., 0.15, 1., 0.5 }, modelGroup );
	modelGroup.resizable( modelImageGroup );

	Fl_Box& modelImageBox = mainWindow.add<Fl_Box>( relPosition{ 0., 0., 1., 1. }, modelImageGroup, "No model loaded");
	modelImageBox.labelsize( 20 ); modelImageBox.box( FL_BORDER_BOX );

	Fl_Image_Widget& modelImage = mainWindow.add<Fl_Image_Widget>( relPosition{ 0., 0., 1., 1. }, modelImageGroup );
	modelImage.hide();

	////Fl_Group modelViewGroup { 40, 150, 320, 320 };
	////modelGroup.add( modelViewGroup );

	//Fl_Box modelViewBox{ 40, 150, 320, 320, "No model loaded!"};
	//modelViewBox.box( FL_BORDER_BOX );
	//modelViewBox.labelsize( 15 );
	//modelGroup.add( modelViewBox );

	//Fl_Image_Widget slice{ 40, 150, (int) 320, (int) 320 };
	//modelGroup.add_resizable( slice );
	//slice.hide();

	//if( !currentState.ModelLoaded() ) currentState.loadModel();
	
	//currentState.loadModel();
	/*model& currentModel = currentState.Model();

	cartCSys* viewSystem = CSYS_TREE().addCSys("View system");

	//const surfLim viewPlane{	uvec3{ v3{ 1, 0, 0 }, viewSystem },
	//							uvec3{ v3{ 0, 1, 0 }, viewSystem },
	//							pnt3{  currentModel.ModSize(), viewSystem } / 2.,
	//							-currentModel.ModSize().x / 2., currentModel.ModSize().x / 2.,
	//							-currentModel.ModSize().y / 2., currentModel.ModSize().y / 2. };

	//const grid modelSlice = currentModel.getSlice( viewPlane, 1. );

	greyImage sliceImage{ modelSlice };

	vector<char> imageBinary;
	sliceImage.serialize( imageBinary );

	exportSerialized( currentState.stateStorage / "modelSliceImage.image" , imageBinary ); */

	//Fl_Window* window = new Fl_Window( (int) ( 500. * 16. / 9. ), 500 );

	
	


	//window->redraw();
	////window->end();
	mainWindow.getFl().show(argc, argv);




	while( Fl::wait() ){

		if( loadModel ){
			loadModel = false;

			modelImageBox.label( "Loading model..." );

			PROGRAM_STATE().loadModel();

			if( PROGRAM_STATE().ModelLoaded() ){
				modelImage.assignImage( currentState.Slice() );
				modelImage.show();
				modelImageBox.hide();
			}
			else{
				modelImageBox.label("No model loaded!");
			}
		}

	}

	currentState.saveState();

	return 0;
}
