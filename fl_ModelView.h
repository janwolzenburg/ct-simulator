#pragma once
/*********************************************************************
 * @file   fl_ModelView.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   March 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include "generel.h"

#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Window.H>

#include "fl_AdjustableGrayscaleImage.h"
#include "fl_MainWindow.fwd.h"

#include "fileChooser.h"

#include "modelViewProperties.h"
#include "persistingObject.h"
#include "widgets.h"
#include "model.h"
#include "callbackFunction.h"


/*********************************************************************
   Definitions
*********************************************************************/


/*!
 * @brief class for the model viewer
*/
class Fl_ModelView : public Fl_Group{

	public: 

	/*!
	 * @brief constructor
	 * @param x x-position
	 * @param y y-position
	 * @param w Width
	 * @param h Height
	 * @param main_window Reference to the main window
	*/
	Fl_ModelView( int x, int y, int w, int h, Fl_MainWindow& main_window );

	/*!
	 * @brief get Reference to model
	 * @return constant reference to model
	*/
	const Model& model( void ){ return model_; };

	/*!
	 * @brief check if a model has been loaded
	 * @return true when a model has been loaded
	*/
	bool IsModelLoaded( void ) const{ return model_.was_loaded(); };


	private:
	
	Fl_Box title_;					/*!< title*/
	
	Fl_Group head_group_;			/*!< header group*/
	Fl_Button load_model_button_;	/*!< button to load model*/

	Fl_Group model_inspection_group_;				/*!< Group to view the model*/
	Fl_Multiline_Output model_information_;			/*!< model properties_*/
	Fl_Button reset_model_button_;					/*!< button to reset model to default*/
	Fl_Box loading_status_;							/*!< box to show test when no model is loaded*/
	Fl_AdjustableGrayscaleImage model_slice_image_;	/*!< Image with adjustable contrast to show model slice*/
	
	Fl_Group model_movement_group_;	/*!< Group for the model movement*/
	Fl_Counter x_rotation_;			/*!< Input for x-rotation*/
	Fl_Counter y_rotation_;			/*!< Input for y-rotation*/
	Fl_Counter z_position_;			/*!< Input for z-transloation*/
	Fl_Counter artefact_impact_;	/*!< Input for artefact impact*/
	
	PersistingObject<ModelViewProperties> properties_;	/*!< persisting storage of view parameters*/
	Fl_MainWindow& main_window_;						/*!< reference to main window*/
	PersistingObject<FileChooser> model_chooser_;		/*!< persisting storage of model chooser*/
	PersistingObject<Model> model_;						/*!< persisting storage of current model*/

	string model_information_string_;					/*!< string with model properties_*/
	DataGrid<VoxelData> model_slice_;					/*!< slice through model as gridded data*/
	

	CallbackFunction<Fl_ModelView> load_model_callback_;		/*!< callback for model loading*/
	CallbackFunction<Fl_ModelView> update_model_callback_;		/*!< callback for model update*/
	CallbackFunction<Fl_ModelView> reset_model_callback_;		/*!< callback for model reset*/
	CallbackFunction<Fl_ModelView> update_artefact_impact_;		/*!< callback for change of artifact impact*/

	/*!
	 * @brief get the model description
	 * @return string with model information
	*/
	string GetModelDescription( void ) const;

	/*!
	 * @brief Load the model a stored path
	 * @return true at success
	*/
	void LoadModel( void );

	/*!
	 * @brief Move model to given values with respect to the slice plane coordinate system
	 * @param target_x_angle Rotation around x-axis
	 * @param target_y_angle Rotation around y-axis
	 * @param target_z_position Translation in z-direction
	 * @return true at success
	*/
	bool MoveModel( double& target_x_angle, double& target_y_angle, double& target_z_position );
	
	/*!
	 * @brief center the model
	*/
	void CenterModel( void );
	
	/*!
	 * @brief reset model's position
	*/
	void ResetModel( void );

	/*!
	 * @brief slice model with stored slice plane
	 * @return true at success
	*/
	bool SliceModel( void );

	/*!
	 * @brief Update the model view
	*/
	void UpdateModel( void );

	/*!
	 * @brief Update VoxelData artefact impact inputted
	*/
	void UpdateArtefactImpact( void );
};