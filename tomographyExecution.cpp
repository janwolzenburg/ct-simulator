#include "tomographyExecution.h"


#include "processingWindow.h"
#include "backprojection.h"
#include "progress.h"

tomographyExec::tomographyExec( int x, int y, int w, int h ) :
	Fl_Group( x, y, w, h ),
	title( X( *this, 0. ), Y( *this, 0. ), W( *this, 1. ), H( *this, .05 ), "Tomography"),

	tomoParameterGrp(		X( *this, .0 ),				Y( *this, .1 ),				W( *this, 1. ),				H( *this, .6 ) ),
	parameterTitle(			X( tomoParameterGrp, 0. ),	Y( tomoParameterGrp, 0. ),	W( tomoParameterGrp, 1. ),	H( tomoParameterGrp, .05 ), "Parameter" ),
	
	exposureTimeIn(			X( tomoParameterGrp, .1 ),	Y( tomoParameterGrp, .08725 ),	W( tomoParameterGrp, .1 ),	H( tomoParameterGrp, .05 ), "Exposure time" ),
	rayStepSizeIn(			X( tomoParameterGrp, .33 ),	Y( tomoParameterGrp, .08725 ),	W( tomoParameterGrp, .15 ),	H( tomoParameterGrp, .05 ), "Ray step size" ),
	
	radiationLoopsIn(		X( tomoParameterGrp, 0. ),	Y( tomoParameterGrp, .175 ),	W( tomoParameterGrp, .35 ),	H( tomoParameterGrp, .05 ), "Maximum loops" ),
	scatterPropabilityIn(	X( tomoParameterGrp, .45 ),	Y( tomoParameterGrp, .175 ),	W( tomoParameterGrp, .15 ),	H( tomoParameterGrp, .05 ), "Scattering prob." ),
	scatteringOnOff(		X( tomoParameterGrp, .75 ),	Y( tomoParameterGrp, .175 ),	W( tomoParameterGrp, .15 ),	H( tomoParameterGrp, .05 ), "Scattering" ),
	
	information(			X( tomoParameterGrp, 0.2 ),	Y( tomoParameterGrp, .35 ),	W( tomoParameterGrp, .8 ),	H( tomoParameterGrp, .6 ), "Information" ),


	controlGrp( X( *this, .0 ), vOff( tomoParameterGrp ), W( *this, 1. ), H( *this, .1 ) ),
	radiationButton( X( controlGrp, .25 ), Y( controlGrp, .1 ), W( controlGrp, .5 ), H( controlGrp, .4 ), "Record Slice" ),
	exportButton(	 X( controlGrp, .65 ), Y( controlGrp, .6 ), W( controlGrp, .4 ), H( controlGrp, .4 ), "Export Sinogram" ),

	
	radiateFlag( false ),
	updateFlag( false ),
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

	exposureTimeIn.value( state.tomographyParamerters.exposureTime );
	rayStepSizeIn.value( state.tomographyParamerters.rayStepSize );
	radiationLoopsIn.value( (double) state.tomographyParamerters.maxRadiationLoops );
	scatterPropabilityIn.value( state.tomographyParamerters.scatterPropability );
	scatteringOnOff.value( state.tomographyParamerters.scattering );
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

tomographyExec::~tomographyExec( void ){

}


void tomographyExec::handleEvents( void ){

	programState& state = PROGRAM_STATE();

	if( state.ModelLoaded() && !this->active() ){
		this->activate();
	}
	else if( !state.ModelLoaded() ){
		this->deactivate();
	}
	if( state.RadonTransformedLoaded() ){
		exportButton.activate();
	}
	else{
		exportButton.deactivate();
	}

	if(  unsetFlag( radiateFlag ) ){

		state.deactivateAll();

		Fl_Progress_Window* radiationProgressWindow = nullptr;
		if( state.mainWindow != nullptr )
			radiationProgressWindow = new Fl_Progress_Window( (Fl_Window*) state.mainWindow, 20, 5, "Radiation progress" );
		

		state.tomographyInstance = tomography{ state.tomographyParamerters };

		if( radiationProgressWindow != nullptr ){
			state.assignRadonTransformed( state.tomographyInstance.recordSlice( state.Gantry(), state.Model(), 0, radiationProgressWindow ) );
			delete radiationProgressWindow;
		}

		if( state.processingWindow != nullptr )
			state.processingWindow->setNewRTFlag();
		


		state.activateAll();

	}

	if( unsetFlag( exportFlag ) ){

		state.exportSinogram();
	}


	if( unsetFlag( updateFlag )){
		
		informationUpdateFlag = true;

		state.tomographyParamerters = tomographyParameter{ exposureTimeIn.value(), (bool) scatteringOnOff.value(), (size_t) radiationLoopsIn.value(), scatterPropabilityIn.value(), rayStepSizeIn.value() };

	}

	if( unsetFlag( informationUpdateFlag )){

		string informationString;

		informationString += "Sinogramgröße:      " + toString( state.RadonParameter().numberPoints.col ) + " x " + toString( state.RadonParameter().numberPoints.row ) + '\n';
		informationString += "Sinogramauflösung:  " + toString( state.RadonParameter().resolution.col / 2. / PI * 360.,2 ) + "° x " + toString( state.RadonParameter().resolution.row, 2 ) + " mm" + '\n' + '\n';
		informationString += "Gantryrotationen:   " + toString( state.RadonParameter().framesToFillSinogram ) + '\n';
		informationString += "Detektorwinkel:	  " + toString( state.DetectorPhysicalParameter().angle / 2. / PI * 360., 2 ) + "°" + '\n';


		informationString += "Elektrische Leistung:	  " + toString( state.Tube().electricalPower() ) + "W" + '\n';
		informationString += "Strahlleistung:	  " + toString( state.Tube().rayPower() ) + "W" + '\n';
		informationString += "Strahlenergie:	  " + toString( state.Tube().getEnergy( state.tomographyParamerters.exposureTime ) ) + "J" + '\n';
		informationString += "Strahlenergie gesamt:	  " + toString( state.Tube().getEnergy( state.tomographyParamerters.exposureTime ) * state.RadonParameter().framesToFillSinogram ) + "J" + '\n';
		information.value( informationString.c_str() );

	}

}