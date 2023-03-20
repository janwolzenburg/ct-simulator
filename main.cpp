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
	
	Own_Valuator<Fl_Counter>& xRot = mainWindow.add<Own_Valuator<Fl_Counter>>( relPosition_Hor{ .15, 1., .15 }, rotationGroup, "x-Rotation");
	xRot.range( -180., 180. ); xRot.step( 1., 10. ); xRot.hide();
	xRot.value( currentState.getValStatus().xRotValue );

	Own_Valuator<Fl_Counter>& yRot = mainWindow.add<Own_Valuator<Fl_Counter>>( relPosition_Hor{ .4, 1., .15 }, rotationGroup, "y-Rotation");
	yRot.range( -180., 180. ); yRot.step( 1., 10. ); yRot.hide();
	yRot.value( currentState.getValStatus().yRotValue );

	Own_Valuator<Fl_Counter>& zTrans = mainWindow.add<Own_Valuator<Fl_Counter>>( relPosition_Hor{ .75, 1., .15 }, rotationGroup, "z-Translation" );
	zTrans.range( -400., 400 ); zTrans.step( 1., 10. ); zTrans.hide();
	zTrans.value( currentState.getValStatus().zTransValue );

	// Bottom padding
	//Fl_Box& bottomPadding = mainWindow.add<Fl_Box>( relPosition{ 0., 0.8, 1., 0.2 }, rotationGroup );
	//bottomPadding.hide(); rotationGroup.resizable( bottomPadding );

	if( currentState.ModelLoaded() ){

		if( PROGRAM_STATE().sliceModel() ){
			modelImage.assignImage( currentState.Slice() );
			modelImage.show(); modelImageBox.hide();
			xRot.show(); yRot.show(); zTrans.show();
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
					xRot.show(); yRot.show(); zTrans.show();
				}
			}
			else{
				modelImageBox.label("No model loaded!");
			}
		}

		if( xRot.ChangeFlag() && currentState.ModelLoaded() ){
			currentState.getValStatus().xRotValue = xRot.value();
			xRot.deactivate();

			if( PROGRAM_STATE().rotateViewX( xRot.value() ) ){
				modelImage.assignImage( currentState.Slice() );	
			}

			xRot.activate();
		}

		if( yRot.ChangeFlag() && currentState.ModelLoaded() ){
			currentState.getValStatus().yRotValue = yRot.value();
			yRot.deactivate();

			if( PROGRAM_STATE().rotateViewY( yRot.value() ) ){
				modelImage.assignImage( currentState.Slice() );
			}

			yRot.activate();
		}

		if( zTrans.ChangeFlag() && currentState.ModelLoaded() ){
			currentState.getValStatus().zTransValue = zTrans.value();
			zTrans.deactivate();

			if( PROGRAM_STATE().translateViewZ( zTrans.value() ) ){
				modelImage.assignImage( currentState.Slice() );
			}

			zTrans.activate();
		}

	}

	currentState.saveState();

	return 0;
}
