#pragma once
/*********************************************************************
 * @file   fl_GantryCreation.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   March 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include <FL/Fl_Group.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Tooltip.H>

#include "fl_Plot.h"
#include "fl_BoundInput.h"
#include "fl_Selector.h"

#include "persistingObject.h"
#include "linePlot.h"
#include "geometryplot.h"
#include "widgets.h"
#include "fl_MainWindow.fwd.h"
#include "callbackFunction.h"
#include "gantry.h"
#include "xRayTube.h"
#include "projectionsProperties.h"

/*!
 * @brief class for a GUI element for gantry creation
*/
class Fl_GantryCreation : public Fl_Group{

	public:

	/*!
	 * @brief constructor
	 * @param x x-position
	 * @param y y-position
	 * @param w Width
	 * @param h Height
	 * @param main_window Reference to the main window
	*/
	Fl_GantryCreation( int x, int y, int w, int h, Fl_MainWindow& main_window );

	/*!
	 * @brief get reference to gantry instance
	 * @return reference to gantry
	*/
	Gantry& gantry( void ){ return gantry_; };

	/*!
	 * @brief get projection properties
	 * @return Current porjection properties
	*/
	ProjectionsProperties projections_properties( void ){ return projections_properties_; };

	/*!
	 * @brief set distance range and detector-focus distance
	 * @param max_corner_distance Longest distance between corners in model slice
	*/
	void SetDistances( const double max_corner_distance );

	/*!
	 * @brief Update gantry
	*/
	void UpdateGantry( void );


	private:

	Fl_Box title_;		/*!< Title*/

	Fl_Group tube_group_;				/*!< Group of tube elements*/
	Fl_Box tube_title_;					/*!< Tube group title*/
	Fl_BoundInput<Fl_Float_Input, double> voltage_input_;	/*!< Tube voltage*/
	Fl_BoundInput<Fl_Float_Input, double> current_input_;	/*!< Tube current*/
	Fl_Selector anode_material_input_;		/*!< anode material*/
	Fl_Toggle_Button toggle_filter_button_;	/*!< Toggle Filter*/
	Fl_BoundInput<Fl_Float_Input, double> filter_cutoff_input;		/*!< Filter cut of energy*/
	Fl_BoundInput<Fl_Float_Input, double> filter_gradient_input;	/*!< Filter strength*/
	Fl_Plot<LinePlot> spectrum_plot_;		/*!< plot of tube sepctrum*/

	Fl_Group detector_group_;	/*!< Group for detector elemnts*/
	Fl_Box detector_title_;	/*!< detector group title*/
	Fl_BoundInput<Fl_Int_Input, size_t> number_of_angles_input_;			/*!< amount of angles in sinogram*/
	Fl_BoundInput<Fl_Int_Input, size_t> number_of_distances_input_;			/*!< amount of distances in sinogram*/
	Fl_BoundInput<Fl_Float_Input, double> distance_range_input_;			/*!< measure field*/
	Fl_BoundInput<Fl_Int_Input, int> number_of_rays_per_pixel_input_;		/*!< amount of rays per pixel to simulate*/
	Fl_BoundInput<Fl_Float_Input, double> detector_focus_distance_input_;	/*!< detector arc radius*/
	Fl_Toggle_Button scattering_structure_input_;							/*!< anti scattering structure toggle*/
	Fl_BoundInput<Fl_Float_Input, double> maximum_ray_angle_input_;			/*!< maximum angle when structure is enabled*/
	Fl_Plot<Geometryplot> detector_plot_;									/*!< plot for detector geometry*/

	Fl_MainWindow& main_window_;	/*!< reference to main window*/

	PersistingObject<XRayTubeProperties> tube_properties_;						/*!< xRay tube attributes*/
	PersistingObject<ProjectionsProperties> projections_properties_;			/*!< parameter in radon space affecting the detector*/
	PersistingObject<PhysicalDetectorProperties> physical_detector_properties_;	/*!< parameter only dependent on the physical properties_ od detector*/
	Gantry gantry_;		/*!< Instance of the gantry constructed from tube and detector parameter*/

	CallbackFunction<Fl_GantryCreation> update_gantry_callback_;	/*!< callback for gantry update*/
};