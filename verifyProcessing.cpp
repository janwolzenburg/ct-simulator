
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

	path model_path{ "./phantom 1.model" };
	PersistingObject<Model> model{ Model{}, model_path, true };
	Tuple3D center = PrimitiveVector3{ model.size() } / -2.;
	model.coordinate_system()->SetPrimitive( PrimitiveCoordinateSystem{ center, Tuple3D{ 1,0,0 }, Tuple3D{ 0, 1, 0 }, Tuple3D{ 0 ,0 ,1} } );

	ProjectionsProperties projections_properties{ number_of_projections, number_of_pixel, sqrt( pow( model.size().x, 2. ) + pow( model.size().y, 2. ) ) };
	PhysicalDetectorProperties physical_detector_properties{ .5, projections_properties.measuring_field_size() * 1.1, false };
	XRayTubeProperties tube_properties{ 160000., 0.2, XRayTubeProperties::Material::Thungsten, 1, true, 5000., 4 };

	CoordinateSystem* gantry_system = GlobalSystem()->CreateCopy("Gantry system");
	Gantry gantry{ gantry_system, tube_properties, projections_properties, physical_detector_properties };

	RayScattering ray_scattering{ number_of_scatter_angles, {1000., 200000.}, 32,  gantry_system->GetEz(), PI / 2. };
	TomographyProperties tomography_properties{ false, 1, 0., true, 0., };

	Tomography tomography{ tomography_properties };

	optional<Projections> projections = tomography.RecordSlice( projections_properties, gantry, model, 0. );



}