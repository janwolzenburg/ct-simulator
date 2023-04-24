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
#include "guiPlots.h"

/*********************************************************************
  Implementations
*********************************************************************/



gantryEdition::gantryEdition( int x, int y, int w, int h ) :
	Fl_Group{ x, y, w, h },
	tubeGrp{ X( *this, .05 ),	Y( *this, .05 ),	W( *this, .5 ),		H( *this, .1 ) },
	tubeVoltageIn{ X( tubeGrp, .0 ),	Y( tubeGrp, .2 ),	W( tubeGrp, .4 ),	H( tubeGrp, .35 ),	"Voltage" },
	tubeCurrentIn{ X( tubeGrp, .6 ),	Y( tubeGrp, .2 ),	W( tubeGrp, .4 ),	H( tubeGrp, .35 ),	"Current" },
	materialIn{ X( tubeGrp, .0 ),	Y( tubeGrp, .75 ),	W( tubeGrp, 1. ),	H( tubeGrp, .25 ),	"Material" },


	radonGrp{ X( *this, .2 ),		vOff( tubeGrp ) + Y( *this, .05 ),		W( *this, .12 ),	H( *this, .1 ) },
	colPnts{ X( radonGrp, .0 ),	Y( radonGrp, 0. ),	W( radonGrp, 1. ),	H( radonGrp, .25 ),	"Angles" },
	rowPnts{ X( radonGrp, .0 ),	Y( radonGrp, .375 ),	W( radonGrp, 1. ),	H( radonGrp, .25 ),	"Pixel" },
	distRange{ X( radonGrp, .0 ),	Y( radonGrp, .75 ),	W( radonGrp, 1. ),	H( radonGrp, .25 ),	"Range" },

	detectorGrp{ X( *this, .6 ),			vOff( tubeGrp ) + Y( *this, .05 ),			W( *this, .4 ),			H( *this, .1 ) },
	raysPerPixelIn{ X( detectorGrp, .0 ),	Y( detectorGrp, 0. ),	W( detectorGrp, .5 ),	H( detectorGrp, .25 ),	"Rays / Pix" },
	arcRadiusIn{ X( detectorGrp, .0 ),	Y( detectorGrp, .375 ),	W( detectorGrp, .5 ),	H( detectorGrp, .25 ),	"Arc Radius" },
	maxRayAngleIn{ X( detectorGrp, .0 ),	Y( detectorGrp, .75 ),	W( detectorGrp, .3 ),	H( detectorGrp, .25 ),	"Max. angle" },
	structureIn{ X( detectorGrp, .5 ),	Y( detectorGrp, .75 ),	W( detectorGrp, .5 ),	H( detectorGrp, .25 ),	"Anti scat." },

	specView{ X( *this, 0. ),			vOff( detectorGrp ) + Y( *this, .025 ),			W( *this, 1. ),			H( *this, .2 ) },
	spectrumPlot{ X( specView, .0 ),	Y( specView, 0. ),	W( specView, 1. ),	H( specView, 1. ),	"Spectrum Plot" },

	detectorView{ X( *this, 0. ),			vOff( specView ) + Y( *this, .025 ),			W( *this, 1. ),			H( *this, .3 ) },
	detectorPlot{ X( detectorView, .0 ),	Y( detectorView, 0. ),	W( detectorView, 1. ),	H( detectorView, 1. ),	"Detector Plot" },

	updateGantry( false )
	
	{
		Fl_Group::box( FL_BORDER_BOX );

		Fl_Group::add( tubeGrp );

		tubeGrp.add( tubeVoltageIn ); tubeGrp.add( tubeCurrentIn ); tubeGrp.add( materialIn );
		tubeGrp.box( FL_BORDER_BOX );

		tubeVoltageIn.align( FL_ALIGN_TOP ); tubeCurrentIn.align( FL_ALIGN_TOP ); materialIn.align( FL_ALIGN_TOP );

		tubeVoltageIn.setProperties( 1., 200000., 0 );
		tubeCurrentIn.setProperties( .001, 10., 3 );

		tubeVoltageIn.value( PROGRAM_STATE().TubeParameter().anodeVoltage_V );
		tubeCurrentIn.value( PROGRAM_STATE().TubeParameter().anodeCurrent_A );

		tubeVoltageIn.callback( button_cb, &updateGantry );
		tubeCurrentIn.callback( button_cb, &updateGantry );
		materialIn.callback( button_cb, &updateGantry );

		vector<string> materialNames;
		for( auto& el : tubeParameter::material ) materialNames.push_back( el.second.first );

		materialIn.setElements( materialNames );
		MATERIAL anodeMaterial = PROGRAM_STATE().TubeParameter().anodeMaterial;
		string materialName = tubeParameter::material.at( anodeMaterial ).first;
		materialIn.value( materialName );

		tubeVoltageIn.tooltip( "Voltage in Volts." );
		tubeCurrentIn.tooltip( "Current in Ampere." );
		materialIn.tooltip( "Anode material." );


		Fl_Group::add( radonGrp ); radonGrp.add( colPnts ); radonGrp.add( rowPnts ); radonGrp.add( distRange );
		radonGrp.box( FL_BORDER_BOX );
		colPnts.align( FL_ALIGN_LEFT ); rowPnts.align( FL_ALIGN_LEFT ); distRange.align( FL_ALIGN_LEFT );

		colPnts.setProperties( 3, 10000, 0 );
		colPnts.value( PROGRAM_STATE().RadonParameter().numberPoints.col );

		rowPnts.setProperties( 3, 10000, 0, INPUT_CONSTRAINTS::ODD );
		rowPnts.value( PROGRAM_STATE().RadonParameter().numberPoints.row );

		distRange.setProperties( 1., 10000., 0 );
		distRange.value( PROGRAM_STATE().RadonParameter().distanceRange );

		colPnts.callback( button_cb, &updateGantry );
		rowPnts.callback( button_cb, &updateGantry );
		distRange.callback( button_cb, &updateGantry );

		colPnts.tooltip( "Amount of angles in Sinogram." );
		rowPnts.tooltip( "Amount of distances in sinogram. Is the amount of detector pixel." );
		distRange.tooltip( "Size of measure field in mm." );


		Fl_Group::add( detectorGrp ); detectorGrp.add( raysPerPixelIn ); detectorGrp.add( arcRadiusIn ); detectorGrp.add( maxRayAngleIn ); detectorGrp.add( structureIn );
		detectorGrp.box( FL_BORDER_BOX );
		raysPerPixelIn.align( FL_ALIGN_LEFT ); arcRadiusIn.align( FL_ALIGN_LEFT ); maxRayAngleIn.align( FL_ALIGN_LEFT );

		raysPerPixelIn.setProperties( 1, 1000, 0 );
		raysPerPixelIn.value( (int) PROGRAM_STATE().DetectorParameter().raysPerPixel );

		arcRadiusIn.setProperties( 100., 100000., 0 );
		arcRadiusIn.value( PROGRAM_STATE().DetectorParameter().arcRadius );

		maxRayAngleIn.setProperties( .1, 60., 1 );
		maxRayAngleIn.value( PROGRAM_STATE().DetectorParameter().maxRayAngleDetectable );

		structureIn.value( (int) PROGRAM_STATE().DetectorParameter().structured );

		raysPerPixelIn.callback( button_cb, &updateGantry );
		arcRadiusIn.callback( button_cb, &updateGantry );
		maxRayAngleIn.callback( button_cb, &updateGantry );
		structureIn.callback( button_cb, &updateGantry );

		raysPerPixelIn.tooltip( "How many rays will be simulated per pixel." );
		arcRadiusIn.tooltip( "Radius of the arc where the pixel lie." );
		maxRayAngleIn.tooltip( "Maximum detecable angle between pixel and ray. " );
		structureIn.tooltip( "Activate anti scattering structure." );


		Fl_Group::add( specView ); specView.add( spectrumPlot );
		spectrumPlot.initializePlot( PROGRAM_STATE().getPath( "spectrumPlot.png" ), "E in keV", "Spec. Pow. in W/keV", plotLimits{ false, true, range{ 10., 200. }, range{ 0., 1. }, 0.001, 1000. }, "", "", false, false );

		Fl_Group::add( detectorView ); detectorView.add( detectorPlot );
		detectorPlot.initializePlot( PROGRAM_STATE().getPath( "detectorPlot.png" ), "x in mm", "y in mm", plotLimits{ true, true, range{ 0, 1 }, range{ 0, 1 } }, "", "", true, true );


}



void gantryEdition::handleEvents( void ){

	if( UpdateGantry() ){


		Fl_Group::window()->deactivate();

		tubeParameter newTubeParameter{ tubeVoltageIn.current, tubeCurrentIn.current, tubeParameter::getEnum( materialIn.value() ) };
		detectorRadonParameter newRadonParameter{ idx2CR{ colPnts.value(), rowPnts.value() }, distRange.value() };
		detectorIndipendentParameter newDetectorParameter{ (size_t) raysPerPixelIn.value(), arcRadiusIn.value(), 5., (bool) structureIn.value(), maxRayAngleIn.value() };

		rowPnts.value( newRadonParameter.numberPoints.row );
		colPnts.value( newRadonParameter.numberPoints.col );


		PROGRAM_STATE().buildGantry( newTubeParameter, newRadonParameter, newDetectorParameter );

		const tube& tubeRef = PROGRAM_STATE().Gantry().Tube();
		const detector& detectorRef = PROGRAM_STATE().Gantry().Detector();

		spectrumPlot.plotRef().assignData( tubeRef.spectrumPoints( true, true ) );
		spectrumPlot.assignData();



		detectorPlot.plotRef().resetObjects();

		const auto allPixel = detectorRef.getPixel();

		for( const auto& pixel : allPixel ){

			const pnt3 startP = pixel.getPnt( pixel.AMin(), 0. ).convertTo( PROGRAM_STATE().Gantry().CSys() );
			const pnt3 endP = pixel.getPnt( pixel.AMax(), 0. ).convertTo( PROGRAM_STATE().Gantry().CSys() );

			const v2 start = v2( startP.X(), startP.Y() );
			const v2 end = v2( endP.X(), endP.Y() );

			detectorPlot.plotRef().addLine( start, end );

		}

		const pnt3 gantryCenter = PROGRAM_STATE().Gantry().Center();
		detectorPlot.plotRef().addPoint( v2( gantryCenter.X(), gantryCenter.Y() ) );
		detectorPlot.plotRef().create();
		detectorPlot.assignData();

		Fl_Group::window()->activate();

	}

}