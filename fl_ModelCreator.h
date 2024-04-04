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
#include "FL/Fl_Toggle_Button.H"
#include "FL/Fl_Multiline_Output.H"
#include "fl_BoundInput.h"
#include "Fl_Selector.h"
#include "fl_ProgressWindow.h"
#include "widgets.h"

#include <memory>
using std::unique_ptr;
#include <array>
using std::array;

#include "model.h"
#include "callbackFunction.h"
#include "fileChooser.h"
#include "persistingObject.h"



 /*********************************************************************
	Definitions
 *********************************************************************/

/*!
 * @brief class used by Fl_ModelCreator to store model features
*/
class Fl_ModelFeature : public Fl_Group{

	public:

	/*!
	 * @brief shape of feature
	*/
	enum Shape{
		Sphere,
		Cube
	};

	static const std::map<Shape, string> shape_names;			/*!< map for shape names*/
		
	/*!
	 * @brief get shape enumeration from string
	 * @param shape_string Name of enumeration
	 * @return enumeration of shape fitting to name
	*/
	static Shape GetShapeEnum( const string shape_string );		
	
	/*!
	 * @brief constructor
	 * @param x x-position
	 * @param y y-position
	 * @param w Width
	 * @param h Height
	 * @param label Label
	*/
	Fl_ModelFeature( int x, int y, int w, int h, const char* label = 0L );

	/*!
	 * @brief register same function to all elements as callback
	 * @param callback_function Function to assign as callback
	 * @param p User data
	*/
	void callback( Fl_Callback* callback_function, void* p );

	/*!
	 * @brief get if this feature is active
	 * @return true if active
	*/
	bool IsActive( void ) const{ return static_cast<bool>( active_button_.value() ); };

	/*!
	 * @brief get feature shape
	 * @return shape
	*/
	Shape GetShape( void ) const{ return GetShapeEnum( string{ shape_input_.current_element() } ); };

	/*!
	 * @brief get center of feature
	 * @return center of feature in mm
	*/
	Tuple3D GetCenter( void ) const{ return { x_positon_input_.value(), y_positon_input_.value(), z_positon_input_.value() }; };

	/*!
	 * @brief get size of feature
	 * @return size in mm
	*/
	double GetSize( void ) const{ return size_input_.value(); };

	/*!
	 * @brief get special property
	 * @return property of feature
	*/
	VoxelData::SpecialProperty GetProperty( void ) const{ return VoxelData::GetPropertyEnum( string{ special_property_input_.current_element() } ); };

	/*!
	 * @brief get absorption of feature
	 * @return absorption at reference energy
	*/
	double GetValue( void ) const{ return value_input_.value(); };

	/*!
	 * @brief set this feature to be active
	 * @param  
	*/
	void SetActive( void ){
		active_button_.value( 1 ); ShowFields(); };

	/*!
	 * @brief Hide fields
	*/
	void HideFields( void );

	/*!
	 * @brief show fields
	*/
	void ShowFields( void );


	private:

	Fl_Toggle_Button active_button_;		/*!< Button to activate feature*/
	Fl_Selector special_property_input_;	/*!< selector for special property*/
	Fl_BoundInput<Fl_Float_Input, double> value_input_;		/*!< absorption in 1/mm at reference energy*/
	Fl_BoundInput<Fl_Float_Input, double> x_positon_input_;	/*!< x position in mm*/
	Fl_BoundInput<Fl_Float_Input, double> y_positon_input_;	/*!< y position in mm*/
	Fl_BoundInput<Fl_Float_Input, double> z_positon_input_;	/*!< z position in mm*/
	Fl_BoundInput<Fl_Float_Input, double> size_input_;		/*!< size in mm*/
	Fl_Selector shape_input_;				/*!< selector for feature shape*/

	bool is_visible_;						/*!< Flag to indicate visability*/

 };



/*!
 * @brief class for a window that can build models
*/
class Fl_ModelCreator : public Fl_Window{


	public:

	/*!
	 * @brief constructor
	 * @param w Width of window
	 * @param h Height of window
	 * @param label Window title
	*/
	Fl_ModelCreator( int w, int h, const char* label );


	private:

	constexpr static size_t num_features = 10;		/*!< max. amount of features*/
	constexpr static double h_feature = .05;		/*!< height of each feature in GUI*/

	Fl_Group model_size_group_;	/*!< Group for model size*/
	Fl_BoundInput<Fl_Int_Input, unsigned int> model_size_x_input_;	/*!< Voxel amount in x direction*/
	Fl_BoundInput<Fl_Int_Input, unsigned int> model_size_y_input_;	/*!< Voxel amount in y direction*/
	Fl_BoundInput<Fl_Int_Input, unsigned int> model_size_z_input_;	/*!< Voxel amount in z direction*/
	Fl_BoundInput<Fl_Float_Input, double> voxel_size_x_input_;	/*!< Voxel size in x direction*/
	Fl_BoundInput<Fl_Float_Input, double> voxel_size_y_input_;	/*!< Voxel size in y direction*/
	Fl_BoundInput<Fl_Float_Input, double> voxel_size_z_input_;	/*!< Voxel size in z direction*/
	Fl_Input name_input_;			/*/*!< name of model*/
	Fl_Button store_size_button_;	/*!< Button to store size and continue*/
	
	Fl_Group features_group_;	/*!< Group for all features and model properties*/
	Fl_BoundInput<Fl_Float_Input, double> background_input_;	/*!< absorption value of background*/
	Fl_Multiline_Output information_;							/*!< Information about features*/
	vector<unique_ptr<Fl_ModelFeature>> features_;				/*!< all the features*/
	Fl_Button build_button_;									/*!< Button for building model*/

	CallbackFunction<Fl_ModelCreator> model_size_changed_callback_;	/*!< callback for model size change*/
	CallbackFunction<Fl_ModelCreator> model_size_stored_callback_;	/*!< callback for size storage*/
	CallbackFunction<Fl_ModelCreator> feature_changed_callback_;	/*!< callback for feature change*/
	CallbackFunction<Fl_ModelCreator> build_model_callback_;		/*!< callback for model building*/

	Index3D model_size_;		/*!< size of model. The amount of voxel*/
	Tuple3D voxel_size_;		/*!< Voxel size in mm*/
	string name_;				/*/*!< name of model*/

	double background_absorption_;	/*!< Value of background absorption*/


	/*!
	 * @brief Update size of model and voxel
	*/
	void UpdateModelSize( void );

	/*!
	 * @brief store model size and continue to next step  
	*/
	void StoreModelSize( void );

	/*!
	 * @brief Update model features
	*/
	void UpdateFeatures( void );

	/*!
	 * @brief Build and store model 
	*/
	void BuildModel( void );
};