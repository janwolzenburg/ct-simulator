#pragma once
/*********************************************************************
 * @file   modelCreator.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   November 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include "FL/Fl_Window.H"
#include "FL/Fl_Group.H"
#include "FL/Fl_Button.H"
#include "FL/Fl_Int_Input.H"
#include "FL/Fl_Float_Input.H"
#include "fl_BoundInput.h"
#include "widgets.h"

//#include "programState.h"
#include "model.h"
#include "callbackFunction.h"



 /*********************************************************************
	Definitions
 *********************************************************************/

/*!
 * @brief Class for a window that can build models
*/
class Fl_ModelCreator : public Fl_Window{

	public:

	/*!
	 * @brief Constructor
	 * @param w Width of window
	 * @param h Height of window
	 * @param label Window title
	*/
	Fl_ModelCreator( int w, int h, const char* label ) :
		Fl_Window{ w, h, label },
		
		model_size_group_{		X( *this, .05 ),				Y( *this, .05 ),				W( *this, .4 ),				H( *this, .9 ), "Model size" },
		model_size_x_input_{	X( model_size_group_, .5 ),		Y( model_size_group_, .05 ),	W( model_size_group_, .4 ),	H( model_size_group_, .05 ), "# Voxel x: " },
		model_size_y_input_{	X( model_size_group_, .5 ),		Y( model_size_group_, .15 ),	W( model_size_group_, .4 ),	H( model_size_group_, .05 ), "# Voxel x: " },
		model_size_z_input_{	X( model_size_group_, .5 ),		Y( model_size_group_, .25 ),	W( model_size_group_, .4 ),	H( model_size_group_, .05 ), "# Voxel x: " },
		voxel_size_x_input_{	X( model_size_group_, .5 ),		Y( model_size_group_, .4 ),	W( model_size_group_, .4 ),	H( model_size_group_, .05 ), "Voxelsize x in mm: " },
		voxel_size_y_input_{	X( model_size_group_, .5 ),		Y( model_size_group_, .5 ),	W( model_size_group_, .4 ),	H( model_size_group_, .05 ), "Voxelsize x in mm: " },
		voxel_size_z_input_{	X( model_size_group_, .5 ),		Y( model_size_group_, .6 ),	W( model_size_group_, .4 ),	H( model_size_group_, .05 ), "Voxelsize x in mm: " },
		store_size_button_{		X( model_size_group_, .5 ),		Y( model_size_group_, .6 ),	W( model_size_group_, .4 ),	H( model_size_group_, .05 ), "Store size" },


		model_size_changed_callback_{ *this, &Fl_ModelCreator::UpdateModelSize },
		model_size_stored_callback_{ *this, &Fl_ModelCreator::StoreModelSize },

		model_size_{ 3, 3, 3 },
		voxel_size_{ 1., 1., 1. }
	{
		Fl_Window::add( model_size_group_ );

		
		model_size_group_.add( model_size_x_input_ ); model_size_group_.add( model_size_y_input_ ); model_size_group_.add( model_size_z_input_ ); 
		model_size_group_.add( voxel_size_x_input_ ); model_size_group_.add( voxel_size_y_input_ ); model_size_group_.add( voxel_size_z_input_ ); 

		model_size_x_input_.align( FL_ALIGN_LEFT ); model_size_y_input_.align( FL_ALIGN_LEFT ); model_size_z_input_.align( FL_ALIGN_LEFT );
		voxel_size_x_input_.align( FL_ALIGN_LEFT ); voxel_size_y_input_.align( FL_ALIGN_LEFT ); voxel_size_z_input_.align( FL_ALIGN_LEFT );

		model_size_x_input_.SetProperties( 1, 4096, 0, None ); model_size_y_input_.SetProperties( 1, 4096, 0, None );
		model_size_z_input_.SetProperties( 1, 4096, 0, None );

		voxel_size_x_input_.SetProperties( .001, 1000., 3, None ); voxel_size_y_input_.SetProperties( .001, 1000., 3, None );
		voxel_size_z_input_.SetProperties( .001, 1000., 3, None );

		model_size_x_input_.value( 3 ); model_size_y_input_.value( 3 ); model_size_z_input_.value( 3 );
		voxel_size_x_input_.value( 1. ); voxel_size_y_input_.value( 1. ); voxel_size_z_input_.value( 1. );

		model_size_x_input_.callback( CallbackFunction<Fl_ModelCreator>::Fl_Callback, &model_size_changed_callback_ );
		model_size_y_input_.callback( CallbackFunction<Fl_ModelCreator>::Fl_Callback, &model_size_changed_callback_ );
		model_size_z_input_.callback( CallbackFunction<Fl_ModelCreator>::Fl_Callback, &model_size_changed_callback_ );
		
		voxel_size_x_input_.callback( CallbackFunction<Fl_ModelCreator>::Fl_Callback, &model_size_changed_callback_ );
		voxel_size_y_input_.callback( CallbackFunction<Fl_ModelCreator>::Fl_Callback, &model_size_changed_callback_ );
		voxel_size_z_input_.callback( CallbackFunction<Fl_ModelCreator>::Fl_Callback, &model_size_changed_callback_ );


		model_size_group_.add( store_size_button_ );
		store_size_button_.callback( CallbackFunction<Fl_ModelCreator>::Fl_Callback, &model_size_stored_callback_ );

	};



	private:

	Fl_Group model_size_group_;
	Fl_BoundInput<Fl_Int_Input, unsigned int> model_size_x_input_;
	Fl_BoundInput<Fl_Int_Input, unsigned int> model_size_y_input_;
	Fl_BoundInput<Fl_Int_Input, unsigned int> model_size_z_input_;
	Fl_BoundInput<Fl_Float_Input, double> voxel_size_x_input_;
	Fl_BoundInput<Fl_Float_Input, double> voxel_size_y_input_;
	Fl_BoundInput<Fl_Float_Input, double> voxel_size_z_input_;
	Fl_Button store_size_button_;
	
	CallbackFunction<Fl_ModelCreator> model_size_changed_callback_;
	CallbackFunction<Fl_ModelCreator> model_size_stored_callback_;


	Index3D model_size_;
	Tuple3D voxel_size_;


	void UpdateModelSize( void ){

		model_size_.x = ForceRange<unsigned int>( model_size_x_input_.value(), 1, 4096 );
		model_size_.y = ForceRange<unsigned int>( model_size_y_input_.value(), 1, 4096 );
		model_size_.z = ForceRange<unsigned int>( model_size_z_input_.value(), 1, 4096 );

		voxel_size_.x = ForceRange<double>( voxel_size_x_input_.value(), .001, 1000. );
		voxel_size_.y = ForceRange<double>( voxel_size_y_input_.value(), .001, 1000. );
		voxel_size_.z = ForceRange<double>( voxel_size_z_input_.value(), .001, 1000. );

	};

	void StoreModelSize( void ){

		model_size_group_.deactivate();

	};

};