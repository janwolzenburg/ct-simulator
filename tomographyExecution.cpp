#include "tomographyExecution.h"


#include "processingWindow.h"
#include "backprojection.h"
#include "progress.h"

tomographyExec::tomographyExec( int x, int y, int w, int h ) :
	Fl_Group( x, y, w, h ),
	title( X( *this, 0. ), Y( *this, 0. ), W( *this, 1. ), H( *this, .05 ), "Tomography"),

	tomoParameterGrp(		X( *this, .0 ),				Y( *this, .0 ),				W( *this, 1. ),				H( *this, .3 ) ),
	parameterTitle(			X( tomoParameterGrp, 0. ),	Y( tomoParameterGrp, 0. ),	W( tomoParameterGrp, 1. ),	H( tomoParameterGrp, .1 ), "Parameter" ),
	
	exposureTimeIn(			X( tomoParameterGrp, 0. ),	Y( tomoParameterGrp, .15 ),	W( tomoParameterGrp, .2 ),	H( tomoParameterGrp, .15 ), "Exposure time" ),
	rayStepSizeIn(			X( tomoParameterGrp, .5 ),	Y( tomoParameterGrp, .15 ),	W( tomoParameterGrp, .2 ),	H( tomoParameterGrp, .15 ), "Ray step size" ),
	
	radiationLoopsIn(		X( tomoParameterGrp, 0. ),	Y( tomoParameterGrp, .25 ),	W( tomoParameterGrp, .2 ),	H( tomoParameterGrp, .15 ), "Maximum loops" ),
	scatterPropabilityIn(	X( tomoParameterGrp, .33 ),	Y( tomoParameterGrp, .25 ),	W( tomoParameterGrp, .2 ),	H( tomoParameterGrp, .15 ), "Scattering prob." ),
	scatteringOnOff(		X( tomoParameterGrp, .66 ),	Y( tomoParameterGrp, .25 ),	W( tomoParameterGrp, .2 ),	H( tomoParameterGrp, .15 ), "Scattering" ),
	
	information(			X( tomoParameterGrp, .5 ),	Y( tomoParameterGrp, .6 ),	W( tomoParameterGrp, 1. ),	H( tomoParameterGrp, .4 ), "Information" ),


	controlGrp( X( *this, .05 ), vOff( tomoParameterGrp ) + Y( *this, .05 ), W( *this, .9 ), H( *this, .1 ) ),
	radiationButton( X( controlGrp, .25 ), Y( controlGrp, 0. ), W( controlGrp, .5 ), H( controlGrp, .5 ), "Record Slice" ),

	radiateFlag( false ),
	updateFlag( false )

{
	Fl_Group::box( FL_BORDER_BOX );
	Fl_Group::add( title );
	title.box( FL_NO_BOX ); title.align( FL_ALIGN_CENTER ); title.labelsize( 30 );

	Fl_Group::add( tomoParameterGrp );
	tomoParameterGrp.box( FL_BORDER_BOX );

	tomoParameterGrp.add( exposureTimeIn );
	tomoParameterGrp.add( rayStepSizeIn );

	tomoParameterGrp.add( radiationLoopsIn );
	tomoParameterGrp.add( scatterPropabilityIn );
	tomoParameterGrp.add( scatteringOnOff );
	tomoParameterGrp.add( information );

	exposureTimeIn.align( FL_ALIGN_TOP );
	rayStepSizeIn.align( FL_ALIGN_TOP );
	radiationLoopsIn.align( FL_ALIGN_TOP );
	scatterPropabilityIn.align( FL_ALIGN_TOP );


	exposureTimeIn.setProperties( 0.001, 10., 3 );
	rayStepSizeIn.setProperties( 0.001, 10., 3 );
	scatterPropabilityIn.setProperties( 0., 1., 3 );


	exposureTimeIn.value( PROGRAM_STATE().TomographyParameter().ExposureTime() );
	rayStepSizeIn.value( PROGRAM_STATE().TomographyParameter().RayStepSize() );
	radiationLoopsIn.value( (double) PROGRAM_STATE().TomographyParameter().MaxLoops() );
	scatterPropabilityIn.value( PROGRAM_STATE().TomographyParameter().ScatterPropability() );
	scatteringOnOff.value( PROGRAM_STATE().TomographyParameter().Scattering() );

	exposureTimeIn.callback( button_cb, &updateFlag );
	rayStepSizeIn.callback( button_cb, &updateFlag );
	radiationLoopsIn.callback( button_cb, &updateFlag );
	scatterPropabilityIn.callback( button_cb, &updateFlag );
	scatteringOnOff.callback( button_cb, &updateFlag );


	Fl_Group::add( controlGrp );
	controlGrp.box( FL_BORDER_BOX );

	controlGrp.add( radiationButton );
	radiationButton.callback( button_cb, &radiateFlag );

	this->deactivate();
}

tomographyExec::~tomographyExec( void ){

}


void tomographyExec::handleEvents( void ){

	programState& state = PROGRAM_STATE();

	if( state.ModelLoaded() && !this->active() ) this->activate();
	else if( !state.ModelLoaded() ) this->deactivate();

	if( radiateFlag ){


		Fl_Group::window()->deactivate();

		radiateFlag = false;

		Fl_Progress_Window* radiationProgressWindow = new Fl_Progress_Window( (Fl_Window*) PROGRAM_STATE().MainWindow(), 20, 5, "Radiation progress" );

		state.Tomography() = tomography( state.TomographyParameter() );
		state.assignRadonTransformed( state.Tomography().recordSlice( state.Gantry(), state.Model(), 0, radiationProgressWindow ) );

		if( state.ProcessingWindow() != nullptr ){
			state.ProcessingWindow()->setNewRTFlag();
		}


		delete radiationProgressWindow;

		Fl_Group::window()->activate();


	}

	if( updateFlag ){
		updateFlag = false;

		state.TomographyParameter() = tomographyParameter{ exposureTimeIn.value(), (bool) scatteringOnOff.value(), (size_t) radiationLoopsIn.value(), scatterPropabilityIn.value(), rayStepSizeIn.value() };

	}

}