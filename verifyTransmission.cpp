#include "verifyTransmission.h"
#include "verificationParameter.h"
#include "plotting.h"

#include "model.h"
#include "persistingObject.h"
#include "gantry.h"
#include "tomography.h"

void TransmitAndDraw( const Model& model, Ray& ray, TomographyProperties& tomography_properties, RayScattering& ray_scattering, ofstream& axis, ofstream& simple_intensities_axis, ofstream& intensities_axis, ofstream& simple_delta_axis, ofstream& delta_axis, bool add_inout ){

	pair<Ray, vector<Ray>> returned_rays = model.TransmitRay( ray, tomography_properties , ray_scattering, true );

	vector<Tuple2D> simple_intensity_values;
	vector<Tuple2D> intensity_values;

	vector<Tuple2D> simple_deltas;
	vector<Tuple2D> deltas;

	double current_step = 0.;

	Ray transmitted_ray = returned_rays.first;
	bool first = true;

	for( RayTrace::TracingStep step : transmitted_ray.properties().ray_tracing.tracing_steps ){

		if( !step.inside_model ) continue;

		if( first ){
			first = false;
			
			simple_intensity_values.emplace_back( current_step, 1. );
			intensity_values.emplace_back( current_step, transmitted_ray.properties().start_intensity() );
			current_step += step.distance;

			simple_intensity_values.emplace_back( current_step, 1. );
			intensity_values.emplace_back( current_step, transmitted_ray.properties().start_intensity() );
			current_step += step.distance;
			
			simple_deltas.emplace_back( current_step,-log(simple_intensity_values.back().y / (simple_intensity_values.end() - 2)->y ) / step.distance );
			deltas.emplace_back( current_step, -log( intensity_values.back().y / (intensity_values.end() - 2)->y ) / step.distance );
			
			simple_intensity_values.emplace_back( current_step, 1. );
			intensity_values.emplace_back( current_step, transmitted_ray.properties().start_intensity() );
			current_step += step.distance;

			simple_deltas.emplace_back( current_step,-log(simple_intensity_values.back().y / (simple_intensity_values.end() - 2)->y ) / step.distance );
			deltas.emplace_back( current_step, -log( intensity_values.back().y / (intensity_values.end() - 2)->y ) / step.distance );
		}

		if( add_inout )
			addSingleObject( axis, "InOut", step.entrance, "m" );

		simple_intensity_values.emplace_back( current_step, step.current_simple_intensity );
		intensity_values.emplace_back( current_step, step.current_intensity );

		simple_deltas.emplace_back( current_step, -log( simple_intensity_values.back().y / (simple_intensity_values.end() - 2)->y ) / step.distance );
		deltas.emplace_back( current_step, -log( intensity_values.back().y / (intensity_values.end() - 2)->y ) / step.distance );
		
		current_step += step.distance;
	}
	
	simple_intensity_values.emplace_back( current_step, simple_intensity_values.back().y );
	intensity_values.emplace_back( current_step, intensity_values.back().y );

	addSingleObject( simple_intensities_axis, "SimpleIntensity", simple_intensity_values, "$d$ in mm;$I$;Step" );
	addSingleObject( intensities_axis, "Intensity", intensity_values, "$d$ in mm;$I$ in W;Step");

	addSingleObject( simple_delta_axis, "SimpleIntensityDelta", simple_deltas, "$d$ in mm;ln$(I\\cdot I_0^{-1})\\cdot\\Delta d^{-1}$ in mm$^{-1}$;Step" );
	addSingleObject( delta_axis, "IntensityDelta", deltas, "$d$ in mm;ln$(I\\cdot I_0^{-1})\\cdot\\Delta d^{-1}$ in mm$^{-1}$;Step" );
}

void VerifyTransmission( void ){

	path model_path{ "./verification.model" };

	PersistingObject<Model> model{ Model{}, model_path, true };
	
	Tuple3D center = PrimitiveVector3{ model.size() } / -2.;
	model.coordinate_system()->SetPrimitive( PrimitiveCoordinateSystem{ center, Tuple3D{ 1,0,0 }, Tuple3D{ 0, 1, 0 }, Tuple3D{ 0 ,0 ,1} } );


	ProjectionsProperties projections_properties{ number_of_projections, 12, 1.1 * sqrt( pow( model.size().x, 2. ) + pow( model.size().y, 2. ))};
	PhysicalDetectorProperties physical_detector_properties{ 25., projections_properties.measuring_field_size()*1.75 };
	XRayTubeProperties tube_properties{ 120000., 0.2, XRayTubeProperties::Material::Thungsten, 1, true, 5000., 4 };

	CoordinateSystem* gantry_system = GlobalSystem()->CreateCopy("Gantry system");

	Gantry gantry{ gantry_system, tube_properties, projections_properties, physical_detector_properties };
	auto rays = gantry.tube().GetEmittedBeam( gantry.pixel_array(), gantry.detector().properties().detector_focus_distance );

	Surface slice_surface{ gantry.coordinate_system()->GetEx(), gantry.coordinate_system()->GetEy(), gantry.coordinate_system()->GetOriginPoint() };
	auto slice = model.GetSlice( slice_surface, GridIndex{ model.number_of_voxel_3D().x, model.number_of_voxel_3D().y }, GridCoordinates{ model.voxel_size().x, model.voxel_size().y } );
	
	
	auto axis = openAxis( GetPath( "test_transmission" ), true );
	
	for( size_t c = 0; c < slice.size().c; c++ ){
		for( size_t r = 0; r < slice.size().r; r++ ){
			
			const double range = slice.max_value().GetAbsorptionAtReferenceEnergy() - slice.min_value().GetAbsorptionAtReferenceEnergy();
			double value;
			if( range == 0 ){
				value = 1.;
			}
			else
				value = ( slice.GetData( GridIndex{ c, r } ).GetAbsorptionAtReferenceEnergy() - slice.min_value().GetAbsorptionAtReferenceEnergy() ) / 
							range;
		
			const BoundedSurface voxel_surface{ Surface{ gantry_system->GetEx(), gantry_system->GetEy(), gantry_system->GetOriginPoint() + Vector3D{ Tuple3D{ slice.GetColCoordinate( c ), slice.GetRowCoordinate(r) , 0.,}, gantry_system}}, -slice.resolution().c/2., slice.resolution().c/2., -slice.resolution().r/2., slice.resolution().r/2.};


			addSingleObject( axis, "Voxel", voxel_surface, "b", value * 0.6 + 0.3 );

		}
	}

	for( auto ray : rays ){
		addSingleObject( axis, "Ray", ray, "r", gantry.detector().properties().detector_focus_distance );
	}

	Ray ray = rays.at( 0 );
	addSingleObject( axis, "Ray", ray, "g", gantry.detector().properties().detector_focus_distance);
	
	RayScattering ray_scattering{ number_of_scatter_angles, {1000., 200000.}, 32,  gantry_system->GetEz() };
	TomographyProperties tomography_properties{ false, 1, 0., true, 0. };
	
	
	
	auto simple_intensities_axis = openAxis( GetPath( "test_transmission_simple_intensity" ), true );
	auto intensities_axis = openAxis( GetPath( "test_transmission_intensity" ), true );
	auto simple_delta_axis = openAxis( GetPath( "test_transmission_simple_intensity_delta" ), true );
	auto delta_axis = openAxis( GetPath( "test_transmission_intensity_delta" ), true );

	TransmitAndDraw( model, ray, tomography_properties, ray_scattering, axis, simple_intensities_axis, intensities_axis, simple_delta_axis, delta_axis, true );
	
	closeAxis( simple_intensities_axis );
	closeAxis( intensities_axis );
	closeAxis( simple_delta_axis );
	closeAxis( delta_axis );
	closeAxis( axis );
}

void VerifyHardening( void ){
	
	path model_path{ "./verification only water.model" };
	PersistingObject<Model> model{ Model{}, model_path, true };
	Tuple3D center = PrimitiveVector3{ model.size() } / -2.;
	model.coordinate_system()->SetPrimitive( PrimitiveCoordinateSystem{ center, Tuple3D{ 1,0,0 }, Tuple3D{ 0, 1, 0 }, Tuple3D{ 0 ,0 ,1} } );

	ProjectionsProperties projections_properties{ number_of_projections, 12, 200 };
	PhysicalDetectorProperties physical_detector_properties{ 25., 400 };
	XRayTubeProperties tube_properties{ 120000., 0.2, XRayTubeProperties::Material::Thungsten, 1, true, 5000., 4 };

	CoordinateSystem* gantry_system = GlobalSystem()->CreateCopy("Gantry system");
	Gantry gantry{ gantry_system, tube_properties, projections_properties, physical_detector_properties };
	auto rays = gantry.tube().GetEmittedBeam( gantry.pixel_array(), gantry.detector().properties().detector_focus_distance );

	
	RayScattering ray_scattering{ number_of_scatter_angles, {1000., 200000.}, 32,  gantry_system->GetEz() };
	TomographyProperties tomography_properties{ false, 1, 0., true, 0. };

	Surface slice_surface{ gantry.coordinate_system()->GetEx(), gantry.coordinate_system()->GetEy(), gantry.coordinate_system()->GetOriginPoint() };
	auto slice = model.GetSlice( slice_surface, GridIndex{ model.number_of_voxel_3D().x, model.number_of_voxel_3D().y }, GridCoordinates{ model.voxel_size().x, model.voxel_size().y } );
	
	
	auto axis = openAxis( GetPath( "test_hardening" ), true );
	
	for( size_t c = 0; c < slice.size().c; c++ ){
		for( size_t r = 0; r < slice.size().r; r++ ){
			
			const double range = slice.max_value().GetAbsorptionAtReferenceEnergy() - slice.min_value().GetAbsorptionAtReferenceEnergy();
			double value;
			if( range == 0 ){
				value = 1.;
			}
			else
				value = ( slice.GetData( GridIndex{ c, r } ).GetAbsorptionAtReferenceEnergy() - slice.min_value().GetAbsorptionAtReferenceEnergy() ) / 
							range;
		
			const BoundedSurface voxel_surface{ Surface{ gantry_system->GetEx(), gantry_system->GetEy(), gantry_system->GetOriginPoint() + Vector3D{ Tuple3D{ slice.GetColCoordinate( c ), slice.GetRowCoordinate(r) , 0.,}, gantry_system}},-slice.resolution().c/2., slice.resolution().c/2., -slice.resolution().r/2., slice.resolution().r/2.};
			addSingleObject( axis, "Voxel", voxel_surface, "b", value * 0.6 + 0.3 );
		}
	}

	for( auto ray : rays ){
		addSingleObject( axis, "Ray", ray, "r", gantry.detector().properties().detector_focus_distance );
	}

	
	
	ofstream simple_intensities_axis;// = openAxis( GetPath( "test_transmission_simple_intensity" ), true );
	ofstream intensities_axis;// = openAxis( GetPath( "test_transmission_intensity" ), true );
	ofstream simple_delta_axis;// = openAxis( GetPath( "test_transmission_simple_intensity_delta" ), true );
	ofstream delta_axis;// = openAxis( GetPath( "test_transmission_intensity_delta" ), true );

	for( auto i = 0; i < 3; i++ ){
		simple_intensities_axis =	openAxis( GetPath( string{"test_hardening_simple_intensity_" + to_string(i) }), true);
		intensities_axis =			openAxis( GetPath( string{"test_hardening_intensity_" + to_string(i) } ), true );
		simple_delta_axis =			openAxis( GetPath( string{"test_hardening_simple_intensity_delta_" + to_string(i) } ), true );
		delta_axis =				openAxis( GetPath( string{"test_hardening_intensity_delta_" + to_string(i) } ), true );

	

		Ray ray = rays.at( 2*i );
		addSingleObject( axis, "Ray", ray, "g", gantry.detector().properties().detector_focus_distance);

		TransmitAndDraw( model, ray, tomography_properties, ray_scattering, axis, simple_intensities_axis, intensities_axis, simple_delta_axis, delta_axis, false );
		
		closeAxis( simple_intensities_axis );
		closeAxis( intensities_axis );
		closeAxis( simple_delta_axis );
		closeAxis( delta_axis );
	}


	closeAxis( axis );
}

void VerifyScattering( void ){

	path model_path{ "./verification only water.model" };
	PersistingObject<Model> model{ Model{}, model_path, true };
	Tuple3D center = PrimitiveVector3{ model.size() } / -2.;
	model.coordinate_system()->SetPrimitive( PrimitiveCoordinateSystem{ center, Tuple3D{ 1,0,0 }, Tuple3D{ 0, 1, 0 }, Tuple3D{ 0 ,0 ,1} } );

	ProjectionsProperties projections_properties{8, 4, 100 };
	PhysicalDetectorProperties physical_detector_properties{ 25., 400 };
	XRayTubeProperties tube_properties{ 200000, 0.2, XRayTubeProperties::Material::Thungsten, 1, false, 5000., 4 };

	CoordinateSystem* gantry_system = GlobalSystem()->CreateCopy("Gantry system");
	Gantry gantry{ gantry_system, tube_properties, projections_properties, physical_detector_properties };
	auto rays = gantry.tube().GetEmittedBeam( gantry.pixel_array(), gantry.detector().properties().detector_focus_distance );

	RayScattering ray_scattering{ number_of_scatter_angles,  gantry.tube().GetEmittedEnergyRange(), number_of_energies_for_scattering, gantry_system->GetEz() };

	TomographyProperties tomography_properties{ true, 1, 1., true, 1. };

	Surface slice_surface{ gantry.coordinate_system()->GetEx(), gantry.coordinate_system()->GetEy(), gantry.coordinate_system()->GetOriginPoint() };
	auto slice = model.GetSlice( slice_surface, GridIndex{ model.number_of_voxel_3D().x, model.number_of_voxel_3D().y }, GridCoordinates{ model.voxel_size().x, model.voxel_size().y } );
	
	auto axis = openAxis( GetPath( "test_scattering" ), true );
	
	for( size_t c = 0; c < slice.size().c; c++ ){
		for( size_t r = 0; r < slice.size().r; r++ ){
			
			const double range = slice.max_value().GetAbsorptionAtReferenceEnergy() - slice.min_value().GetAbsorptionAtReferenceEnergy();
			double value;
			if( range == 0 ){
				value = 1.;
			}
			else
				value = ( slice.GetData( GridIndex{ c, r } ).GetAbsorptionAtReferenceEnergy() - slice.min_value().GetAbsorptionAtReferenceEnergy() ) / 
							range;
		
			const BoundedSurface voxel_surface{ Surface{ gantry_system->GetEx(), gantry_system->GetEy(), gantry_system->GetOriginPoint() + Vector3D{ Tuple3D{ slice.GetColCoordinate( c ), slice.GetRowCoordinate(r) , 0.,}, gantry_system}}, -slice.resolution().c/2., slice.resolution().c/2., -slice.resolution().r/2., slice.resolution().r/2.};
			addSingleObject( axis, "Voxel", voxel_surface, "b", value * 0.6 + 0.3 );
		}
	}
	

	Ray ray = rays.at( 0 );
	
	DetectorPixel pixel{ BoundedSurface{ ray.direction().GetCoordinateSystem()->GetEz(), ray.direction().GetCoordinateSystem()->GetEz() ^ ray.direction(),
										 ray.GetPoint( gantry.detector().properties().detector_focus_distance ),
										 -1.,1.,-1.,1. } };

	addSingleObject( axis, "Pixel", static_cast<BoundedSurface>( pixel ), "g", .5 );

	const RayPixelIntersection pixelHit{ ray, pixel };
		if( pixelHit.intersection_exists_ ){
			pixel.AddDetectedRayProperties( ray.properties() );
		}

	pair<Ray, vector<Ray>> returned_rays = model.TransmitRay( ray, tomography_properties , ray_scattering, false );
	addSingleObject( axis, "Ray", ray, "r", gantry.detector().properties().detector_focus_distance );



	for( auto scattered_ray : returned_rays.second ){
		addSingleObject( axis, "Scattered ray", scattered_ray, "g", gantry.detector().properties().detector_focus_distance );
	}

	
	closeAxis( axis );


	vector<Tuple2D> attenuations = ray.properties().energy_spectrum().data();
	for( auto& [energy, attenuation] : attenuations ){
		attenuation = 0.;
	}

	double scattered_rays_power_sum = 0;
	double mu_times_d_sum = 0.;

	size_t ray_iterations = 1000; size_t num_scattered_rays = 0;
	for( auto it = 0; it < ray_iterations; it++ ){

		pair<Ray, vector<Ray>> returned_rays_loc = model.TransmitRay( ray, tomography_properties , ray_scattering, false );

		
		
		for( auto& [energy, attenuation] : attenuations ){
			returned_rays_loc.first.properties().only_scattering_spectrum.GetPhotonflow()
		}
	}
	
	double mean_scattered_ray_power = scattered_rays_power_sum / num_scattered_rays;
	double mu_times_d = mu_times_d_sum / num_scattered_rays ;


	// 0.06 vs 0.015: Becaus main ray's energy is not attenuated and larger chunks are scattered

	vector<ofstream> angles_axis;
	const int num_energies = 2;

	for( int i = 0; i < num_energies; i++ ){
		angles_axis.emplace_back( std::move( openAxis( GetPath( string{"test_scattering_angles_e_"} + to_string( i + 1 ) ), true ) ) );
	}

	vector<double> energies = CreateLinearSpace( 10000., 210000., num_energies );
	
	array<VectorPair, num_energies> angles;
	for( auto& angles_element : angles ){
		angles_element = VectorPair{ CreateLinearSpace( 0., PI, ( number_of_scatter_angles + 1) / 2) , vector<double>( (number_of_scatter_angles + 1) / 2, 0. ) };
	}
	
	double angle_resolution = angles.at(0).first.at(1) - angles.at(0).first.at(0);

	size_t num_iterations = 100000;
	for( int i = 0; i < num_iterations; i++ ){

		for( int e_i = 0; e_i < num_energies; e_i++ ){
			
			const double angle = abs(ray_scattering.GetRandomAngle( energies.at( e_i ) ) );
			size_t angle_index = static_cast<size_t>( floor(angle / angle_resolution + 0.5) );//GetClosestElementIndex( angles.at(e_i).first, angle ) ;
			angles.at(e_i).second.at( ForceToMax( angle_index, angles.at(e_i).second.size() - 1 ) )++;
		}

	}

	for( auto& angles_element : angles ){
		for( auto& angle : angles_element.second )
			angle /= num_iterations * 0.01;
	}

	for( int e_i = 0; e_i < num_energies; e_i++ ){
		addSingleObject( angles_axis.at(e_i), "Angles", ConvertToTuple(angles.at(e_i)), ";;Dots", 1 );
		closeAxis( angles_axis.at(e_i) );
	}



	
	vector<Tuple2D> scatterings = ray.properties().energy_spectrum().data();
	for( auto e_i = 0; e_i < scatterings.size(); e_i++ ){
		scatterings.at( e_i ).y = 0.;
	}
	
	const double coefficient_factor = 1.;

	const size_t iterations = 100000;
	for( size_t i = 0; i < iterations; i++ ){


		for( auto& [energy, number] : scatterings ){

			const double cross_section_mm = ScatteringCrossSection::GetInstance().GetCrossSection( energy );
			const double coefficient_1Permm = cross_section_mm * electron_density_water_1Permm3 * coefficient_factor;
			const double scatter_propability = 1. - exp( -coefficient_1Permm * 1. );

			if( integer_random_number_generator.DidARandomEventHappen( scatter_propability  ) )
				number++;
		}

	}

	for( auto& [energy, number] : scatterings ){
		number /= static_cast<double>( iterations );
		number = -log( 1. - number ) * 1.e27 / 100. / coefficient_factor / electron_density_water_1Permm3;
	}

	auto propability_axis = openAxis( GetPath( string{"test_scattering_propability"} ), true );
	addSingleObject( propability_axis, "Propability", scatterings, "$E$ in eV;$\\sigma_s$ in $10^{-27}$ cm$^2$;Dots", 2);
	closeAxis( propability_axis );
}