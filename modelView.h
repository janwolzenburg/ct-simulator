#pragma once
/*********************************************************************
 * @file   modelView.h
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

#include "modelViewProperties.h"
#include "widgets.h"
#include "mainWindow.fwd.h"


/*********************************************************************
   Definitions
*********************************************************************/


/*!
 * @brief Class for the model viewer
*/
class ModelView : public Fl_Group{

	public: 

	/*!
	 * @brief Constructor
	 * @param x x-position
	 * @param y y-position
	 * @param w Width
	 * @param h Height
	 * @param main_window Reference to the main window
	*/
	ModelView( int x, int y, int w, int h, mainWindow& main_window );

	/*!
	 * @brief Destructor
	*/
	~ModelView( void );

	/*!
	 * @brief Get Reference to model
	 * @return Constant reference to model
	*/
	const Model& model( void ){ return model_; };

	/*!
	 * @brief Check if a model has been loaded
	 * @return True when a model has been loaded
	*/
	bool IsModelLoaded( void ) const{ return storedModel.was_loaded(); };


	private:
	
	Fl_Box title;		/*!<Title*/
	
	Fl_Group headGrp;	/*!<Header group*/
	Fl_Button loadBtn;	/*!<Button to load model*/

	Fl_Group viewGrp;						/*!<Group to view the model*/
	Fl_Multiline_Output modelData;			/*!<Model properties_*/
	Fl_Button resetBtn;						/*!<Button to reset model to default*/
	Fl_Box viewBox;							/*!<Box to show test when no model is loaded*/
	Fl_AdjustableGrayscaleImage viewImg;	/*!<Image with adjustable contrast to show model slice*/
	
	Fl_Group moveGrp;	/*!<Group for the model movement*/
	Fl_Counter xRot;	/*!<Input for x-rotation*/
	Fl_Counter yRot;	/*!<Input for y-rotation*/
	Fl_Counter zTrans;	/*!<Input for z-transloation*/


	mainWindow& main_window_;

	FileChooser modelChooserInstance;			/*!<File chooser for the model*/
	string modelDataString;	/*!<String with model properties_*/
	Model model_;						/*!<Current model*/
	ModelViewProperties modelViewPara;			/*!<Parameter of the model view*/
	DataGrid<VoxelData> modelSliceInstance;		/*!<Slice through model as gridded data*/
	
	PersistingObject<FileChooser> storedModelChooser;			/*!<Persisting storage of model chooser*/
	PersistingObject<Model> storedModel;						/*!<Persisting storage of current model*/
	PersistingObject<ModelViewProperties> storedViewParameter;	/*!<Persisting storage of view parameters*/




	
	CallbackMethod<ModelView> load_model;
	CallbackMethod<ModelView> update_model_;
	CallbackMethod<ModelView> reset_model_;
	

	/*!
	 * @brief Get the model description
	 * @return String with model information
	*/
	string modelDescription( void ) const;

	
	/*!
	 * @brief Load the model a stored path
	 * @return True at success
	*/
	void loadModel( void );

	/*!
	 * @brief Move model to given values with respect to the slice plane coordinate system
	 * @param targetXRot Rotation around x-axis
	 * @param targetYRot Rotation around y-axis
	 * @param targetZTrans Translation in z-direction
	 * @return True at success
	*/
	bool moveModel( double& targetXRot, double& targetYRot, double& targetZTrans );
	
	/*!
	 * @brief Center the model
	*/
	void centerModel( void );
	
	/*!
	 * @brief Slice model with stored slice plane
	 * @return True at success
	*/
	bool sliceModel( void );

	/*!
	 * @brief Update the model view
	*/
	void UpdateModel( void );

	/*!
	 * @brief Reset model's position
	*/
	void resetModel( void );

};