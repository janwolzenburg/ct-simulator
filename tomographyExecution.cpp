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
#include "filteredProjections.h"
#include "progress.h"
#include "widgets.h"
#include "gantryCreation.h"
#include "mainWindow.h"


tomographyExec::tomographyExec( int x, int y, int w, int h, mainWindow& main_window ) :
	Fl_Group{ x, y, w, h },
	main_window_( main_window ),
	title{ X( *this, 0. ), Y( *this, 0. ), W( *this, 1. ), H( *this, .035 ), "Tomography"},

	tomoParameterGrp{		X( *this, .0 ),				Y( *this, .04 ),				W( *this, 1. ),				H( *this, .6 ) },
	parameterTitle{			X( tomoParameterGrp, 0. ),	Y( tomoParameterGrp, 0. ),	W( tomoParameterGrp, 1. ),	H( tomoParameterGrp, .05 ), "Parameter" },
	
	radiationLoopsIn{		X( tomoParameterGrp, 0. ),	Y( tomoParameterGrp, .175 ),	W( tomoParameterGrp, .25 ),	H( tomoParameterGrp, .05 ), "Maximum loops" },
	scatterPropabilityIn{	X( tomoParameterGrp, .45 ),	Y( tomoParameterGrp, .175 ),	W( tomoParameterGrp, .2 ),	H( tomoParameterGrp, .05 ), "Scattering prob." },
	scatteringOnOff{		X( tomoParameterGrp, .75 ),	Y( tomoParameterGrp, .175 ),	W( tomoParameterGrp, .15 ),	H( tomoParameterGrp, .05 ), "Scattering" },
	
	information{			X( tomoParameterGrp, 0.2 ),	Y( tomoParameterGrp, .35 ),	W( tomoParameterGrp, .8 ),	H( tomoParameterGrp, .6 ), "Information" },


	controlGrp{				X( *this, .0 ), vOff( tomoParameterGrp ), W( *this, 1. ), H( *this, .1 ) },
	radiationButton{		X( controlGrp, .25 ), Y( controlGrp, .1 ), W( controlGrp, .5 ), H( controlGrp, .4 ), "Record Slice" },
	exportButton{			X( controlGrp, .65 ), Y( controlGrp, .6 ), W( controlGrp, .4 ), H( controlGrp, .4 ), "Export Sinogram" },

	
	exportChooserInstance{ FileChooser{ "Export Sinogram", "*.sinogram", path{ "./" }, Fl_Native_File_Chooser::Type::BROWSE_SAVE_FILE }, "export.chooser" },
	
	tomographyInstance{},
	tomographyParamerters{ TomographyProperties{}, "tomography.properties" },
	

	currentProjection{ Projections{}, "projections.sinogram" },
	processing_windows_( 0 ),

	radiateCB{ *this, &tomographyExec::radiate },
	updateCB{ *this, &tomographyExec::updateProperties },
	exportCB{ *this, &tomographyExec::exportSinogram }
{


	Fl_Group::add( title );
	title.box( FL_NO_BOX ); title.align( FL_ALIGN_CENTER ); title.labelsize( 30 );

	Fl_Group::add( tomoParameterGrp );

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

	radiationLoopsIn.callback( CallbackFunction<tomographyExec>::Fl_Callback, &updateCB );

	scatterPropabilityIn.callback( CallbackFunction<tomographyExec>::Fl_Callback, &updateCB );
	scatteringOnOff.callback( CallbackFunction<tomographyExec>::Fl_Callback, &updateCB );


	Fl_Group::add( controlGrp );

	controlGrp.add( radiationButton );
	controlGrp.add( exportButton );

	exportButton.deactivate();

	radiationButton.callback( CallbackFunction<tomographyExec>::Fl_Callback, &radiateCB );
	exportButton.callback( CallbackFunction<tomographyExec>::Fl_Callback, &exportCB );

	this->deactivate();
}

void tomographyExec::AssignProjections( const Projections projections ){
	currentProjection = projections;
	exportButton.activate();

	std::unique_ptr<Fl_ProcessingWindow> ptr = std::make_unique<Fl_ProcessingWindow>(  (int) ( 1920. * 0.9 ), (int) ( 1080. * 0.9 ), "Processing", currentProjection );
	processing_windows_.push_back( std::move( ptr ) );
	processing_windows_.back()->show();
}


void tomographyExec::updateProperties( void ){
		tomographyParamerters = TomographyProperties{ (bool) scatteringOnOff.value(), (size_t) radiationLoopsIn.value(), scatterPropabilityIn.value() };
}

void tomographyExec::radiate( void ){

		parent()->deactivate();

		Fl_Progress_Window* radiationProgressWindow = new Fl_Progress_Window{ (Fl_Window*) window(), 20, 5, "Radiation progress"};
		

		tomographyInstance = Tomography{ tomographyParamerters };

		if( radiationProgressWindow != nullptr ){
			Projections new_projections = tomographyInstance.RecordSlice( main_window_.gantryBuild.projections_properties(), main_window_.gantryBuild.gantry(), main_window_.modView.model(), 0, radiationProgressWindow);
			AssignProjections( std::move( new_projections ) );
			delete radiationProgressWindow;
		}

		parent()->activate();
}

void tomographyExec::updateInformation( ProjectionsProperties projection_properties, DetectorProperties detector_properties, XRayTube tube ){
		string informationString = "";


		informationString += "Sinogramgröße:      " + ToString( projection_properties.number_of_angles() ) + " x " + ToString( projection_properties.number_of_distances() ) + '\n';
		informationString += "Sinogramauflösung:  " + ToString( projection_properties.angles_resolution() / 2. / PI * 360.,2 ) + "° x " + ToString( projection_properties.distances_resolution(), 2) + " mm" + '\n' + '\n';
		informationString += "Gantryrotationen:   " + ToString( projection_properties.number_of_frames_to_fill() ) + '\n';
		informationString += "Detektorwinkel:	  " + ToString( detector_properties.arc_angle / 2. / PI * 360., 2 ) + "°" + '\n';


		informationString += "Elektrische Leistung:	  " + ToString( tube.GetElectricalPower()) + "W" + '\n';
		informationString += "Strahlleistung:	  " + ToString( tube.GetEmittedBeamPower() ) + "W" + '\n';
		
		information.value( informationString.c_str() );
}

void tomographyExec::exportSinogram( void ){

	path exportPath = exportChooserInstance.ChooseFile();
	exportChooserInstance.SetAsLoaded();
	if( exportPath.empty() ) return;

	if( exportPath.extension() != ".sinogram" )
		exportPath += ".sinogram";

	currentProjection.Save( exportPath );

}