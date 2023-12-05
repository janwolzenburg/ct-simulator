#include "verifyTransmission.h"
#include "verificationParameter.h"
#include "plotting.h"

#include "model.h"
#include "persistingObject.h"
#include "gantry.h"

void VerifyTransmission( void ){

	path model_path{ "./verification.model" };

	PersistingObject<Model> model{ Model{}, model_path, true };
	
	Tuple3D center = PrimitiveVector3{ model.size() } / -2.;
	model.coordinate_system()->SetPrimitive( PrimitiveCoordinateSystem{ center, Tuple3D{ 1,0,0 }, Tuple3D{ 0, 1, 0 }, Tuple3D{ 0 ,0 ,1} } );


	ProjectionsProperties projections_properties{ number_of_projections, number_of_pixel, measurefield_size };
	PhysicalDetectorProperties physical_detector_properties{ 25., 650 };
	XRayTubeProperties tube_properties{ 120000., 0.2, XRayTubeProperties::Material::Thungsten, 1, false, 20000., 3.5 };

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
		
			const BoundedSurface voxel_surface{ Surface{ gantry_system->GetEx(), gantry_system->GetEy(), gantry_system->GetOriginPoint() + Vector3D{ Tuple3D{ slice.GetColCoordinate( c ) - slice.resolution().c, slice.GetRowCoordinate(r) - slice.resolution().r, 0.,}, gantry_system}}, 0, slice.resolution().c, 0, slice.resolution().r};


			addSingleObject( axis, "Voxel", voxel_surface, "b", value );

		}
	}

	closeAxis( axis );
}