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
#include "test_model.h"
#include "cSysTree.h"



void button_cb( Fl_Widget*, void* flag ){
	*((bool*) flag ) = true;
}



/*!
 * @brief Main function
 * @param  
 * @return 
*/
int main( int argc, char** argv ){

	//model testModel = getTestModel( GLOBAL_CSYS(), 6 );

	//vector<char> modelBin;
	//testModel.serialize( modelBin );
	//exportSerialized(path{ "./testModel_6x.model" }, modelBin );

	//return 0;

	programState& currentState = PROGRAM_STATE();

	bool loadModel = !PROGRAM_STATE().ModelLoaded();

	bool rotateModelX = false; double currentAngleX = 0;
	bool rotateModelY = false; double currentAngleY = 0;

	// No automatic hierarchies
	Fl_Group::current( NULL );

	window mainWindow{ 1280, 720 };


	/* ------------------------- Model group ------------------------- */
	
	// Model group and box
	Fl_Group& modelGroup = mainWindow.add<Fl_Group>( relPosition{ 0.02, 0.02, .3, 0.96 } );
	Fl_Box& modelBox = mainWindow.add<Fl_Box>( relPosition{0., 0., 1., 1. }, modelGroup );
	modelBox.box( FL_BORDER_BOX );

	// Load model button
	Fl_Group& loadButtonGroup = mainWindow.add<Fl_Group>( relPosition_Hor{ 0.05, 1., 0.05 }, modelGroup );
	Fl_Button& loadButton = mainWindow.add<Fl_Button>( relPosition{ 0., 0., 0.5, 1. }, loadButtonGroup, "Load model" );
	loadButton.labelsize( 20 ); loadButton.callback( button_cb, &loadModel );
	Fl_Box& loadButtonBox = mainWindow.add<Fl_Box>( relPosition{ 0.5, 0., 0.5, 1. }, loadButtonGroup );
	loadButtonGroup.resizable( loadButtonBox );

	// Model slice display
	Fl_Group& modelImageGroup = mainWindow.add<Fl_Group>( relPosition{ 0., 0.15, 1., 0.75 }, modelGroup );
	modelGroup.resizable( modelImageGroup );
	Fl_Box& modelImageBox = mainWindow.add<Fl_Box>( relPosition{ 0., 0., 1., .75 }, modelImageGroup, "No model loaded");
	modelImageBox.labelsize( 20 ); modelImageBox.box( FL_BORDER_BOX );
	Fl_Image_Widget& modelImage = mainWindow.add<Fl_Image_Widget>( relPosition{ 0., 0., 1., .75 }, modelImageGroup );
	modelImage.hide();

	// Model rotation
	Fl_Group& rotationGroup = mainWindow.add<Fl_Group>( relPosition_Hor{ 0.75, 0.8, 0.2 }, modelGroup );
	
	Fl_Roller& xRotRoller = mainWindow.add<Fl_Roller>( relPosition{ 0.1, 0., 0.1, 1. }, rotationGroup );
	xRotRoller.type( 0 ); xRotRoller.range( -180., 180. ); xRotRoller.step( 1. );
	xRotRoller.callback( button_cb, &rotateModelX );

	Fl_Roller& yRotRoller = mainWindow.add<Fl_Roller>( relPosition{ 0.3, (1.-0.3)/2, 0.66, 0.2}, rotationGroup);
	yRotRoller.type( 1 ); yRotRoller.range( -180., 180. ); yRotRoller.step( 1. );
	yRotRoller.callback( button_cb, &rotateModelY );

	// Bottom padding
	//Fl_Box& bottomPadding = mainWindow.add<Fl_Box>( relPosition{ 0., 0.8, 1., 0.2 }, rotationGroup );
	//bottomPadding.hide(); rotationGroup.resizable( bottomPadding );

	if( PROGRAM_STATE().SliceLoaded() ){
		modelImage.assignImage( currentState.Slice() );
		modelImage.show();
		modelImageBox.hide();
	}

	
	mainWindow.getFl().show(argc, argv);


	while( Fl::wait() ){

		if( loadModel ){

			loadModel = false;	
			modelImageBox.label( "Loading model..." );
			modelImage.hide(); modelImageBox.show();

			if( PROGRAM_STATE().loadModel() ){
				if( PROGRAM_STATE().sliceModel() ){
					modelImage.assignImage( currentState.Slice() );
					modelImage.show(); modelImageBox.hide();
				}
			}
			else{
				modelImageBox.label("No model loaded!");
			}
		}

		if( rotateModelX && currentState.ModelLoaded() ){
			rotateModelX = false;
			line rotationAxis = GLOBAL_CSYS()->xAxis();

			double absoluteAngle = xRotRoller.value() / 360. * PI;
			
			double rotationAngle = absoluteAngle - currentAngleX; 
			currentAngleX += rotationAngle;

			currentState.Model().CSys()->rotateM( rotationAxis, rotationAngle );

			if( PROGRAM_STATE().sliceModel() ){
				modelImage.assignImage( currentState.Slice() );
				modelImage.show(); modelImageBox.hide();
			}
		}

	}

	currentState.saveState();

	return 0;
}
