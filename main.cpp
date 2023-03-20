/*********************************************************************
 * @file   main.cpp
 * @brief  Main file
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/

#include <iostream>
using std::cerr;  using std::endl; using std::cout;

#include <type_traits>

#include "programState.h"
#include "gui.h"
#include "test_model.h"
#include "cSysTree.h"





/*!
 * @brief Main function
 * @param  
 * @return 
*/
int main( int argc, char** argv ){

	//model testModel = getTestModel( GLOBAL_CSYS(), 20 );

	//vector<char> modelBin;
	//testModel.serialize( modelBin );
	//exportSerialized(path{ "./testModel_20x.model" }, modelBin );

	//return 0;

	programState& currentState = PROGRAM_STATE();

	bool loadModel = false;//;!PROGRAM_STATE().ModelLoaded();

	//bool rotateModelX = false; 
	double currentAngleX = 0;
	//bool rotateModelY = false; 
	double currentAngleY = 0;

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
	Fl_Group& rotationGroup = mainWindow.add<Fl_Group>( relPosition_Hor{ 0.75, 0.5, 0.25 }, modelGroup );
	
	Fl_Own_Valuator<Fl_Counter>& xRotRoller = mainWindow.add<Fl_Own_Valuator<Fl_Counter>>( relPosition_Hor{ .2, 1., .2 }, rotationGroup, "x-Rotation");
	xRotRoller.range( -180., 180. ); xRotRoller.step( 1., 10. ); xRotRoller.hide();
	xRotRoller.value( currentState.getValStatus().xRotRoller );

	Fl_Own_Valuator<Fl_Counter>& yRotRoller = mainWindow.add<Fl_Own_Valuator<Fl_Counter>>( relPosition_Hor{ .6, 1., .2 }, rotationGroup, "y-Rotation");
	yRotRoller.range( -180., 180. ); yRotRoller.step( 1., 10. ); yRotRoller.hide();
	yRotRoller.value( currentState.getValStatus().yRotRoller );

	// Bottom padding
	//Fl_Box& bottomPadding = mainWindow.add<Fl_Box>( relPosition{ 0., 0.8, 1., 0.2 }, rotationGroup );
	//bottomPadding.hide(); rotationGroup.resizable( bottomPadding );

	if( currentState.ModelLoaded() ){

		if( PROGRAM_STATE().sliceModel() ){
			modelImage.assignImage( currentState.Slice() );
			modelImage.show(); modelImageBox.hide();
			xRotRoller.show(); yRotRoller.show();
		}

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
					xRotRoller.show(); yRotRoller.show();
				}
			}
			else{
				modelImageBox.label("No model loaded!");
			}
		}

		if( xRotRoller.ChangeFlag() && currentState.ModelLoaded() ){
			currentState.getValStatus().xRotRoller = xRotRoller.value();
			xRotRoller.deactivate();

			if( PROGRAM_STATE().rotateViewX( xRotRoller.value() ) ){
				modelImage.assignImage( currentState.Slice() );	
			}

			xRotRoller.activate();
		}

		if( yRotRoller.ChangeFlag() && currentState.ModelLoaded() ){
			currentState.getValStatus().yRotRoller = yRotRoller.value();
			yRotRoller.deactivate();

			if( PROGRAM_STATE().rotateViewY( yRotRoller.value() ) ){
				modelImage.assignImage( currentState.Slice() );
			}

			yRotRoller.activate();
		}

	}

	currentState.saveState();

	return 0;
}
