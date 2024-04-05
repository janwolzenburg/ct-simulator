/******************************************************************
* @file   fl_GantryCreation.cpp
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
	tube_properties_{ XRayTubeProperties{}, "saved.tubeproperties" },
	projections_properties_{ ProjectionsProperties{}, "saved.projectionproperties" },
	physical_detector_properties_{ PhysicalDetectorProperties{}, "saved.physicalproperties" },
	gantry_{ GetCoordinateSystemTree().AddSystem( "Gantry system"), tube_properties_, projections_properties_, physical_detector_properties_ },

	title_{			X( *this, 0. ),		Y( *this, 0. ),		W( *this, 1. ),		H( *this, 0.035 ),	"Gantry" },

	tube_group_{						X( *this, .0 ),		Y( *this, .04 ) ,	W( *this, 1. ),					H( *this, .4 ) },
	tube_title_{						X( tube_group_, 0. ),		Y( tube_group_, 0. ),		W( tube_group_, 1. ),		H( tube_group_, .075 ),	"xRay Tube" },
	voltage_input_{					X( tube_group_, .0 ),		Y( tube_group_, .15 ),	W( tube_group_, .15 ),	H( tube_group_, .075 ),	"Voltage" },
	current_input_{					X( tube_group_, .25 ),	Y( tube_group_, .15 ),	W( tube_group_, .15 ),	H( tube_group_, .075 ),	"Current" },
	anode_material_input_{	X( tube_group_, .5 ),		Y( tube_group_, .15 ),	W( tube_group_, .5 ),		H( tube_group_, .075 ),	"Material" },
	toggle_filter_button_{  X( tube_group_, .0 ),		Y( tube_group_, .25 ),	W( tube_group_, .175 ),	H( tube_group_, .075 ),	"Filter" },
	filter_cutoff_input{		X( tube_group_, .4 ),		Y( tube_group_, .25 ),	W( tube_group_, .15 ),	H( tube_group_, .075 ),	"Cut-Off-Energy" },
	filter_gradient_input{  X( tube_group_, .9 ),		Y( tube_group_, .25 ),	W( tube_group_, .1 ),		H( tube_group_, .075 ),	"Gradient" },
	spectrum_plot_{					X( tube_group_, .0 ),		Y( tube_group_, .35 ),	W( tube_group_, 1. ),		H( tube_group_, .65 ),	"Spectrum Plot" },

	detector_group_{						X( *this, .0 ),			Y( *this, .45 ),		W( *this, 1. ),		H( *this, .55 ) },
	detector_title_{						X( detector_group_, .0 ),	Y( detector_group_, 0. ),		W( detector_group_, 1. ),	H( detector_group_, .1 ),	"Detector" },
	number_of_angles_input_{		X( detector_group_, .0 ),	Y( detector_group_, .125 ),	W( detector_group_, .2 ),	H( detector_group_, .05 ),	"Projections" },
	number_of_distances_input_{	X( detector_group_, .3 ),	Y( detector_group_, .125 ),	W( detector_group_, .2 ),	H( detector_group_, .05 ),	"Distances" },
	distance_range_input_{			X( detector_group_, .6 ),	Y( detector_group_, .125 ),	W( detector_group_, .2 ),	H( detector_group_, .05 ),	"Distance range" },

	number_of_rays_per_pixel_input_{	X( detector_group_, .0 ),		Y( detector_group_, .25 ),	W( detector_group_, .2 ),	H( detector_group_, .05 ),	"Rays / Pixel" },
	detector_focus_distance_input_{		X( detector_group_, .25 ),	Y( detector_group_, .25 ),	W( detector_group_, .2 ),	H( detector_group_, .05 ),	"Focus Dist." },
	scattering_structure_input_{			X( detector_group_, .75 ),	Y( detector_group_, .25 ),	W( detector_group_, .2 ),	H( detector_group_, .05 ),	"Scatter grid" },
	maximum_ray_angle_input_{					X( detector_group_, .50 ),	Y( detector_group_, .25 ),	W( detector_group_, .2 ),	H( detector_group_, .05 ),	"Max. angle" },
	detector_plot_{										X( detector_group_, .0 ),		Y( detector_group_, .35 ),	W( detector_group_, 1. ),	H( detector_group_, .65 ),	"Detector Plot" },

	update_gantry_callback_{ *this, &Fl_GantryCreation::UpdateGantry }
	
	{
		Fl_Group::add( title_ );
		title_.box( FL_NO_BOX ); title_.align( FL_ALIGN_CENTER ); title_.labelsize( 30 );



		Fl_Group::add( tube_group_ );

		tube_group_.add( tube_title_ );
		tube_group_.add( voltage_input_ ); tube_group_.add( current_input_ ); tube_group_.add( anode_material_input_ );
		tube_group_.add( toggle_filter_button_ ); tube_group_.add( filter_cutoff_input ); tube_group_.add( filter_gradient_input );

		tube_title_.box( FL_NO_BOX ); tube_title_.align( FL_ALIGN_CENTER ); tube_title_.labelsize( 20 );

		voltage_input_.align( FL_ALIGN_TOP ); current_input_.align( FL_ALIGN_TOP ); anode_material_input_.align( FL_ALIGN_TOP );

		voltage_input_.SetProperties( 20000., maximum_energy_in_tube_spectrum, 0 );
		current_input_.SetProperties( .001, 10., 3 );

		filter_cutoff_input.SetProperties( 0., 120000., 0 );
		filter_gradient_input.SetProperties( .1, 10., 1 );

		voltage_input_.value( tube_properties_.anode_voltage_V );
		current_input_.value( tube_properties_.anode_current_A );
		toggle_filter_button_.value( static_cast<int>( tube_properties_.has_filter_ ) );
		filter_cutoff_input.value( tube_properties_.filter_cut_of_energy );
		filter_gradient_input.value( tube_properties_.filter_gradient );

		voltage_input_.callback( CallbackFunction<Fl_GantryCreation>::Fl_Callback, &update_gantry_callback_ );
		current_input_.callback( CallbackFunction<Fl_GantryCreation>::Fl_Callback, &update_gantry_callback_ );
		anode_material_input_.callback( CallbackFunction<Fl_GantryCreation>::Fl_Callback, &update_gantry_callback_ );
		toggle_filter_button_.callback( CallbackFunction<Fl_GantryCreation>::Fl_Callback, &update_gantry_callback_ );
		filter_cutoff_input.callback( CallbackFunction<Fl_GantryCreation>::Fl_Callback, &update_gantry_callback_ );
		filter_gradient_input.callback( CallbackFunction<Fl_GantryCreation>::Fl_Callback, &update_gantry_callback_ );
					
		
		toggle_filter_button_.color( FL_BACKGROUND_COLOR, FL_DARK_GREEN );

		vector<string> material_names;
		for( auto& material : XRayTubeProperties::materials ) material_names.push_back( material.second.first );

		anode_material_input_.AssignElements( material_names );
		XRayTubeProperties::Material anode_material = tube_properties_.anode_material;
		string material_name = XRayTubeProperties::materials.at( anode_material ).first;
		anode_material_input_.SetCurrentElement( material_name );

		voltage_input_.tooltip( "Voltage in Volts." );
		current_input_.tooltip( "Current in Ampere." );
		anode_material_input_.tooltip( "Anode material." );
		
		filter_cutoff_input.tooltip( "Energy under which the filter absorbs completely. In eV." );
		filter_gradient_input.tooltip( "Gradient of filter." );

		tube_group_.add( spectrum_plot_ );
		spectrum_plot_.Initialise( PROGRAM_STATE().GetAbsolutePath( "spectrumPlot.png" ), "E in keV", "N_P/sec * E in W/keV", PlotLimits{ false, true, NumberRange{ 10., 200. }, NumberRange{ 0., 1. }, 0.001, 1000. }, "", "", false, false );


		//-----------------------------

		Fl_Group::add( detector_group_ );

		detector_group_.add( detector_title_ );
		detector_title_.box( FL_NO_BOX ); detector_title_.align( FL_ALIGN_CENTER ); detector_title_.labelsize( 20 );


		detector_group_.add( number_of_angles_input_ ); detector_group_.add( number_of_distances_input_ ); detector_group_.add( distance_range_input_ );
		number_of_angles_input_.align( FL_ALIGN_TOP ); number_of_distances_input_.align( FL_ALIGN_TOP ); distance_range_input_.align( FL_ALIGN_TOP );

		number_of_angles_input_.SetProperties( 3, 10000, 0 );
		number_of_angles_input_.value( projections_properties_.number_of_projections() );

		number_of_distances_input_.SetProperties( 4, 10000, 0, Input_Constraints::Even );
		number_of_distances_input_.value( projections_properties_.number_of_distances() );

		distance_range_input_.SetProperties( 1., 10000., 0 );
		distance_range_input_.value( projections_properties_.measuring_field_size() );

		number_of_angles_input_.callback( CallbackFunction<Fl_GantryCreation>::Fl_Callback, &update_gantry_callback_ );
		number_of_distances_input_.callback( CallbackFunction<Fl_GantryCreation>::Fl_Callback, &update_gantry_callback_ );
		distance_range_input_.callback( CallbackFunction<Fl_GantryCreation>::Fl_Callback, &update_gantry_callback_ );

		number_of_angles_input_.tooltip( "amount of projections." );
		number_of_distances_input_.tooltip( "amount of distances in projections. Is the amount of detector pixel." );
		distance_range_input_.tooltip( "Size of measure field in mm." );


		detector_group_.add( number_of_rays_per_pixel_input_ ); detector_group_.add( detector_focus_distance_input_ ); detector_group_.add( maximum_ray_angle_input_ ); detector_group_.add( scattering_structure_input_ );
		
		number_of_rays_per_pixel_input_.align( FL_ALIGN_TOP ); detector_focus_distance_input_.align( FL_ALIGN_TOP ); maximum_ray_angle_input_.align( FL_ALIGN_TOP );

		number_of_rays_per_pixel_input_.SetProperties( 1, 1000, 0 );
		number_of_rays_per_pixel_input_.value( static_cast<int>( gantry_.tube().number_of_rays_per_pixel() ) );

		detector_focus_distance_input_.SetProperties( distance_range_input_.value(), 100000., 0);
		detector_focus_distance_input_.value( physical_detector_properties_.detector_focus_distance );

		maximum_ray_angle_input_.SetProperties( .1, 60., 2 );
		maximum_ray_angle_input_.value( physical_detector_properties_.max_angle_allowed_by_structure / 2. / PI * 360. );

		scattering_structure_input_.value( static_cast<int>( physical_detector_properties_.has_anti_scattering_structure ) );
		scattering_structure_input_.color( FL_BACKGROUND_COLOR, FL_DARK_GREEN );

		number_of_rays_per_pixel_input_.callback(  CallbackFunction<Fl_GantryCreation>::Fl_Callback, &update_gantry_callback_ );
		detector_focus_distance_input_.callback( CallbackFunction<Fl_GantryCreation>::Fl_Callback, &update_gantry_callback_ );
		maximum_ray_angle_input_.callback(  CallbackFunction<Fl_GantryCreation>::Fl_Callback, &update_gantry_callback_ );
		scattering_structure_input_.callback(  CallbackFunction<Fl_GantryCreation>::Fl_Callback, &update_gantry_callback_  );

		number_of_rays_per_pixel_input_.tooltip( "How many rays will be simulated per pixel." );
		detector_focus_distance_input_.tooltip( "Detector focus distance." );
		maximum_ray_angle_input_.tooltip( "Maximum detecable angle in degree between pixel and ray. Only valid when anti scattering is activated." );
		scattering_structure_input_.tooltip( "Activate anti scattering grid." );


		detector_group_.add( detector_plot_ );
		detector_plot_.Initialise( PROGRAM_STATE().GetAbsolutePath( "detectorPlot.png" ), "x in mm", "y in mm", PlotLimits{ true, true, NumberRange{ 0, 1 }, NumberRange{ 0, 1 } }, "", "", true, true );

		UpdateGantry();
}

void Fl_GantryCreation::SetDistances( const double max_corner_distance ){
	distance_range_input_.value( ceil( 2.05 * ( max_corner_distance ) ) );
	detector_focus_distance_input_.SetProperties( distance_range_input_.value(), 100000., 0);
	detector_focus_distance_input_.value( ceil( 2.75 * ( max_corner_distance ) ) );

	UpdateGantry();
}

void Fl_GantryCreation::UpdateGantry( void ){


		Fl_Group::window()->deactivate();

		tube_properties_ = XRayTubeProperties{	voltage_input_.value(), current_input_.value(), XRayTubeProperties::GetMaterialEnum( anode_material_input_.current_element() ), 
												static_cast<size_t>( number_of_rays_per_pixel_input_.value() ), static_cast<bool>( toggle_filter_button_.value() ),
												filter_cutoff_input.value(), filter_gradient_input.value() };
	

		projections_properties_ = ProjectionsProperties{ number_of_angles_input_.value(), number_of_distances_input_.value(), distance_range_input_.value() };
		
		physical_detector_properties_ = PhysicalDetectorProperties{ 2., detector_focus_distance_input_.value(), static_cast<bool>( scattering_structure_input_.value() ), maximum_ray_angle_input_.value() / 360. * 2. * PI };
		

		number_of_distances_input_.value( projections_properties_.number_of_distances() );
		number_of_angles_input_.value( projections_properties_.number_of_projections() );


	
		gantry_.UpdateTubeAndDetectorProperties( tube_properties_, projections_properties_, physical_detector_properties_ );



		const XRayTube tube = gantry_.tube();
		const XRayDetector detector = gantry_.detector();

		VectorPair spectrum_points = tube.GetEnergySpectrumPoints();
		//for( auto& spectrum_value : spectrum_points.second ){
		for( size_t energy_index = 0; energy_index < spectrum_points.first.size(); energy_index++ ){
			spectrum_points.second.at( energy_index ) *= spectrum_points.first.at( energy_index )  / tube.GetSpectralEnergyResolution() * J_Per_eV;		// "Convert" to integral to match power
			//spectrum_points.second.at( energy_index ) *= 1.  / tubeRef.GetSpectralEnergyResolution() * J_Per_eV;		// "Convert" to integral to match power
		}

		spectrum_plot_.plot().AssignData( spectrum_points );
		spectrum_plot_.AssignData();

		main_window_.tomography_execution_.UpdateInformation( projections_properties_, detector.properties(), gantry_.tube() );

		detector_plot_.plot().ResetObjects();

		const auto all_pixel = detector.pixel_array();

		for( const auto& pixel : all_pixel ){

			const Point3D start_point = pixel.GetPoint( pixel.parameter_1_min(), 0. ).ConvertTo( gantry_.coordinate_system() );
			const Point3D end_point = pixel.GetPoint( pixel.parameter_1_max(), 0. ).ConvertTo( gantry_.coordinate_system() );

			const Tuple2D start{ start_point.X(), start_point.Y() };
			const Tuple2D end{ end_point.X(), end_point.Y() };

			detector_plot_.plot().AddLine( start, end );

		}

		const Point3D gantry_center = gantry_.GetCenter();
		detector_plot_.plot().AddPoint( Tuple2D{ gantry_center.X(), gantry_center.Y() }, "Center");
		detector_plot_.plot().AddPoint( Tuple2D{ gantry_.tube().coordinate_system()->GetOriginInParentSystem().X(), gantry_.tube().coordinate_system()->GetOriginInParentSystem().Y() }, "Tube");
		
		detector_plot_.plot().CreatePlot();
		detector_plot_.AssignData();

		Fl_Group::window()->activate();


}
