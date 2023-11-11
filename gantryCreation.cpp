/******************************************************************
* @file   gantryCreation.cpp
* @brief  Implementations
*
* @author Jan Wolzenburg
* @date   April 2023
* ********************************************************************/



/*********************************************************************
  Includes
*********************************************************************/

#include "generel.h"
#include "gantryCreation.h"
#include "widgets.h"
#include "plot.h"
#include "tomographyExecution.h"
#include "mainWindow.h"

/*********************************************************************
  Implementations
*********************************************************************/



gantryEdition::gantryEdition( int x, int y, int w, int h, mainWindow& main_window ) :
	Fl_Group{ x, y, w, h },
	main_window_( main_window ),
	xRayTubeParameter{},
	storedXRayTubeParameter{ PROGRAM_STATE().getPath( "storedTubeParameter.txt" ), xRayTubeParameter },
	radonParameter{},
	storedRadonParameter{  PROGRAM_STATE().getPath( "storedRadonParameter.txt" ), radonParameter },
	physical_detector_properties_{},
	storedDetectorParameter{  PROGRAM_STATE().getPath( "storedDetectorParameter.txt" ), physical_detector_properties_ },
	gantryInstance{ CoordinateSystems().AddSystem( "Gantry system"), xRayTubeParameter, radonParameter, physical_detector_properties_ },

	title{			X( *this, 0. ),		Y( *this, 0. ),		W( *this, 1. ),		H( *this, 0.035 ),	"Gantry" },

	tubeGrp{		X( *this, .0 ),		Y( *this, .035 ) ,	W( *this, 1. ),		H( *this, .4 ) },
	tubeTitle{		X( tubeGrp, 0. ),	Y( tubeGrp, 0. ),	W( tubeGrp, 1. ),	H( tubeGrp, .075 ),	"xRay Tube" },
	tubeVoltageIn{	X( tubeGrp, .0 ),	Y( tubeGrp, .15 ),	W( tubeGrp, .15 ),	H( tubeGrp, .1 ),	"Voltage" },
	tubeCurrentIn{	X( tubeGrp, .25 ),	Y( tubeGrp, .15 ),	W( tubeGrp, .15 ),	H( tubeGrp, .1 ),	"Current" },
	materialIn{		X( tubeGrp, .5 ),	Y( tubeGrp, .15 ),	W( tubeGrp, .5 ),	H( tubeGrp, .1 ),	"Material" },
	spectrumPlot{	X( tubeGrp, .0 ),	Y( tubeGrp, .325 ),	W( tubeGrp, 1. ),	H( tubeGrp, .725 ),	"Spectrum Plot" },

	detectorGrp{	X( *this, .0 ),			Y( *this, .475 ),		W( *this, 1. ),		H( *this, .475 ) },
	detectorTitle{	X( detectorGrp, .0 ),	Y( detectorGrp, 0. ),	W( detectorGrp, 1. ),	H( detectorGrp, .1 ),	"Detector" },
	colPnts{		X( detectorGrp, .0 ),	Y( detectorGrp, .125 ),	W( detectorGrp, .2 ),	H( detectorGrp, .05 ),	"Angles" },
	rowPnts{		X( detectorGrp, .3 ),	Y( detectorGrp, .125 ),	W( detectorGrp, .2 ),	H( detectorGrp, .05 ),	"Pixel" },
	distRange{		X( detectorGrp, .6 ),	Y( detectorGrp, .125 ),	W( detectorGrp, .2 ),	H( detectorGrp, .05 ),	"Range" },

	raysPerPixelIn{ X( detectorGrp, .0 ),	Y( detectorGrp, .25 ),	W( detectorGrp, .2 ),	H( detectorGrp, .05 ),	"Rays / Pix" },
	detector_focus_distance_input{	X( detectorGrp, .25 ),	Y( detectorGrp, .25 ),	W( detectorGrp, .2 ),	H( detectorGrp, .05 ),	"Focus Dist." },
	structureIn{	X( detectorGrp, .75 ),	Y( detectorGrp, .25 ),	W( detectorGrp, .2 ),	H( detectorGrp, .05 ),	"Anti scat." },
	maxRayAngleIn{	X( detectorGrp, .50 ),	Y( detectorGrp, .25 ),	W( detectorGrp, .2 ),	H( detectorGrp, .05 ),	"Max angle" },

	detectorPlot{	X( detectorGrp, .0 ),	Y( detectorGrp, .35 ),	W( detectorGrp, 1. ),	H( detectorGrp, .65 ),	"Detector Plot" },

	updateCB{ *this, &gantryEdition::UpdateGantry }
	
	{

		Fl_Group::box( FL_BORDER_BOX );

		Fl_Group::add( title );
		title.box( FL_NO_BOX ); title.align( FL_ALIGN_CENTER ); title.labelsize( 30 );



		Fl_Group::add( tubeGrp );

		tubeGrp.add( tubeTitle );
		tubeGrp.add( tubeVoltageIn ); tubeGrp.add( tubeCurrentIn ); tubeGrp.add( materialIn );
		tubeGrp.box( FL_BORDER_BOX );

		tubeTitle.box( FL_NO_BOX ); tubeTitle.align( FL_ALIGN_CENTER ); tubeTitle.labelsize( 20 );

		tubeVoltageIn.align( FL_ALIGN_TOP ); tubeCurrentIn.align( FL_ALIGN_TOP ); materialIn.align( FL_ALIGN_TOP );

		tubeVoltageIn.SetProperties( 1., 200000., 0 );
		tubeCurrentIn.SetProperties( .001, 10., 3 );

		tubeVoltageIn.value( xRayTubeParameter.anode_voltage_V );
		tubeCurrentIn.value( xRayTubeParameter.anode_current_A );

		tubeVoltageIn.callback( HandleCallback<gantryEdition>, &updateCB );
		tubeCurrentIn.callback( HandleCallback<gantryEdition>, &updateCB );
		materialIn.callback( HandleCallback<gantryEdition>, &updateCB );

		vector<string> materialNames;
		for( auto& el : XRayTubeProperties::materials ) materialNames.push_back( el.second.first );

		materialIn.AssignElements( materialNames );
		XRayTubeProperties::Material anode_material = xRayTubeParameter.anode_material;
		string materialName = XRayTubeProperties::materials.at( anode_material ).first;
		materialIn.SetCurrentElement( materialName );

		tubeVoltageIn.tooltip( "Voltage in Volts." );
		tubeCurrentIn.tooltip( "Current in Ampere." );
		materialIn.tooltip( "Anode material." );


		tubeGrp.add( spectrumPlot );
		spectrumPlot.Initialise( PROGRAM_STATE().getPath( "spectrumPlot.png" ), "E in keV", "Spec. Pow. in W/keV", PlotLimits{ false, true, NumberRange{ 10., 200. }, NumberRange{ 0., 1. }, 0.001, 1000. }, "", "", false, false );


		//-----------------------------

		Fl_Group::add( detectorGrp );

		detectorGrp.add( detectorTitle );
		detectorTitle.box( FL_NO_BOX ); detectorTitle.align( FL_ALIGN_CENTER ); detectorTitle.labelsize( 20 );


		detectorGrp.add( colPnts ); detectorGrp.add( rowPnts ); detectorGrp.add( distRange );
		detectorGrp.box( FL_BORDER_BOX );
		colPnts.align( FL_ALIGN_TOP ); rowPnts.align( FL_ALIGN_TOP ); distRange.align( FL_ALIGN_TOP );

		colPnts.SetProperties( 3, 10000, 0 );
		colPnts.value( radonParameter.number_of_angles() );

		rowPnts.SetProperties( 3, 10000, 0, Input_Constraints::Odd );
		rowPnts.value( radonParameter.number_of_distances() );

		distRange.SetProperties( 1., 10000., 0 );
		distRange.value( radonParameter.measuring_field_size() );

		colPnts.callback( HandleCallback<gantryEdition>, &updateCB );
		rowPnts.callback( HandleCallback<gantryEdition>, &updateCB );
		distRange.callback( HandleCallback<gantryEdition>, &updateCB );

		colPnts.tooltip( "Amount of angles in Sinogram." );
		rowPnts.tooltip( "Amount of distances in sinogram. Is the amount of detector pixel." );
		distRange.tooltip( "Size of measure field in mm." );


		detectorGrp.add( raysPerPixelIn ); detectorGrp.add( detector_focus_distance_input ); detectorGrp.add( maxRayAngleIn ); detectorGrp.add( structureIn );
		
		detectorGrp.box( FL_BORDER_BOX );
		raysPerPixelIn.align( FL_ALIGN_TOP ); detector_focus_distance_input.align( FL_ALIGN_TOP ); maxRayAngleIn.align( FL_ALIGN_TOP );

		raysPerPixelIn.SetProperties( 1, 1000, 0 );
		raysPerPixelIn.value( (int)  gantryInstance.tube().number_of_rays_per_pixel());

		detector_focus_distance_input.SetProperties( distRange.value(), 100000., 0);
		detector_focus_distance_input.value( physical_detector_properties_.detector_focus_distance );

		maxRayAngleIn.SetProperties( .1, 60., 2 );
		maxRayAngleIn.value( physical_detector_properties_.max_ray_angle_allowed_by_structure / 2. / PI * 360. );

		structureIn.value( (int) physical_detector_properties_.has_anti_scattering_structure );
		structureIn.color( FL_BACKGROUND_COLOR, FL_DARK_GREEN );

		raysPerPixelIn.callback(  HandleCallback<gantryEdition>, &updateCB );
		detector_focus_distance_input.callback( HandleCallback<gantryEdition>, &updateCB );
		maxRayAngleIn.callback(  HandleCallback<gantryEdition>, &updateCB );
		structureIn.callback(  HandleCallback<gantryEdition>, &updateCB  );

		raysPerPixelIn.tooltip( "How many rays will be simulated per pixel." );
		detector_focus_distance_input.tooltip( "Detector focus distance." );
		maxRayAngleIn.tooltip( "Maximum detecable angle in degree between pixel and ray. Only valid when anti scattering is activated." );
		structureIn.tooltip( "Activate anti scattering structure." );


		detectorGrp.add( detectorPlot );
		detectorPlot.Initialise( PROGRAM_STATE().getPath( "detectorPlot.png" ), "x in mm", "y in mm", PlotLimits{ true, true, NumberRange{ 0, 1 }, NumberRange{ 0, 1 } }, "", "", true, true );

		UpdateGantry();
}

gantryEdition::~gantryEdition( void ){
	storedXRayTubeParameter.Save();
	storedRadonParameter.Save();
	storedDetectorParameter.Save();
}

void gantryEdition::SetDistances( const double max_corner_distance ){
	detector_focus_distance_input.value( 2. * ( max_corner_distance + 150. ) );
	distRange.value( 2. * ( max_corner_distance + 25. ) );

	UpdateGantry();
}

void gantryEdition::UpdateGantry( void ){


		Fl_Group::window()->deactivate();

		
		detector_focus_distance_input.SetProperties( distRange.value(), 10000., 0 );

		xRayTubeParameter = XRayTubeProperties{ tubeVoltageIn.value(), tubeCurrentIn.value(), XRayTubeProperties::GetMaterialEnum( materialIn.current_element() ), (size_t) raysPerPixelIn.value() };
		radonParameter = ProjectionsProperties{ colPnts.value(), rowPnts.value(), distRange.value() };
		physical_detector_properties_ = PhysicalDetectorProperties{ 5., detector_focus_distance_input.value(), (bool) structureIn.value(), maxRayAngleIn.value() / 360. * 2. * PI };
		
		storedXRayTubeParameter.SetAsLoaded();
		storedRadonParameter.SetAsLoaded();
		storedDetectorParameter.SetAsLoaded();

		rowPnts.value( radonParameter.number_of_distances() );
		colPnts.value( radonParameter.number_of_angles() );


	
		gantryInstance.UpdateTubeAndDetectorProperties( xRayTubeParameter, radonParameter, physical_detector_properties_ );



		const XRayTube tubeRef = gantryInstance.tube();
		const XRayDetector detectorRef = gantryInstance.detector();

		VectorPair spectrum_points = tubeRef.GetEnergySpectrumPoints();
		for( auto& spectrum_value : spectrum_points.second ){
			spectrum_value /= tubeRef.GetSpectralEnergyResolution();		// "Convert" to integral to match power
		}

		spectrumPlot.plot().AssignData( spectrum_points );
		spectrumPlot.AssignData();

		main_window_.tomographyExecution.updateInformation( radonParameter, detectorRef.properties(), gantryInstance.tube() );

		detectorPlot.plot().ResetObjects();

		const auto allPixel = detectorRef.pixel_array();

		for( const auto& pixel : allPixel ){

			const Point3D startP = pixel.GetPoint( pixel.parameter_1_min(), 0. ).ConvertTo( gantryInstance.coordinate_system() );
			const Point3D endP = pixel.GetPoint( pixel.parameter_1_max(), 0. ).ConvertTo( gantryInstance.coordinate_system() );

			const Tuple2D start{ startP.X(), startP.Y() };
			const Tuple2D end{ endP.X(), endP.Y() };

			detectorPlot.plot().AddLine( start, end );

		}

		const Point3D gantryCenter = gantryInstance.GetCenter();
		detectorPlot.plot().AddPoint( Tuple2D( gantryCenter.X(), gantryCenter.Y() ) );
		detectorPlot.plot().CreatePlot();
		detectorPlot.AssignData();

		Fl_Group::window()->activate();


}
