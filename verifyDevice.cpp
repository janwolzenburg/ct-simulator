#include "verifyDevice.h"
#include "verificationParameter.h"
#include "plotting.h"

#include "gantry.h"
#include "projections.h"


void BuildTestDetector( void ){

	ProjectionsProperties projections_properties{ number_of_projections, number_of_pixel, measurefield_size };
	PhysicalDetectorProperties physical_detector_properties{ 25., 650 };
	
	CoordinateSystem* detector_system = GlobalSystem()->CreateCopy("Detector system");
	detector_system->Rotate( detector_system->GetEz(), PI );
	XRayDetector detector{ detector_system, projections_properties, physical_detector_properties };
	

	auto axis = openAxis( GetPath( "test_detector" ), true );

	addObject( axis, "Pixel", detector.pixel_array(), "b", 0.325 );
	
	int number = 0;
	for( auto pixel : detector.pixel_array() ){
		addSingleObject( axis, "Normal"+to_string(number++), pixel.GetCenterNormal(), "r", physical_detector_properties.detector_focus_distance * 1.1 );
	}

	addSingleObject( axis, "Orígin", detector_system->GetOriginPoint(), "b" );


	closeAxis( axis ); 

}

void TestRadonCoordinates( void ){


	ProjectionsProperties projections_properties{ number_of_projections, number_of_pixel, measurefield_size };
	PhysicalDetectorProperties physical_detector_properties{ 25., 650 };
	XRayTubeProperties tube_properties{ 120000., 0.2, XRayTubeProperties::Material::Thungsten, 1, false, 20000., -3.5 };

	CoordinateSystem* gantry_system = GlobalSystem()->CreateCopy("Gantry system");
	CoordinateSystem* radon_system = GlobalSystem()->CreateCopy("Radon system");

	Gantry gantry{ gantry_system, tube_properties, projections_properties, physical_detector_properties };

	Projections projections{ projections_properties };
	int frames = 1;

	vector<RadonPoint> radon_points;
	for( auto pixel : gantry.pixel_array() ){
		RadonCoordinates radon_coordinate{ radon_system, pixel.GetCenterNormal() };
		radon_points.emplace_back( radon_coordinate, 1. );
	}

	auto axis = openAxis( GetPath( "test_radon_coordinates_1" ), true );
	addSingleObject( axis, "RadonPoint", projections.data(), "\\Theta in rad;s in mm;Status;Circ", false );
	addSingleObject( axis, "HitRadonPoint1", radon_points, "{\\it \\Theta} in rad;{\\it s} in mm;Status;Dots" );
	closeAxis( axis );



	for( size_t i = 0; i < 5; i++ ){
		gantry.RotateCounterClockwise( projections_properties.angles_resolution() );
		frames++;
		for( auto pixel : gantry.pixel_array() ){
			RadonCoordinates radon_coordinate{ radon_system, pixel.GetCenterNormal() };
			radon_points.emplace_back( radon_coordinate, 1. );
		}
	}

	axis = openAxis( GetPath( "test_radon_coordinates_2" ), true );
	addSingleObject( axis, "RadonPoint", projections.data(), "\\Theta in rad;s in mm;Status;Circ", false );	
	addSingleObject( axis, "HitRadonPoint2", radon_points, "{\\it \\Theta} in rad;{\\it s} in mm;Status;Dots" );
	closeAxis( axis ); 

	
	
	for( size_t i = 0; i < 5; i++ ){
		gantry.RotateCounterClockwise( projections_properties.angles_resolution() );
		frames++;
		for( auto pixel : gantry.pixel_array() ){
			RadonCoordinates radon_coordinate{ radon_system, pixel.GetCenterNormal() };
			radon_points.emplace_back( radon_coordinate, 1. );
		}
	}

	
	axis = openAxis( GetPath( "test_radon_coordinates_3" ), true );
	addSingleObject( axis, "RadonPoint", projections.data(), "\\Theta in rad;s in mm;Status;Circ", false );
	addSingleObject( axis, "HitRadonPoint3", radon_points, "{\\it \\Theta} in rad;{\\it s} in mm;Status;Dots" );
	closeAxis( axis ); 

	
	size_t max_frames = projections_properties.number_of_frames_to_fill();
	while( frames < max_frames - 1 ){
		gantry.RotateCounterClockwise( projections_properties.angles_resolution() );
		frames++;
		for( auto pixel : gantry.pixel_array() ){
			RadonCoordinates radon_coordinate{ radon_system, pixel.GetCenterNormal() };
			radon_points.emplace_back( radon_coordinate, 1. );
		}
	}

	axis = openAxis( GetPath( "test_radon_coordinates_4" ), true );
	addSingleObject( axis, "RadonPoint", projections.data(), "\\Theta in rad;s in mm;Status;Circ", false );
	addSingleObject( axis, "HitRadonPoint4", radon_points, "{\\it \\Theta} in rad;{\\it s} in mm;Status;Dots" );
	closeAxis( axis );  

}