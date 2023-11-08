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

#include "programState.h"
#include "modelViewProperties.h"
#include "fl_AdjustableGrayscaleImage.h"


/*********************************************************************
   Definitions
*********************************************************************/


/*!
 * @brief Class for the model viewer
*/
class modelView : public Fl_Group{

	public: 

	/*!
	 * @brief Constructor
	 * @param x x-position
	 * @param y y-position
	 * @param w Width
	 * @param h Height
	*/
	modelView( int x, int y, int w, int h );

	/*!
	 * @brief Destructor
	*/
	~modelView( void );

	/*!
	 * @brief Get Reference to model
	 * @return Constant reference to model
	*/
	const Model& model( void ){ return modelInstance; };

	/*!
	 * @brief Check if a model has been loaded
	 * @return True when a model has been loaded
	*/
	bool ModelLoaded( void ) const{ return storedModel.was_loaded(); };

	/*!
	 * @brief Set flag for model update
	*/
	void setUpdateFlag( void ){ updateModelFlag = true; };

	/*!
	 * @brief Handle all events
	*/
	void handleEvents( void );


	private:
	
	FileChooser modelChooserInstance;			/*!<File chooser for the model*/
	Model modelInstance;						/*!<Current model*/
	ModelViewProperties modelViewPara;			/*!<Parameter of the model view*/
	DataGrid<VoxelData> modelSliceInstance;		/*!<Slice through model as gridded data*/
	
	PersistingObject<FileChooser> storedModelChooser;			/*!<Persisting storage of model chooser*/
	PersistingObject<Model> storedModel;						/*!<Persisting storage of current model*/
	PersistingObject<ModelViewProperties> storedViewParameter;	/*!<Persisting storage of view parameters*/


	Fl_Group headGrp;	/*!<Header group*/
	Fl_Button loadBtn;	/*!<Button to load model*/

	Fl_Group viewGrp;				/*!<Group to view the model*/
	Fl_Multiline_Output modelData;	/*!<Model properties_*/
	Fl_Box viewBox;					/*!<Box to show test when no model is loaded*/
	Fl_AdjustableGrayscaleImage viewImg;	/*!<Image with adjustable contrast to show model slice*/
	
	Fl_Group moveGrp;	/*!<Group for the model movement*/
	Fl_Counter xRot;	/*!<Input for x-rotation*/
	Fl_Counter yRot;	/*!<Input for y-rotation*/
	Fl_Counter zTrans;	/*!<Input for z-transloation*/
	Fl_Button resetBtn;	/*!<Button to reset model to default*/

	string modelDataString;	/*!<String with model properties_*/
	bool loadBtnPressed;	/*!<Flag for model load*/
	bool updateModelFlag;	/*!<Flag for model update*/
	bool resetBtnPressed;	/*!<Flag for model reset*/
	

	/*!
	 * @brief Get the model description
	 * @return String with model information
	*/
	string modelDescription( void ) const;

	/*!
	 * @brief Check if load button was pressed
	 * @return True when it was pressed
	*/
	bool LoadBtnPressed( void ){ return UnsetFlag( loadBtnPressed ); };

	/*!
	 * @brief Check if model needs update
	 * @return True when model need update and a model is loaded
	*/
	bool ModelNeedsUpdate( void ){ return UnsetFlag( updateModelFlag )&& ModelLoaded(); };

	/*!
	 * @brief Check if reset button was pressed
	 * @return True when it was pressed
	*/
	bool ResetBtnPressed( void ){ return UnsetFlag( resetBtnPressed ); };
	
	/*!
	 * @brief Load the model a stored path
	 * @return True at success
	*/
	bool loadModel( void );

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