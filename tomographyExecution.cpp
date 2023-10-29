/******************************************************************
* @file   tomographyExecution.cpp
* @brief  Implementations
*
* @author Jan Wolzenburg
* @date   September 2023
* ********************************************************************/



/*********************************************************************
  Includes
*********************************************************************/
#include "tomographyExecution.h"

#include "processingWindow.h"
#include "backprojection.h"
#include "progress.h"
#include "widgets.h"


tomographyExec::tomographyExec( int x, int y, int w, int h ) :
	Fl_Group{ x, y, w, h },
	title{ X( *this, 0. ), Y( *this, 0. ), W( *this, 1. ), H( *this, .05 ), "Tomography"},

	tomoParameterGrp{		X( *this, .0 ),				Y( *this, .1 ),				W( *this, 1. ),				H( *this, .6 ) },
	parameterTitle{			X( tomoParameterGrp, 0. ),	Y( tomoParameterGrp, 0. ),	W( tomoParameterGrp, 1. ),	H( tomoParameterGrp, .05 ), "Parameter" },
	
	exposureTimeIn{			X( tomoParameterGrp, .1 ),	Y( tomoParameterGrp, .08725 ),	W( tomoParameterGrp, .1 ),	H( tomoParameterGrp, .05 ), "Exposure time" },
	rayStepSizeIn{			X( tomoParameterGrp, .33 ),	Y( tomoParameterGrp, .08725 ),	W( tomoParameterGrp, .15 ),	H( tomoParameterGrp, .05 ), "Ray step size" },
	
	radiationLoopsIn{		X( tomoParameterGrp, 0. ),	Y( tomoParameterGrp, .175 ),	W( tomoParameterGrp, .35 ),	H( tomoParameterGrp, .05 ), "Maximum loops" },
	scatterPropabilityIn{	X( tomoParameterGrp, .45 ),	Y( tomoParameterGrp, .175 ),	W( tomoParameterGrp, .15 ),	H( tomoParameterGrp, .05 ), "Scattering prob." },
	scatteringOnOff{		X( tomoParameterGrp, .75 ),	Y( tomoParameterGrp, .175 ),	W( tomoParameterGrp, .15 ),	H( tomoParameterGrp, .05 ), "Scattering" },
	
	information{			X( tomoParameterGrp, 0.2 ),	Y( tomoParameterGrp, .35 ),	W( tomoParameterGrp, .8 ),	H( tomoParameterGrp, .6 ), "Information" },


	controlGrp{				X( *this, .0 ), vOff( tomoParameterGrp ), W( *this, 1. ), H( *this, .1 ) },
	radiationButton{		X( controlGrp, .25 ), Y( controlGrp, .1 ), W( controlGrp, .5 ), H( controlGrp, .4 ), "Record Slice" },
	exportButton{			X( controlGrp, .65 ), Y( controlGrp, .6 ), W( controlGrp, .4 ), H( controlGrp, .4 ), "Export Sinogram" },

	
	radiateFlag( false ),
	updateFlag( false ),
	exportFlag( false ),
	informationUpdateFlag( false )

{
	Fl_Group::box( FL_BORDER_BOX );
	Fl_Group::add( title );
	title.box( FL_NO_BOX ); title.align( FL_ALIGN_CENTER ); title.labelsize( 30 );

	Fl_Group::add( tomoParameterGrp );
	tomoParameterGrp.box( FL_BORDER_BOX );

	tomoParameterGrp.add( parameterTitle );
	tomoParameterGrp.add( exposureTimeIn );
	tomoParameterGrp.add( rayStepSizeIn );

	tomoParameterGrp.add( radiationLoopsIn );
	tomoParameterGrp.add( scatterPropabilityIn );
	tomoParameterGrp.add( scatteringOnOff );
	tomoParameterGrp.add( information );


	parameterTitle.box( FL_NO_BOX ); parameterTitle.align( FL_ALIGN_CENTER ); parameterTitle.labelsize( 20 );

	exposureTimeIn.align( FL_ALIGN_TOP );
	rayStepSizeIn.align( FL_ALIGN_TOP );
	radiationLoopsIn.align( FL_ALIGN_TOP );
	scatterPropabilityIn.align( FL_ALIGN_TOP );


	exposureTimeIn.setProperties( 0.001, 10., 3 );
	rayStepSizeIn.setProperties( 0.001, 10., 3 );
	scatterPropabilityIn.setProperties( 0., 1., 3 );


	exposureTimeIn.tooltip( "Exposure time in seconds." );
	rayStepSizeIn.tooltip( "Step size of ray tracing in mm." );
	radiationLoopsIn.tooltip( "Maximum amount of iterations for ray tracing. How often a ray can be scattered." );
	scatterPropabilityIn.tooltip( "Probability that a ray will be scattered once when going through complete model." );
	scatteringOnOff.tooltip( "Enable or disable scattering." );

	
	programState& state = PROGRAM_STATE();

	exposureTimeIn.value( state.tomographyParamerters.exposure_time );
	rayStepSizeIn.value( state.tomographyParamerters.ray_step_length );
	radiationLoopsIn.value( (double) state.tomographyParamerters.max_scattering_occurrences );
	scatterPropabilityIn.value( state.tomographyParamerters.scatter_propability );
	scatteringOnOff.value( state.tomographyParamerters.scattering_enabled );
	scatteringOnOff.color( FL_BACKGROUND_COLOR, FL_DARK_GREEN );

	exposureTimeIn.callback( button_cb, &updateFlag );
	rayStepSizeIn.callback( button_cb, &updateFlag );
	radiationLoopsIn.callback( button_cb, &updateFlag );
	scatterPropabilityIn.callback( button_cb, &updateFlag );
	scatteringOnOff.callback( button_cb, &updateFlag );


	Fl_Group::add( controlGrp );
	controlGrp.box( FL_BORDER_BOX );

	controlGrp.add( radiationButton );
	controlGrp.add( exportButton );

	exportButton.deactivate();

	radiationButton.callback( button_cb, &radiateFlag );
	exportButton.callback( button_cb, &exportFlag );

	this->deactivate();
}

void tomographyExec::handleEvents( void ){

	programState& state = PROGRAM_STATE();

	if( state.ModelLoaded() && !this->active() )
		this->activate();
	else if( !state.ModelLoaded() )
		this->deactivate();
	
	if( state.RadonTransformedLoaded() )
		exportButton.activate();
	else
		exportButton.deactivate();
	
	if(  UnsetFlag( radiateFlag ) ){

		state.deactivateAll();

		Fl_Progress_Window* radiationProgressWindow = nullptr;
		if( state.mainWindow_ != nullptr )
			radiationProgressWindow = new Fl_Progress_Window{ (Fl_Window*) state.mainWindow_, 20, 5, "Radiation progress" };
		

		state.tomographyInstance = Tomography{ state.tomographyParamerters };

		if( radiationProgressWindow != nullptr ){
			state.assignRadonTransformed( state.tomographyInstance.RecordSlice( state.RadonParameter(), state.gantry(), state.model(), 0, radiationProgressWindow ) );
			delete radiationProgressWindow;
		}

		if( state.processingWindow_ != nullptr )
			state.processingWindow_->setNewRTFlag();

		state.activateAll();
	}

	if( UnsetFlag( exportFlag ) )
		state.exportSinogram();
	

	if( UnsetFlag( updateFlag )){
		informationUpdateFlag = true;
		state.tomographyParamerters = TomographyProperties{ exposureTimeIn.value(), (bool) scatteringOnOff.value(), (size_t) radiationLoopsIn.value(), scatterPropabilityIn.value(), rayStepSizeIn.value() };
	}

	if( UnsetFlag( informationUpdateFlag )){

		string informationString;

		informationString += "Sinogramgröße:      " + ToString( state.RadonParameter().numberPoints.c ) + " x " + ToString( state.RadonParameter().numberPoints.r ) + '\n';
		informationString += "Sinogramauflösung:  " + ToString( state.RadonParameter().resolution.c / 2. / PI * 360.,2 ) + "° x " + ToString( state.RadonParameter().resolution.r, 2 ) + " mm" + '\n' + '\n';
		informationString += "Gantryrotationen:   " + ToString( state.RadonParameter().framesToFillSinogram ) + '\n';
		informationString += "Detektorwinkel:	  " + ToString( state.DetectorPhysicalParameter().arc_angle / 2. / PI * 360., 2 ) + "°" + '\n';


		informationString += "Elektrische Leistung:	  " + ToString( state.Tube().GetElectricalPower() ) + "W" + '\n';
		informationString += "Strahlleistung:	  " + ToString( state.Tube().GetEmittedBeamPower() ) + "W" + '\n';
		informationString += "Strahlenergie:	  " + ToString( state.Tube().GetEmittedEnergy( state.tomographyParamerters.exposure_time ) ) + "J" + '\n';
		informationString += "Strahlenergie gesamt:	  " + ToString( state.Tube().GetEmittedEnergy( state.tomographyParamerters.exposure_time ) * static_cast<double>( state.RadonParameter().framesToFillSinogram ) ) + "J" + '\n';
		information.value( informationString.c_str() );

	}
}