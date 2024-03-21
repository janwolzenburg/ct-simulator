
#include "verifyprocessing.h"
#include "verificationParameter.h"
#include "plotting.h"
#include "model.h"
#include "persistingObject.h"
#include "gantry.h"
#include "tomography.h"
#include "projectionsProperties.h"
#include "projections.h"
#include "filteredProjections.h"
#include "backprojection.h"

//#define RECALC

void VerifyFilteredprojections( void ){

	path model_path{ "./phantom 2.model" };
	PersistingObject<Model> model{ Model{}, model_path, true };
	Tuple3D center = PrimitiveVector3{ model.size() } / -2.;
	model.coordinate_system()->SetPrimitive( PrimitiveCoordinateSystem{ center, Tuple3D{ 1,0,0 }, Tuple3D{ 0, 1, 0 }, Tuple3D{ 0 ,0 ,1} } );

	ProjectionsProperties projections_properties{ 6*number_of_projections, 6*number_of_pixel, sqrt( pow( model.size().x, 2. ) + pow( model.size().y, 2. ) ) };
	PhysicalDetectorProperties physical_detector_properties{ .5, projections_properties.measuring_field_size() * 1.1, false };
	XRayTubeProperties tube_properties{ 140000., 0.5, XRayTubeProperties::Material::Thungsten, 1, true, 16000., 3.5 };

	CoordinateSystem* gantry_system = GlobalSystem()->CreateCopy("Gantry system");
	Gantry gantry{ gantry_system, tube_properties, projections_properties, physical_detector_properties };

	RayScattering ray_scattering{ simulation_properties.number_of_scatter_angles, {10000., 200000.}, 32,  gantry_system->GetEz(), PI / 2. };
	TomographyProperties tomography_properties{ false, 1, 0., true, 0., };

	Tomography tomography{ tomography_properties };
	VoxelData::SetArtefactImpactFactor( 0 );

	Surface slice_surface{ gantry.coordinate_system()->GetEy(), gantry.coordinate_system()->GetEx(), gantry.coordinate_system()->GetOriginPoint() };
	auto slice = model.GetSlice( slice_surface, GridIndex{ model.number_of_voxel_3D().x, model.number_of_voxel_3D().y }, GridCoordinates{ model.voxel_size().x, model.voxel_size().y } );
	

	auto slice_axis = openAxis(  GetPath("phantom_slice_image"), true );
	string format{ to_string(slice.start().c) + "," + to_string(slice.GetEnd().c) + ";" + to_string(slice.start().r) + "," + to_string(slice.GetEnd().c) + ";$x$ in mm;$y$ in mm;$\\mu_\\mathrm{abs}$ in mm$^{-1}$"};
	addSingleObject( slice_axis, "PhantomSliceImage", slice.GetDoubleGrid(), format, true);
	
	

	#ifndef RECALC
	PersistingObject<Projections> projections{Projections{}, GetPath( "verification.projections" ), false };
	if( !projections.was_loaded() ){
		optional<Projections> opt_projections = tomography.RecordSlice( projections_properties, gantry, model, 0. );
		if( !opt_projections.has_value() ) return;

		projections = opt_projections.value();
	}
	#else

	optional<Projections> opt_projections = tomography.RecordSlice( projections_properties, gantry, model, 0. );
	if( !opt_projections.has_value() ) return;
	Projections projections = opt_projections.value();

	#endif

	vector<double> distances =  CreateLinearSpace(	projections.data().start().r, 
													projections.data().GetEnd().r, 
													projections.data().size().r );
	const double angle = PI / 2. * 0.3;
	const double t  = angle;
	const double ray_length = 480;

	for( int s_index = 0; s_index < distances.size() / 2; s_index++ ){
		
		if( s_index % 4 != 0 ) continue;

		double s = distances.at( distances.size() / 2 - s_index - 1);

		Tuple2D start{	s * cos( t ) + ray_length / 2. * sin( t ),
						s * sin( t ) - ray_length / 2. * cos( t ) };

		Tuple2D end{	s * cos( t ) - ray_length / 2. * sin( t ),
						s * sin( t ) + ray_length / 2. * cos( t ) };

		vector<Tuple2D> ray_plot{ start, end };
		addSingleObject( slice_axis, "Ray", ray_plot, ";;;b--" );


		s = distances.at( distances.size() / 2 + s_index - 1);

		start = {	s * cos( t ) + ray_length / 2. * sin( t ),
						s * sin( t ) - ray_length / 2. * cos( t ) };

		end = {	s * cos( t ) - ray_length / 2. * sin( t ),
						s * sin( t ) + ray_length / 2. * cos( t ) };

		ray_plot = { start, end };
		addSingleObject( slice_axis, "Ray", ray_plot, ";;;b--" );


	}

	
	//--------------------------------------------------------------------------------------------------
	auto proj_axis = openAxis(  GetPath("verify_projections_projection_image"), true );

	format = string{  to_string(projections.data().start().c) + "," + to_string(projections.data().GetEnd().c) + ";" + to_string(projections.data().start().r) + "," + to_string(projections.data().GetEnd().r) + ";$\\theta$ in rad;$s$ in mm;ln$(J_0/J)$" };
	addSingleObject( proj_axis, "ProjectionImage", projections.data(), format, true);
	
	vector<Tuple2D> single_angle_line{ { angle, projections.data().start().r }, { angle, projections.data().GetEnd().r } };
	addSingleObject( proj_axis, "Projections", single_angle_line, ";;;b--" );

	
	//--------------------------------------------------------------------------------------------------
	auto single_proj_axis = openAxis(  GetPath("verify_projections_single_projections"), true );

	vector<double> single_projection;

	for( int s_index = 0; s_index < distances.size(); s_index++ ){	
		single_projection.push_back( projections.data().GetData( GridCoordinates{ angle, distances.at( s_index ) } ) );
	}

	addSingleObject( single_proj_axis, "SingleProjection", ConvertToTuple( VectorPair{ single_projection, distances }), 
					 "ln$(J_0/J)$;$s$ in mm;" );


	FilteredProjections filtered_projections{ projections, BackprojectionFilter::TYPE::ramLak};
	auto filt_proj_axis = openAxis(  GetPath("verify_projections_filt_projection_image"), true );

	format = string{  to_string(filtered_projections.start().c) + "," + to_string(filtered_projections.data_grid().GetEnd().c) + ";" + to_string(filtered_projections.start().r) + "," + to_string(filtered_projections.data_grid().GetEnd().r) + ";$\\theta$ in rad;$s$ in mm;$\\tilde{p_\\theta}(s)$" };
	addSingleObject( filt_proj_axis, "FiltProjectionImage", filtered_projections.data_grid(), format, true);
	single_angle_line = vector<Tuple2D>{ { angle, filtered_projections.start().r }, { angle, filtered_projections.data_grid().GetEnd().r } };
	addSingleObject( filt_proj_axis, "FiltProjections", single_angle_line, ";;;b--" );

	
	//--------------------------------------------------------------------------------------------------
	auto single_filt_proj_axis = openAxis(  GetPath("verify_projections_single_filt_projections"), true );

	vector<double> single_filt_projection;

	for( int s_index = 0; s_index < distances.size(); s_index++ ){	
		single_filt_projection.push_back( filtered_projections.data_grid().GetData(GridCoordinates{angle, distances.at(s_index)}));
	}

	addSingleObject( single_filt_proj_axis, "SingleFiltProjection", ConvertToTuple( VectorPair{ single_filt_projection, distances }), 
					 "$\\tilde{p_\\theta}(s)$;$s$ in mm;" );


	
	//--------------------------------------------------------------------------------------------------
	Backprojection backprojection{ filtered_projections };

	auto backproj_axis = openAxis(  GetPath("verify_projections_backprojection_image"), true );

	format = string{  to_string(backprojection.getGrid().start().c) + "," + to_string(backprojection.getGrid().GetEnd().c) + ";" + to_string(backprojection.getGrid().start().r) + "," + to_string(backprojection.getGrid().GetEnd().r) + ";$x$ in mm;$y$ in mm;$\\mu$ in mm$^{-1}$"};
	addSingleObject( backproj_axis, "BackProjectionImage", backprojection.getGrid(), format, true);


	//--------------------------------------------------------------------------------------------------
	
	FilteredProjections filtered_projections_constant{ projections, BackprojectionFilter::TYPE::constant};
	Backprojection backprojection_nofilt{ filtered_projections_constant };

	auto nofilt_backproj_axis = openAxis(  GetPath("verify_projections_backprojection_nofilt_image"), true );

	format = string{  to_string(backprojection_nofilt.getGrid().start().c) + "," + to_string(backprojection_nofilt.getGrid().GetEnd().c) + ";" + to_string(backprojection_nofilt.getGrid().start().r) + "," + to_string(backprojection_nofilt.getGrid().GetEnd().r) + ";$x$ in mm;$y$ in mm;$\\mu$ in mm$^{-1}$"};
	addSingleObject( nofilt_backproj_axis, "NoFiltBackProjectionImage", backprojection_nofilt.getGrid(), format, true);



	closeAxis( slice_axis );
	closeAxis( proj_axis );
	closeAxis( single_proj_axis );
	closeAxis( filt_proj_axis );
	closeAxis( single_filt_proj_axis );
	closeAxis( backproj_axis );
	closeAxis( nofilt_backproj_axis );
	VoxelData::SetArtefactImpactFactor( 10 );
}
