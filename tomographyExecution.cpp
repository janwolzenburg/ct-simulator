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
	importButton(	 X( controlGrp, .05 ), Y( controlGrp, .6 ), W( controlGrp, .4 ), H( controlGrp, .4 ), "Import Sinogram" ),
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


	exposureTimeIn.value( PROGRAM_STATE().TomographyParameter().exposureTime );
	rayStepSizeIn.value( PROGRAM_STATE().TomographyParameter().rayStepSize );
	radiationLoopsIn.value( (double) PROGRAM_STATE().TomographyParameter().maxRadiationLoops );
	scatterPropabilityIn.value( PROGRAM_STATE().TomographyParameter().scatterPropability );
	scatteringOnOff.value( PROGRAM_STATE().TomographyParameter().scattering );
	scatteringOnOff.color( FL_BACKGROUND_COLOR, FL_DARK_GREEN );

	exposureTimeIn.callback( button_cb, &updateFlag );
	rayStepSizeIn.callback( button_cb, &updateFlag );
	radiationLoopsIn.callback( button_cb, &updateFlag );
	scatterPropabilityIn.callback( button_cb, &updateFlag );
	scatteringOnOff.callback( button_cb, &updateFlag );


	Fl_Group::add( controlGrp );
	controlGrp.box( FL_BORDER_BOX );

	controlGrp.add( radiationButton );
	controlGrp.add( importButton );
	controlGrp.add( exportButton );

	exportButton.deactivate();

	radiationButton.callback( button_cb, &radiateFlag );
	importButton.callback( button_cb, &importFlag );
	exportButton.callback( button_cb, &exportFlag );

	this->deactivate();
}

tomographyExec::~tomographyExec( void ){

}


void tomographyExec::handleEvents( void ){

	programState& state = PROGRAM_STATE();

	if( state.ModelLoaded() && !this->active() ){
		exportButton.deactivate();
		this->activate();
	}
	else if( !state.ModelLoaded() ) this->deactivate();

	if( PROGRAM_STATE().RadonTransformedLoaded() && !exportButton.active() ){
		exportButton.activate();
	}

	if(  unsetFlag( radiateFlag ) ){

		Fl_Group::window()->deactivate();


		Fl_Progress_Window* radiationProgressWindow = new Fl_Progress_Window( (Fl_Window*) PROGRAM_STATE().MainWindow(), 20, 5, "Radiation progress" );
		state.Tomography() = tomography{ state.TomographyParameter() };

		state.assignRadonTransformed( state.Tomography().recordSlice( state.Gantry(), state.Model(), 0, radiationProgressWindow ) );

		if( state.ProcessingWindow() != nullptr ){
			state.ProcessingWindow()->setNewRTFlag();
		}

		delete radiationProgressWindow;


		exportButton.activate();
		Fl_Group::window()->activate();

	}

	if( unsetFlag( exportFlag ) ){

		PROGRAM_STATE().exportSinogram();
	}

	if( unsetFlag( importFlag ) ){

		path chosenPath = PROGRAM_STATE().importSinogram();

		if( chosenPath.empty() ) return;

		vector<char> binData = importSerialized( chosenPath );
		vector<char>::const_iterator it = binData.cbegin();

		radonTransformed importedSinogram{ binData, it };
		state.assignRadonTransformed( importedSinogram );

		if( state.ProcessingWindow() != nullptr ){
			state.ProcessingWindow()->setNewRTFlag();
		}

		exportButton.activate();

	}

	if( unsetFlag( updateFlag )){
		
		informationUpdateFlag = true;

		state.TomographyParameter() = tomographyParameter{ exposureTimeIn.value(), (bool) scatteringOnOff.value(), (size_t) radiationLoopsIn.value(), scatterPropabilityIn.value(), rayStepSizeIn.value() };

	}

	if( unsetFlag( informationUpdateFlag )){

		string informationString;

		informationString += "Sinogramgröße:      " + toString( state.RadonParameter().numberPoints.col ) + " x " + toString( state.RadonParameter().numberPoints.row ) + '\n';
		informationString += "Sinogramauflösung:  " + toString( state.RadonParameter().resolution.col / 2. / PI * 360.,2 ) + "° x " + toString( state.RadonParameter().resolution.row, 2 ) + " mm" + '\n' + '\n';
		informationString += "Gantryrotationen:   " + toString( state.RadonParameter().framesToFillSinogram ) + '\n';
		informationString += "Detektorwinkel:	  " + toString( state.DetectorPhysicalParameter().angle / 2. / PI * 360., 2 ) + "°" + '\n';


		informationString += "Elektrische Leistung:	  " + toString( state.Tube().electricalPower() ) + "W" + '\n';
		informationString += "Strahlleistung:	  " + toString( state.Tube().rayPower() ) + "W" + '\n';
		informationString += "Strahlenergie:	  " + toString( state.Tube().getEnergy( state.TomographyParameter().exposureTime ) ) + "J" + '\n';
		informationString += "Strahlenergie gesamt:	  " + toString( state.Tube().getEnergy( state.TomographyParameter().exposureTime ) * state.RadonParameter().framesToFillSinogram ) + "J" + '\n';
		information.value( informationString.c_str() );

	}

}