
#include "verifyprocessing.h"
#include "verificationParameter.h"
#include "plotting.h"
#include "model.h"
#include "persistingObject.h"
#include "gantry.h"
#include "tomography.h"
#include "projectionsProperties.h"
#include "projections.h"

void VerifyFilteredprojections( void ){

	path model_path{ "./phantom 2.model" };
	PersistingObject<Model> model{ Model{}, model_path, true };
	Tuple3D center = PrimitiveVector3{ model.size() } / -2.;
	model.coordinate_system()->SetPrimitive( PrimitiveCoordinateSystem{ center, Tuple3D{ 1,0,0 }, Tuple3D{ 0, 1, 0 }, Tuple3D{ 0 ,0 ,1} } );

	ProjectionsProperties projections_properties{ 1*number_of_projections, 1*number_of_pixel, sqrt( pow( model.size().x, 2. ) + pow( model.size().y, 2. ) ) };
	PhysicalDetectorProperties physical_detector_properties{ .5, projections_properties.measuring_field_size() * 1.1, false };
	XRayTubeProperties tube_properties{ 160000., 0.2, XRayTubeProperties::Material::Thungsten, 1, true, 5000., 4 };

	CoordinateSystem* gantry_system = GlobalSystem()->CreateCopy("Gantry system");
	Gantry gantry{ gantry_system, tube_properties, projections_properties, physical_detector_properties };

	RayScattering ray_scattering{ number_of_scatter_angles, {1000., 200000.}, 32,  gantry_system->GetEz(), PI / 2. };
	TomographyProperties tomography_properties{ false, 1, 0., true, 0., };

	Tomography tomography{ tomography_properties };
	VoxelData::SetArtefactImpactFactor( 0 );

	Surface slice_surface{ gantry.coordinate_system()->GetEx(), gantry.coordinate_system()->GetEy(), gantry.coordinate_system()->GetOriginPoint() };
	auto slice = model.GetSlice( slice_surface, GridIndex{ model.number_of_voxel_3D().x, model.number_of_voxel_3D().y }, GridCoordinates{ model.voxel_size().x, model.voxel_size().y } );
	

	auto slice_axis = openAxis(  GetPath("phantom_slice_image"), true );
	string format{ to_string(slice.start().c) + "," + to_string(slice.GetEnd().c) + ";" + to_string(slice.start().r) + "," + to_string(slice.GetEnd().c) + ";$x$ in mm;$y$ in mm;$\\mu_a$ in mm$^{-1}$"};
	addSingleObject( slice_axis, "PhantomSliceImage", slice.GetDoubleGrid(), format, true);
	

	optional<Projections> projections = tomography.RecordSlice( projections_properties, gantry, model, 0. );

	vector<double> distances =  CreateLinearSpace(	projections.value().data().start().r, 
													projections.value().data().GetEnd().r, 
													projections.value().data().size().r );
	const double angle = PI / 2. * 0.3;
	const double t  = angle;
	const double ray_length = 480;

	for( int s_index = 0; s_index < distances.size() / 2; s_index++ ){
		

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


	auto proj_axis = openAxis(  GetPath("verify_projections_projection_image"), true );

	format = string{  to_string(projections.value().data().start().c) + "," + to_string(projections.value().data().GetEnd().c) + ";" + to_string(projections.value().data().start().r) + "," + to_string(projections.value().data().GetEnd().r) + ";$\\theta$ in rad;$s$ in mm;ln$(I_0/I)$" };
	addSingleObject( proj_axis, "ProjectionImage", projections.value().data(), format, true);
	
	vector<Tuple2D> single_angle_line{ { angle, projections.value().data().start().r }, { angle, projections.value().data().GetEnd().r } };
	addSingleObject( proj_axis, "Projections", single_angle_line, ";;;b--" );


	auto single_proj_axis = openAxis(  GetPath("verify_projections_single_projections"), true );

	vector<double> single_projection;

	for( int s_index = 0; s_index < distances.size(); s_index++ ){
		
		single_projection.push_back( projections.value().data().GetData( GridCoordinates{ angle, distances.at( s_index ) } ) );
	
	}

	addSingleObject( single_proj_axis, "SingleProjection", ConvertToTuple( VectorPair{ distances, single_projection }), 
					 "$s$ in mm;ln$(I_0/I)$;" );

	closeAxis( slice_axis );
	closeAxis( proj_axis );
	closeAxis( single_proj_axis );
	VoxelData::SetArtefactImpactFactor( 10 );
}