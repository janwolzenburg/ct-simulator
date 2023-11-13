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
#include "fl_GantryCreation.h"
#include "widgets.h"
#include "plot.h"
#include "fl_TomographyExecution.h"
#include "fl_MainWindow.h"

/*********************************************************************
  Implementations
*********************************************************************/



Fl_GantryCreation::Fl_GantryCreation( int x, int y, int w, int h, Fl_MainWindow& main_window ) :
	Fl_Group{ x, y, w, h },
	main_window_( main_window ),
	tube_properties_{ XRayTubeProperties{}, "tube.properties" },
	projections_properties_{ ProjectionsProperties{}, "projections.properties" },
	physical_detector_properties_{ PhysicalDetectorProperties{}, "physical.properties" },
	gantry_{ CoordinateSystems().AddSystem( "Gantry system"), tube_properties_, projections_properties_, physical_detector_properties_ },

	title_{			X( *this, 0. ),		Y( *this, 0. ),		W( *this, 1. ),		H( *this, 0.035 ),	"Gantry" },

	tube_group_{			X( *this, .0 ),		Y( *this, .04 ) ,	W( *this, 1. ),					H( *this, .4 ) },
	tube_title_{			X( tube_group_, 0. ),	Y( tube_group_, 0. ),	W( tube_group_, 1. ),	H( tube_group_, .075 ),	"xRay Tube" },
	voltage_input_{			X( tube_group_, .0 ),	Y( tube_group_, .15 ),	W( tube_group_, .15 ),	H( tube_group_, .075 ),	"Voltage" },
	current_input_{			X( tube_group_, .25 ),	Y( tube_group_, .15 ),	W( tube_group_, .15 ),	H( tube_group_, .075 ),	"Current" },
	anode_material_input_{	X( tube_group_, .5 ),	Y( tube_group_, .15 ),	W( tube_group_, .5 ),	H( tube_group_, .075 ),	"Material" },
	toggle_filter_button_{  X( tube_group_, .33 ),	Y( tube_group_, .25 ),	W( tube_group_, .33 ),	H( tube_group_, .075 ),	"Al-Filter" },
	spectrum_plot_{			X( tube_group_, .0 ),	Y( tube_group_, .35 ),	W( tube_group_, 1. ),	H( tube_group_, .65 ),	"Spectrum Plot" },

	detector_group_{	X( *this, .0 ),			Y( *this, .45 ),		W( *this, 1. ),		H( *this, .55 ) },
	detector_title_{	X( detector_group_, .0 ),	Y( detector_group_, 0. ),	W( detector_group_, 1. ),	H( detector_group_, .1 ),	"Detector" },
	number_of_angles_input_{		X( detector_group_, .0 ),	Y( detector_group_, .125 ),	W( detector_group_, .2 ),	H( detector_group_, .05 ),	"Angles" },
	number_of_distances_input_{		X( detector_group_, .3 ),	Y( detector_group_, .125 ),	W( detector_group_, .2 ),	H( detector_group_, .05 ),	"Pixel" },
	distance_range_input_{		X( detector_group_, .6 ),	Y( detector_group_, .125 ),	W( detector_group_, .2 ),	H( detector_group_, .05 ),	"Range" },

	number_of_rays_per_pixel_input_{ X( detector_group_, .0 ),	Y( detector_group_, .25 ),	W( detector_group_, .2 ),	H( detector_group_, .05 ),	"Rays / Pix" },
	detector_focus_distance_input_{	X( detector_group_, .25 ),	Y( detector_group_, .25 ),	W( detector_group_, .2 ),	H( detector_group_, .05 ),	"Focus Dist." },
	scattering_structure_input_{	X( detector_group_, .75 ),	Y( detector_group_, .25 ),	W( detector_group_, .2 ),	H( detector_group_, .05 ),	"Anti scat." },
	maximum_ray_angle_input_{	X( detector_group_, .50 ),	Y( detector_group_, .25 ),	W( detector_group_, .2 ),	H( detector_group_, .05 ),	"Max angle" },

	detector_plot_{	X( detector_group_, .0 ),	Y( detector_group_, .35 ),	W( detector_group_, 1. ),	H( detector_group_, .65 ),	"Detector Plot" },

	update_gantry_callback_{ *this, &Fl_GantryCreation::UpdateGantry }
	
	{
		Fl_Group::add( title_ );
		title_.box( FL_NO_BOX ); title_.align( FL_ALIGN_CENTER ); title_.labelsize( 30 );



		Fl_Group::add( tube_group_ );

		tube_group_.add( tube_title_ );
		tube_group_.add( voltage_input_ ); tube_group_.add( current_input_ ); tube_group_.add( anode_material_input_ );
		tube_group_.add( toggle_filter_button_ );

		tube_title_.box( FL_NO_BOX ); tube_title_.align( FL_ALIGN_CENTER ); tube_title_.labelsize( 20 );

		voltage_input_.align( FL_ALIGN_TOP ); current_input_.align( FL_ALIGN_TOP ); anode_material_input_.align( FL_ALIGN_TOP );

		voltage_input_.SetProperties( 20000., 200000., 0 );
		current_input_.SetProperties( .001, 10., 3 );

		voltage_input_.value( tube_properties_.anode_voltage_V );
		current_input_.value( tube_properties_.anode_current_A );
		toggle_filter_button_.value( static_cast<int>( tube_properties_.has_filter_ ) );

		voltage_input_.callback( CallbackFunction<Fl_GantryCreation>::Fl_Callback, &update_gantry_callback_ );
		current_input_.callback( CallbackFunction<Fl_GantryCreation>::Fl_Callback, &update_gantry_callback_ );
		anode_material_input_.callback( CallbackFunction<Fl_GantryCreation>::Fl_Callback, &update_gantry_callback_ );
		toggle_filter_button_.callback( CallbackFunction<Fl_GantryCreation>::Fl_Callback, &update_gantry_callback_ );
				
		toggle_filter_button_.color( FL_BACKGROUND_COLOR, FL_DARK_GREEN );

		vector<string> materialNames;
		for( auto& el : XRayTubeProperties::materials ) materialNames.push_back( el.second.first );

		anode_material_input_.AssignElements( materialNames );
		XRayTubeProperties::Material anode_material = tube_properties_.anode_material;
		string materialName = XRayTubeProperties::materials.at( anode_material ).first;
		anode_material_input_.SetCurrentElement( materialName );

		voltage_input_.tooltip( "Voltage in Volts." );
		current_input_.tooltip( "Current in Ampere." );
		anode_material_input_.tooltip( "Anode material." );


		tube_group_.add( spectrum_plot_ );
		spectrum_plot_.Initialise( PROGRAM_STATE().GetAbsolutePath( "spectrumPlot.png" ), "E in keV", "Spec. Pow. in W/keV", PlotLimits{ false, true, NumberRange{ 10., 200. }, NumberRange{ 0., 1. }, 0.001, 1000. }, "", "", false, false );


		//-----------------------------

		Fl_Group::add( detector_group_ );

		detector_group_.add( detector_title_ );
		detector_title_.box( FL_NO_BOX ); detector_title_.align( FL_ALIGN_CENTER ); detector_title_.labelsize( 20 );


		detector_group_.add( number_of_angles_input_ ); detector_group_.add( number_of_distances_input_ ); detector_group_.add( distance_range_input_ );
		number_of_angles_input_.align( FL_ALIGN_TOP ); number_of_distances_input_.align( FL_ALIGN_TOP ); distance_range_input_.align( FL_ALIGN_TOP );

		number_of_angles_input_.SetProperties( 3, 10000, 0 );
		number_of_angles_input_.value( projections_properties_.number_of_angles() );

		number_of_distances_input_.SetProperties( 3, 10000, 0, Input_Constraints::Odd );
		number_of_distances_input_.value( projections_properties_.number_of_distances() );

		distance_range_input_.SetProperties( 1., 10000., 0 );
		distance_range_input_.value( projections_properties_.measuring_field_size() );

		number_of_angles_input_.callback( CallbackFunction<Fl_GantryCreation>::Fl_Callback, &update_gantry_callback_ );
		number_of_distances_input_.callback( CallbackFunction<Fl_GantryCreation>::Fl_Callback, &update_gantry_callback_ );
		distance_range_input_.callback( CallbackFunction<Fl_GantryCreation>::Fl_Callback, &update_gantry_callback_ );

		number_of_angles_input_.tooltip( "Amount of angles in Sinogram." );
		number_of_distances_input_.tooltip( "Amount of distances in sinogram. Is the amount of detector pixel." );
		distance_range_input_.tooltip( "Size of measure field in mm." );


		detector_group_.add( number_of_rays_per_pixel_input_ ); detector_group_.add( detector_focus_distance_input_ ); detector_group_.add( maximum_ray_angle_input_ ); detector_group_.add( scattering_structure_input_ );
		
		number_of_rays_per_pixel_input_.align( FL_ALIGN_TOP ); detector_focus_distance_input_.align( FL_ALIGN_TOP ); maximum_ray_angle_input_.align( FL_ALIGN_TOP );

		number_of_rays_per_pixel_input_.SetProperties( 1, 1000, 0 );
		number_of_rays_per_pixel_input_.value( static_cast<int>( gantry_.tube().number_of_rays_per_pixel() ) );

		detector_focus_distance_input_.SetProperties( distance_range_input_.value(), 100000., 0);
		detector_focus_distance_input_.value( physical_detector_properties_.detector_focus_distance );

		maximum_ray_angle_input_.SetProperties( .1, 60., 2 );
		maximum_ray_angle_input_.value( physical_detector_properties_.max_ray_angle_allowed_by_structure / 2. / PI * 360. );

		scattering_structure_input_.value( static_cast<int>( physical_detector_properties_.has_anti_scattering_structure ) );
		scattering_structure_input_.color( FL_BACKGROUND_COLOR, FL_DARK_GREEN );

		number_of_rays_per_pixel_input_.callback(  CallbackFunction<Fl_GantryCreation>::Fl_Callback, &update_gantry_callback_ );
		detector_focus_distance_input_.callback( CallbackFunction<Fl_GantryCreation>::Fl_Callback, &update_gantry_callback_ );
		maximum_ray_angle_input_.callback(  CallbackFunction<Fl_GantryCreation>::Fl_Callback, &update_gantry_callback_ );
		scattering_structure_input_.callback(  CallbackFunction<Fl_GantryCreation>::Fl_Callback, &update_gantry_callback_  );

		number_of_rays_per_pixel_input_.tooltip( "How many rays will be simulated per pixel." );
		detector_focus_distance_input_.tooltip( "Detector focus distance." );
		maximum_ray_angle_input_.tooltip( "Maximum detecable angle in degree between pixel and ray. Only valid when anti scattering is activated." );
		scattering_structure_input_.tooltip( "Activate anti scattering structure." );


		detector_group_.add( detector_plot_ );
		detector_plot_.Initialise( PROGRAM_STATE().GetAbsolutePath( "detectorPlot.png" ), "x in mm", "y in mm", PlotLimits{ true, true, NumberRange{ 0, 1 }, NumberRange{ 0, 1 } }, "", "", true, true );

		UpdateGantry();
}

void Fl_GantryCreation::SetDistances( const double max_corner_distance ){
	detector_focus_distance_input_.value( 2. * ( max_corner_distance + 150. ) );
	distance_range_input_.value( 2. * ( max_corner_distance + 25. ) );

	UpdateGantry();
}

void Fl_GantryCreation::UpdateGantry( void ){


		Fl_Group::window()->deactivate();

		
		detector_focus_distance_input_.SetProperties( distance_range_input_.value(), 10000., 0 );
		tube_properties_ = XRayTubeProperties{ voltage_input_.value(), current_input_.value(), XRayTubeProperties::GetMaterialEnum( anode_material_input_.current_element() ), static_cast<size_t>( number_of_rays_per_pixel_input_.value() ), static_cast<bool>( toggle_filter_button_.value() ) };
	

		projections_properties_ = ProjectionsProperties{ number_of_angles_input_.value(), number_of_distances_input_.value(), distance_range_input_.value() };
		physical_detector_properties_ = PhysicalDetectorProperties{ 5., detector_focus_distance_input_.value(), static_cast<bool>( scattering_structure_input_.value() ), maximum_ray_angle_input_.value() / 360. * 2. * PI };
		

		number_of_distances_input_.value( projections_properties_.number_of_distances() );
		number_of_angles_input_.value( projections_properties_.number_of_angles() );


	
		gantry_.UpdateTubeAndDetectorProperties( tube_properties_, projections_properties_, physical_detector_properties_ );



		const XRayTube tubeRef = gantry_.tube();
		const XRayDetector detectorRef = gantry_.detector();

		VectorPair spectrum_points = tubeRef.GetEnergySpectrumPoints();
		for( auto& spectrum_value : spectrum_points.second ){
			spectrum_value /= tubeRef.GetSpectralEnergyResolution();		// "Convert" to integral to match power
		}

		spectrum_plot_.plot().AssignData( spectrum_points );
		spectrum_plot_.AssignData();

		main_window_.tomography_execution_.UpdateInformation( projections_properties_, detectorRef.properties(), gantry_.tube() );

		detector_plot_.plot().ResetObjects();

		const auto allPixel = detectorRef.pixel_array();

		for( const auto& pixel : allPixel ){

			const Point3D startP = pixel.GetPoint( pixel.parameter_1_min(), 0. ).ConvertTo( gantry_.coordinate_system() );
			const Point3D endP = pixel.GetPoint( pixel.parameter_1_max(), 0. ).ConvertTo( gantry_.coordinate_system() );

			const Tuple2D start{ startP.X(), startP.Y() };
			const Tuple2D end{ endP.X(), endP.Y() };

			detector_plot_.plot().AddLine( start, end );

		}

		const Point3D gantryCenter = gantry_.GetCenter();
		detector_plot_.plot().AddPoint( Tuple2D( gantryCenter.X(), gantryCenter.Y() ) );
		detector_plot_.plot().CreatePlot();
		detector_plot_.AssignData();

		Fl_Group::window()->activate();


}