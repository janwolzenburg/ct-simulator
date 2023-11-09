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
#include "filteredProjections.h"
#include "progress.h"
#include "widgets.h"
#include "gantryCreation.h"
#include "mainWindow.h"

tomographyExec::tomographyExec( int x, int y, int w, int h, mainWindow* const main_window ) :
	Fl_Group{ x, y, w, h },
	main_window_( main_window ),
	title{ X( *this, 0. ), Y( *this, 0. ), W( *this, 1. ), H( *this, .05 ), "Tomography"},

	tomoParameterGrp{		X( *this, .0 ),				Y( *this, .1 ),				W( *this, 1. ),				H( *this, .6 ) },
	parameterTitle{			X( tomoParameterGrp, 0. ),	Y( tomoParameterGrp, 0. ),	W( tomoParameterGrp, 1. ),	H( tomoParameterGrp, .05 ), "Parameter" },
	
	radiationLoopsIn{		X( tomoParameterGrp, 0. ),	Y( tomoParameterGrp, .175 ),	W( tomoParameterGrp, .25 ),	H( tomoParameterGrp, .05 ), "Maximum loops" },
	scatterPropabilityIn{	X( tomoParameterGrp, .45 ),	Y( tomoParameterGrp, .175 ),	W( tomoParameterGrp, .2 ),	H( tomoParameterGrp, .05 ), "Scattering prob." },
	scatteringOnOff{		X( tomoParameterGrp, .75 ),	Y( tomoParameterGrp, .175 ),	W( tomoParameterGrp, .15 ),	H( tomoParameterGrp, .05 ), "Scattering" },
	
	information{			X( tomoParameterGrp, 0.2 ),	Y( tomoParameterGrp, .35 ),	W( tomoParameterGrp, .8 ),	H( tomoParameterGrp, .6 ), "Information" },


	controlGrp{				X( *this, .0 ), vOff( tomoParameterGrp ), W( *this, 1. ), H( *this, .1 ) },
	radiationButton{		X( controlGrp, .25 ), Y( controlGrp, .1 ), W( controlGrp, .5 ), H( controlGrp, .4 ), "Record Slice" },
	exportButton{			X( controlGrp, .65 ), Y( controlGrp, .6 ), W( controlGrp, .4 ), H( controlGrp, .4 ), "Export Sinogram" },

	
	exportChooserInstance{ "Export Sinogram", "*.sinogram", path{ "./" }, Fl_Native_File_Chooser::Type::BROWSE_SAVE_FILE },
	storedExportChooser{ PROGRAM_STATE().getPath( "storedExportChooser.txt" ), exportChooserInstance },
	
	tomographyInstance{},
	tomographyParamerters{},
	storedTomographyParamerter{ PROGRAM_STATE().getPath("storedTomograpyParameter.txt"), tomographyParamerters},
	
	radiateFlag( false ),
	updateFlag( false ),
	exportFlag( false ),
	informationUpdateFlag( false ),

	projections_loaded( false ),
	currentProjection{},
	processing_windows_( 0, nullptr )


{
	main_window_->add( title );

	Fl_Group::box( FL_BORDER_BOX );
	Fl_Group::add( title );
	title.box( FL_NO_BOX ); title.align( FL_ALIGN_CENTER ); title.labelsize( 30 );

	Fl_Group::add( tomoParameterGrp );
	tomoParameterGrp.box( FL_BORDER_BOX );

	tomoParameterGrp.add( parameterTitle );

	tomoParameterGrp.add( radiationLoopsIn );
	tomoParameterGrp.add( scatterPropabilityIn );
	tomoParameterGrp.add( scatteringOnOff );
	tomoParameterGrp.add( information );


	parameterTitle.box( FL_NO_BOX ); parameterTitle.align( FL_ALIGN_CENTER ); parameterTitle.labelsize( 20 );

	radiationLoopsIn.align( FL_ALIGN_TOP );
	scatterPropabilityIn.align( FL_ALIGN_TOP );

	radiationLoopsIn.bounds(0, 100);
	radiationLoopsIn.step( 1. );
	scatterPropabilityIn.SetProperties( 0., 1., 6 );


	radiationLoopsIn.tooltip( "Maximum amount of iterations for ray tracing. How often a ray can be scattered." );
	scatterPropabilityIn.tooltip( "Correction factor for scattering propability. More scattering with higher value." );
	scatteringOnOff.tooltip( "Enable or disable scattering." );

	
	programState& state = PROGRAM_STATE();

	radiationLoopsIn.value( (double) tomographyParamerters.max_scattering_occurrences );
	scatterPropabilityIn.value( tomographyParamerters.scatter_propability_correction );
	scatteringOnOff.value( tomographyParamerters.scattering_enabled );
	scatteringOnOff.color( FL_BACKGROUND_COLOR, FL_DARK_GREEN );

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
	
	storedExportChooser.Save();
	storedTomographyParamerter.Save();

	for( auto currentWindow : processing_windows_ )
		delete currentWindow;
}

void tomographyExec::AssignProjections( const Projections projections ){
	projections_loaded = true;
	currentProjection = projections;
	processing_windows_.emplace_back( new processingWindow{ (int) ( 1920. * 0.9 ), (int) ( 1080. * 0.9 ), "Processing", currentProjection } );
	processing_windows_.back()->show();
}

void tomographyExec::handleEvents( void ){

	programState& state = PROGRAM_STATE();

	if( projections_loaded )
		exportButton.activate();
	else
		exportButton.deactivate();
	
	if(  UnsetFlag( radiateFlag ) ){

		this->parent()->deactivate();

		Fl_Progress_Window* radiationProgressWindow = new Fl_Progress_Window{ (Fl_Window*) this->window(), 20, 5, "Radiation progress"};
		

		tomographyInstance = Tomography{ tomographyParamerters };

		if( radiationProgressWindow != nullptr ){
			projections_loaded = true;
			currentProjection = tomographyInstance.RecordSlice( main_window_->gantryBuild.projections_properties(), main_window_->gantryBuild.gantry(), main_window_->modView.model(), 0, radiationProgressWindow);
			delete radiationProgressWindow;
		}

		processing_windows_.emplace_back( new processingWindow{ (int) ( 1920. * 0.9 ), (int) ( 1080. * 0.9 ), "Processing", currentProjection } );
		processing_windows_.back()->show();

		this->parent()->activate();
	}

	if( UnsetFlag( exportFlag ) )
		exportSinogram();
	

	if( UnsetFlag( updateFlag )){
		informationUpdateFlag = true;
		tomographyParamerters = TomographyProperties{ (bool) scatteringOnOff.value(), (size_t) radiationLoopsIn.value(), scatterPropabilityIn.value() };
		storedTomographyParamerter.SetAsLoaded();
	}

	if( UnsetFlag( informationUpdateFlag )){

		string informationString;

		informationString += "Sinogramgröße:      " + ToString( main_window_->gantryBuild.projections_properties().number_of_angles() ) + " x " + ToString( main_window_->gantryBuild.projections_properties().number_of_distances() ) + '\n';
		informationString += "Sinogramauflösung:  " + ToString( main_window_->gantryBuild.projections_properties().angles_resolution() / 2. / PI * 360.,2 ) + "° x " + ToString( main_window_->gantryBuild.projections_properties().distances_resolution(), 2) + " mm" + '\n' + '\n';
		informationString += "Gantryrotationen:   " + ToString( main_window_->gantryBuild.projections_properties().number_of_frames_to_fill() ) + '\n';
		informationString += "Detektorwinkel:	  " + ToString( main_window_->gantryBuild.gantry().detector().properties().arc_angle / 2. / PI * 360., 2 ) + "°" + '\n';


		informationString += "Elektrische Leistung:	  " + ToString( main_window_->gantryBuild.gantry().tube().GetElectricalPower()) + "W" + '\n';
		informationString += "Strahlleistung:	  " + ToString( main_window_->gantryBuild.gantry().tube().GetEmittedBeamPower() ) + "W" + '\n';
		
		information.value( informationString.c_str() );
	}


	for( auto currentWindow : processing_windows_ )
		currentWindow->handleEvents();

}

void tomographyExec::exportSinogram( void ){
	if( projections_loaded ){

		path exportPath = exportChooserInstance.ChooseFile();
		storedExportChooser.SetAsLoaded();
		if( exportPath.empty() ) return;

		if( exportPath.extension() != ".sinogram" )
			exportPath += ".sinogram";

		vector<char> binary_data;
		currentProjection.Serialize( binary_data );

		ExportSerialized( exportPath, binary_data );

	}
}