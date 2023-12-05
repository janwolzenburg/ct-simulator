#include "verifyTransmission.h"
#include "verificationParameter.h"
#include "plotting.h"

#include "model.h"
#include "persistingObject.h"
#include "gantry.h"
#include "tomography.h"

void TransmitAndDraw( const Model& model, Ray& ray, TomographyProperties& tomography_properties, RayScattering& ray_scattering, ofstream& axis, ofstream& simple_intensities_axis, ofstream& intensities_axis, ofstream& simple_delta_axis, ofstream& delta_axis ){

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
		
			const BoundedSurface voxel_surface{ Surface{ gantry_system->GetEx(), gantry_system->GetEy(), gantry_system->GetOriginPoint() + Vector3D{ Tuple3D{ slice.GetColCoordinate( c ), slice.GetRowCoordinate(r) , 0.,}, gantry_system}}, 0, slice.resolution().c, 0, slice.resolution().r};


			addSingleObject( axis, "Voxel", voxel_surface, "b", value * 0.6 + 0.3 );

		}
	}

	for( auto ray : rays ){
		addSingleObject( axis, "Ray", ray, "r", gantry.detector().properties().detector_focus_distance );
	}

	Ray ray = rays.at( 0 );
	addSingleObject( axis, "Ray", ray, "g", gantry.detector().properties().detector_focus_distance);
	
	RayScattering ray_scattering{ 32, {1000., 200000.}, 32,  gantry_system->GetEz() };
	TomographyProperties tomography_properties{ false, 1, 0., true, 0. };
	
	
	
	auto simple_intensities_axis = openAxis( GetPath( "test_transmission_simple_intensity" ), true );
	auto intensities_axis = openAxis( GetPath( "test_transmission_intensity" ), true );
	auto simple_delta_axis = openAxis( GetPath( "test_transmission_simple_intensity_delta" ), true );
	auto delta_axis = openAxis( GetPath( "test_transmission_intensity_delta" ), true );

	TransmitAndDraw( model, ray, tomography_properties, ray_scattering, axis, simple_intensities_axis, intensities_axis, simple_delta_axis, delta_axis );
	
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
	PhysicalDetectorProperties physical_detector_properties{ 25., projections_properties.measuring_field_size()*1.75 };
	XRayTubeProperties tube_properties{ 120000., 0.2, XRayTubeProperties::Material::Thungsten, 1, true, 5000., 4 };

	CoordinateSystem* gantry_system = GlobalSystem()->CreateCopy("Gantry system");

	Gantry gantry{ gantry_system, tube_properties, projections_properties, physical_detector_properties };
	auto rays = gantry.tube().GetEmittedBeam( gantry.pixel_array(), gantry.detector().properties().detector_focus_distance );

	
	RayScattering ray_scattering{ 32, {1000., 200000.}, 32,  gantry_system->GetEz() };
	TomographyProperties tomography_properties{ false, 1, 0., true, 0. };

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
		
			const BoundedSurface voxel_surface{ Surface{ gantry_system->GetEx(), gantry_system->GetEy(), gantry_system->GetOriginPoint() + Vector3D{ Tuple3D{ slice.GetColCoordinate( c ), slice.GetRowCoordinate(r) , 0.,}, gantry_system}}, 0, slice.resolution().c, 0, slice.resolution().r};


			addSingleObject( axis, "Voxel", voxel_surface, "b", value * 0.6 + 0.3 );

		}
	}

	for( auto ray : rays ){
		addSingleObject( axis, "Ray", ray, "r", gantry.detector().properties().detector_focus_distance );
	}

	Ray ray = rays.at( 0 );
	addSingleObject( axis, "Ray", ray, "g", gantry.detector().properties().detector_focus_distance);
	
	ofstream simple_intensities_axis;// = openAxis( GetPath( "test_transmission_simple_intensity" ), true );
	ofstream intensities_axis;// = openAxis( GetPath( "test_transmission_intensity" ), true );
	ofstream simple_delta_axis;// = openAxis( GetPath( "test_transmission_simple_intensity_delta" ), true );
	ofstream delta_axis;// = openAxis( GetPath( "test_transmission_intensity_delta" ), true );

	for( auto i = 0; i < 3; i++ ){
		simple_intensities_axis =	openAxis( GetPath( string{"test_hardening_simple_intensity_" + to_string(i) }), true);
		intensities_axis =			openAxis( GetPath( string{"test_hardening_intensity_" + to_string(i) } ), true );
		simple_delta_axis =			openAxis( GetPath( string{"test_hardening_simple_intensity_delta_" + to_string(i) } ), true );
		delta_axis =				openAxis( GetPath( string{"test_hardening_intensity_delta_" + to_string(i) } ), true );

	

		TransmitAndDraw( model, ray, tomography_properties, ray_scattering, axis, simple_intensities_axis, intensities_axis, simple_delta_axis, delta_axis );
		
		closeAxis( simple_intensities_axis );
		closeAxis( intensities_axis );
		closeAxis( simple_delta_axis );
		closeAxis( delta_axis );
		closeAxis( axis );
	}



}