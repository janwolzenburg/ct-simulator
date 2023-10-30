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
#include "plots.h"



/*********************************************************************
  Implementations
*********************************************************************/



gantryEdition::gantryEdition( int x, int y, int w, int h ) :
	Fl_Group{ x, y, w, h },
	title{			X( *this, 0. ),		Y( *this, 0. ),		W( *this, 1. ),		H( *this, 0.05 ),	"Gantry" },

	tubeGrp{		X( *this, .0 ),	vOff( title ) + Y( *this, .02 ),	W( *this, 1. ),		H( *this, .3 ) },
	tubeTitle{		X( tubeGrp, 0. ),	Y( tubeGrp, 0. ),	W( tubeGrp, 1. ),	H( tubeGrp, .075 ),	"xRay Tube" },
	tubeVoltageIn{	X( tubeGrp, .0 ),	Y( tubeGrp, .15 ),	W( tubeGrp, .15 ),	H( tubeGrp, .1 ),	"Voltage" },
	tubeCurrentIn{	X( tubeGrp, .25 ),	Y( tubeGrp, .15 ),	W( tubeGrp, .15 ),	H( tubeGrp, .1 ),	"Current" },
	materialIn{		X( tubeGrp, .5 ),	Y( tubeGrp, .15 ),	W( tubeGrp, .5 ),	H( tubeGrp, .1 ),	"Material" },
	spectrumPlot{	X( tubeGrp, .0 ),	Y( tubeGrp, .325 ),	W( tubeGrp, 1. ),	H( tubeGrp, .725 ),	"Spectrum Plot" },

	detectorGrp{	X( *this, .0 ),			vOff( tubeGrp ) + Y( *this, .03 ),			W( *this, 1. ),		H( *this, .6 ) },
	detectorTitle{	X( detectorGrp, .0 ),	Y( detectorGrp, 0. ),	W( detectorGrp, 1. ),	H( detectorGrp, .1 ),	"Detector" },
	colPnts{		X( detectorGrp, .0 ),	Y( detectorGrp, .125 ),	W( detectorGrp, .2 ),	H( detectorGrp, .05 ),	"Angles" },
	rowPnts{		X( detectorGrp, .3 ),	Y( detectorGrp, .125 ),	W( detectorGrp, .2 ),	H( detectorGrp, .05 ),	"Pixel" },
	distRange{		X( detectorGrp, .6 ),	Y( detectorGrp, .125 ),	W( detectorGrp, .2 ),	H( detectorGrp, .05 ),	"Range" },

	raysPerPixelIn{ X( detectorGrp, .0 ),	Y( detectorGrp, .25 ),	W( detectorGrp, .2 ),	H( detectorGrp, .05 ),	"Rays / Pix" },
	detector_focus_distance_input{	X( detectorGrp, .25 ),	Y( detectorGrp, .25 ),	W( detectorGrp, .2 ),	H( detectorGrp, .05 ),	"Focus Dist." },
	structureIn{	X( detectorGrp, .75 ),	Y( detectorGrp, .25 ),	W( detectorGrp, .2 ),	H( detectorGrp, .05 ),	"Anti scat." },
	maxRayAngleIn{	X( detectorGrp, .50 ),	Y( detectorGrp, .25 ),	W( detectorGrp, .2 ),	H( detectorGrp, .05 ),	"Max angle" },

	detectorPlot{	X( detectorGrp, .0 ),	Y( detectorGrp, .35 ),	W( detectorGrp, 1. ),	H( detectorGrp, .65 ),	"Detector Plot" },

	updateGantry( false )
	
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

		tubeVoltageIn.setProperties( 1., 200000., 0 );
		tubeCurrentIn.setProperties( .001, 10., 3 );

		tubeVoltageIn.value( PROGRAM_STATE().TubeParameter().anode_voltage_V );
		tubeCurrentIn.value( PROGRAM_STATE().TubeParameter().anode_current_A );

		tubeVoltageIn.callback( button_cb, &updateGantry );
		tubeCurrentIn.callback( button_cb, &updateGantry );
		materialIn.callback( button_cb, &updateGantry );

		vector<string> materialNames;
		for( auto& el : XRayTubeProperties::materials ) materialNames.push_back( el.second.first );

		materialIn.setElements( materialNames );
		XRayTubeProperties::Material anode_material = PROGRAM_STATE().TubeParameter().anode_material;
		string materialName = XRayTubeProperties::materials.at( anode_material ).first;
		materialIn.value( materialName );

		tubeVoltageIn.tooltip( "Voltage in Volts." );
		tubeCurrentIn.tooltip( "Current in Ampere." );
		materialIn.tooltip( "Anode material." );


		tubeGrp.add( spectrumPlot );
		spectrumPlot.initialisePlot( PROGRAM_STATE().getPath( "spectrumPlot.png" ), "E in keV", "Spec. Pow. in W/keV", plotLimits{ false, true, NumberRange{ 10., 200. }, NumberRange{ 0., 1. }, 0.001, 1000. }, "", "", false, false );


		//-----------------------------

		Fl_Group::add( detectorGrp );

		detectorGrp.add( detectorTitle );
		detectorTitle.box( FL_NO_BOX ); detectorTitle.align( FL_ALIGN_CENTER ); detectorTitle.labelsize( 20 );


		detectorGrp.add( colPnts ); detectorGrp.add( rowPnts ); detectorGrp.add( distRange );
		detectorGrp.box( FL_BORDER_BOX );
		colPnts.align( FL_ALIGN_TOP ); rowPnts.align( FL_ALIGN_TOP ); distRange.align( FL_ALIGN_TOP );

		colPnts.setProperties( 3, 10000, 0 );
		colPnts.value( PROGRAM_STATE().RadonParameter().number_of_angles() );

		rowPnts.setProperties( 3, 10000, 0, INPUT_CONSTRAINTS::ODD );
		rowPnts.value( PROGRAM_STATE().RadonParameter().number_of_distances() );

		distRange.setProperties( 1., 10000., 0 );
		distRange.value( PROGRAM_STATE().RadonParameter().measuring_field_size() );

		colPnts.callback( button_cb, &updateGantry );
		rowPnts.callback( button_cb, &updateGantry );
		distRange.callback( button_cb, &updateGantry );

		colPnts.tooltip( "Amount of angles in Sinogram." );
		rowPnts.tooltip( "Amount of distances in sinogram. Is the amount of detector pixel." );
		distRange.tooltip( "Size of measure field in mm." );


		detectorGrp.add( raysPerPixelIn ); detectorGrp.add( detector_focus_distance_input ); detectorGrp.add( maxRayAngleIn ); detectorGrp.add( structureIn );
		
		detectorGrp.box( FL_BORDER_BOX );
		raysPerPixelIn.align( FL_ALIGN_TOP ); detector_focus_distance_input.align( FL_ALIGN_TOP ); maxRayAngleIn.align( FL_ALIGN_TOP );

		raysPerPixelIn.setProperties( 1, 1000, 0 );
		raysPerPixelIn.value( (int) PROGRAM_STATE().Tube().number_of_rays_per_pixel() );

		detector_focus_distance_input.setProperties( distRange.value(), 100000., 0);
		detector_focus_distance_input.value( PROGRAM_STATE().DetectorParameter().detector_focus_distance );

		maxRayAngleIn.setProperties( .1, 60., 2 );
		maxRayAngleIn.value( PROGRAM_STATE().DetectorParameter().max_ray_angle_allowed_by_structure / 2. / PI * 360. );

		structureIn.value( (int) PROGRAM_STATE().DetectorParameter().has_anti_scattering_structure );
		structureIn.color( FL_BACKGROUND_COLOR, FL_DARK_GREEN );

		raysPerPixelIn.callback( button_cb, &updateGantry );
		detector_focus_distance_input.callback( button_cb, &updateGantry );
		maxRayAngleIn.callback( button_cb, &updateGantry );
		structureIn.callback( button_cb, &updateGantry );

		raysPerPixelIn.tooltip( "How many rays will be simulated per pixel." );
		detector_focus_distance_input.tooltip( "Detector focus distance." );
		maxRayAngleIn.tooltip( "Maximum detecable angle in degree between pixel and ray. Only valid when anti scattering is activated." );
		structureIn.tooltip( "Activate anti scattering structure." );


		detectorGrp.add( detectorPlot );
		detectorPlot.initialisePlot( PROGRAM_STATE().getPath( "detectorPlot.png" ), "x in mm", "y in mm", plotLimits{ true, true, NumberRange{ 0, 1 }, NumberRange{ 0, 1 } }, "", "", true, true );


}

void gantryEdition::handleEvents( void ){

	if( UpdateGantry() ){


		Fl_Group::window()->deactivate();

		
		detector_focus_distance_input.setProperties( distRange.value(), 10000., 0 );

		XRayTubeProperties newTubeParameter{ tubeVoltageIn.value(), tubeCurrentIn.value(), XRayTubeProperties::GetMaterialEnum( materialIn.value() ), (size_t) raysPerPixelIn.value() };
		ProjectionsProperties newRadonParameter{ colPnts.value(), rowPnts.value(), distRange.value() };
		PhysicalDetectorProperties newDetectorParameter{ 5., detector_focus_distance_input.value(), (bool) structureIn.value(), maxRayAngleIn.value() / 360. * 2. * PI };


		rowPnts.value( newRadonParameter.number_of_distances() );
		colPnts.value( newRadonParameter.number_of_angles() );


		PROGRAM_STATE().buildGantry( newTubeParameter, newRadonParameter, newDetectorParameter );

		const XRayTube tubeRef = PROGRAM_STATE().gantry().tube();
		const XRayDetector detectorRef = PROGRAM_STATE().gantry().detector();

		VectorPair spectrum_points = tubeRef.GetEnergySpectrumPoints();
		for( auto& spectrum_value : spectrum_points.second ){
			spectrum_value /= tubeRef.GetSpectralEnergyResolution();		// "Convert" to integral to match power
		}

		spectrumPlot.plotRef().assignData( spectrum_points );
		spectrumPlot.assignData();

		PROGRAM_STATE().setUpdateInformationFlag();

		detectorPlot.plotRef().resetObjects();

		const auto allPixel = detectorRef.pixel_array();

		for( const auto& pixel : allPixel ){

			const Point3D startP = pixel.GetPoint( pixel.parameter_1_min(), 0. ).ConvertTo( PROGRAM_STATE().gantry().coordinate_system() );
			const Point3D endP = pixel.GetPoint( pixel.parameter_1_max(), 0. ).ConvertTo( PROGRAM_STATE().gantry().coordinate_system() );

			const Tuple2D start{ startP.X(), startP.Y() };
			const Tuple2D end{ endP.X(), endP.Y() };

			detectorPlot.plotRef().addLine( start, end );

		}

		const Point3D gantryCenter = PROGRAM_STATE().gantry().GetCenter();
		detectorPlot.plotRef().addPoint( Tuple2D( gantryCenter.X(), gantryCenter.Y() ) );
		detectorPlot.plotRef().create();
		detectorPlot.assignData();

		Fl_Group::window()->activate();

	}

}