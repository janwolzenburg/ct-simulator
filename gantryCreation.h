#pragma once
/*********************************************************************
 * @file   gantryCreation.h
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

#include "programState.h"
#include "Fl_Plot.h"
#include "Fl_Bound_Input.h"
#include "Fl_Selector.h"
#include "lineplot.h"
#include "geoplot.h"


/*!
 * @brief Class for a GUI element for gantry creation
*/
class gantryEdition : public Fl_Group{

	public:

	/*!
	 * @brief Constructor
	 * @param x x-position
	 * @param y y-position
	 * @param w Width
	 * @param h Height
	*/
	gantryEdition( int x, int y, int w, int h );

	~gantryEdition( void );

	Gantry& gantry( void ){ return gantryInstance; };

	ProjectionsProperties projections_properties ( void ){ return radonParameter; };

	
	/*!
	 * @brief Set flag for gantry update
	*/
	void setUpdateFlag( void ){ updateGantry = true; };

	/*!
	 * @brief Check if gantry needs update 
	 * @return True when gantry needs update
	*/
	bool UpdateGantry( void ){ return UnsetFlag( updateGantry ); };

	/*!
	 * @brief Handle events
	*/
	void handleEvents( void );


	private:

	XRayTubeProperties xRayTubeParameter;									/*!<xRay tube attributes*/
	PersistingObject<XRayTubeProperties> storedXRayTubeParameter;				/*!<Persisting storage of tube attributes*/
	ProjectionsProperties radonParameter;								/*!<Parameter in radon space affecting the detector*/
	PersistingObject<ProjectionsProperties> storedRadonParameter;			/*!<Persisting storage of radon parameter*/
	PhysicalDetectorProperties physical_detector_properties_;						/*!<Parameter only dependent on the physical properties_ od detector*/
	PersistingObject<PhysicalDetectorProperties> storedDetectorParameter;	/*!<Persisting storage of the detector parameter*/
	Gantry gantryInstance;												/*!<Instance of the gantry constructed from tube and detector parameter*/

	Fl_Box title;		/*!<Title*/

	Fl_Group tubeGrp;					/*!<Group of tube elements*/
	Fl_Box tubeTitle;					/*!<Tube group title*/
	Fl_Bound_Input<Fl_Float_Input, double> tubeVoltageIn;	/*!<Tube voltage*/
	Fl_Bound_Input<Fl_Float_Input, double> tubeCurrentIn;	/*!<Tube current*/
	Fl_Selector materialIn;				/*!<Anode material*/
	Fl_Plot<lineplot> spectrumPlot;		/*!<Plot of tube sepctrum*/

	Fl_Group detectorGrp;	/*!<Group for detector elemnts*/
	Fl_Box detectorTitle;	/*!<Detector group title*/
	Fl_Bound_Input<Fl_Int_Input, size_t> colPnts;			/*!<Amount of angles in sinogram*/
	Fl_Bound_Input<Fl_Int_Input, size_t> rowPnts;			/*!<Amount of distances in sinogram*/
	Fl_Bound_Input<Fl_Float_Input, double> distRange;		/*!<Measure field*/
	Fl_Bound_Input<Fl_Int_Input, int> raysPerPixelIn;		/*!<Amount of rays per pixel to simulate*/
	Fl_Bound_Input<Fl_Float_Input, double> detector_focus_distance_input;		/*!<Detector arc radius*/
	Fl_Toggle_Button structureIn;							/*!<Anti scattering structure toggle*/
	Fl_Bound_Input<Fl_Float_Input, double> maxRayAngleIn;	/*!<Maximum angle when structure is enabled*/
	Fl_Plot<geoplot> detectorPlot;							/*!<Plot for detector geometry*/

	bool updateGantry;	/*!<Flag for gantry update*/

};