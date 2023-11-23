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
#include <memory>
using std::unique_ptr;

#include "programState.h"
#include "test_model.h"
#include "coordinateSystemTree.h"
#include "fl_ProgressWindow.h"

#include "fl_MainWindow.h"
#include "fl_ProcessingWindow.h"

#include "fl_ModelCreator.h"

/*!
 * @brief Main function
 * @param  
 * @return 
*/
int main( [[maybe_unused]] int argc, [[maybe_unused]] char** argv ){

	
	//Model testModel = getTestModel( GlobalSystem(), 20 );
	
	//Model debugModel{ GlobalSystem()->CreateCopy( "Model system"), Index3D{3, 3, 1}, Tuple3D{1., 1., 1.}, string{"Debug model"}};
	/*
	debugModel.SetVoxelData( VoxelData{ mu_water, reference_energy_for_mu_eV, }, Index3D{ 0, 0, 0 } );
	debugModel.SetVoxelData( VoxelData{ mu_water, reference_energy_for_mu_eV, }, Index3D{ 0, 1, 0 } );
	debugModel.SetVoxelData( VoxelData{ mu_water, reference_energy_for_mu_eV, }, Index3D{ 0, 2, 0 } );
	debugModel.SetVoxelData( VoxelData{ mu_water, reference_energy_for_mu_eV, }, Index3D{ 1, 0, 0 } );
	debugModel.SetVoxelData( VoxelData{ mu_bone, reference_energy_for_mu_eV, }, Index3D{ 1, 1, 0 } );
	debugModel.SetVoxelData( VoxelData{ mu_water, reference_energy_for_mu_eV, }, Index3D{ 1, 2, 0 } );
	debugModel.SetVoxelData( VoxelData{ mu_water, reference_energy_for_mu_eV, }, Index3D{ 2, 0, 0 } );
	debugModel.SetVoxelData( VoxelData{ mu_water, reference_energy_for_mu_eV, }, Index3D{ 2, 1, 0 } );
	debugModel.SetVoxelData( VoxelData{ mu_water, reference_energy_for_mu_eV, }, Index3D{ 2, 2, 0 } );
	

	PersistingObject<Model> saveableModel{ Model{}, path{"./debug.model"} };
	saveableModel = debugModel;
	saveableModel.Save( path{". / debug.model"}, true );
	return 0;
	
	*/
	

	Fl_Group::current( NULL );
	Fl_Tooltip::enable();
	Fl_Tooltip::hoverdelay( (float) 0.05 );

	Fl_MainWindow* mainWindow_ = new Fl_MainWindow{ static_cast<int>( 1920. * 0.9 ), static_cast<int>( 1080. * 0.9 ), "CT-Simulator" };

	mainWindow_->hide();

	Fl_Progress_Window* initialWindow = new Fl_Progress_Window{ mainWindow_, 20, 3, "Initialisation" };
	initialWindow->ChangeLineText( 1, "Loading program state..." );
	initialWindow->show();


	bool firstLoop = true;


	while( Fl::wait() ){
		
		if( firstLoop ){
			mainWindow_->show();
			initialWindow->hide();
			firstLoop = false;
		}
	}

	delete mainWindow_;
	delete initialWindow;

	
	return 0;
	
}
