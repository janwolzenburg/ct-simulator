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
	XRayTubeProperties tube_properties{ 140000., 0.5, XRayTubeProperties::Material::Thungsten, 1, false, 16000., 3.5 };

	CoordinateSystem* gantry_system = GlobalSystem()->CreateCopy("Gantry system");
	CoordinateSystem* radon_system = GlobalSystem()->CreateCopy("Radon system");

	Gantry gantry{ gantry_system, tube_properties, projections_properties, physical_detector_properties };

	Projections projections{ projections_properties, TomographyProperties{} };
	int frames = 1;

	vector<RadonPoint> radon_points;
	for( auto pixel : gantry.pixel_array() ){
		RadonCoordinates radon_coordinate{ radon_system, pixel.GetCenterNormal() };
		radon_points.emplace_back( radon_coordinate, 1. );
	}

	auto axis = openAxis( GetPath( "test_radon_coordinates_1" ), true );
	addSingleObject( axis, "RadonPoint", projections.data(), "$\\theta$ in rad;$s$ in mm;Status;Circ", false );
	addSingleObject( axis, "HitRadonPoint1", radon_points, "$\\theta$ in rad;$s$ in mm;Status;Dots" );
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
	addSingleObject( axis, "RadonPoint", projections.data(), "$\\theta$ in rad;$s$ in mm;Status;Circ", false );	
	addSingleObject( axis, "HitRadonPoint2", radon_points, "$\\theta$ in rad;$s$ in mm;Status;Dots" );
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
	addSingleObject( axis, "RadonPoint", projections.data(), "$\\theta$ in rad;$s$ in mm;Status;Circ", false );
	addSingleObject( axis, "HitRadonPoint3", radon_points, "$\\theta$ in rad;$s$ in mm;Status;Dots" );
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
	addSingleObject( axis, "RadonPoint", projections.data(), "$\\theta$ in rad;$s$ in mm;Status;Circ", false );
	addSingleObject( axis, "HitRadonPoint4", radon_points, "$\\theta$ in rad;$s$ in mm;Status;Dots" );
	closeAxis( axis );  

}


void TestSpectrum( void ){


	XRayTubeProperties tube_properties{ 140000., 0.5, XRayTubeProperties::Thungsten, 1, false, 16000., 3.5 };
	
	CoordinateSystem* tube_system = GlobalSystem()->CreateCopy("Tube system");
	
	XRayTube tube{ tube_system, tube_properties };

	vector<Tuple2D> photon_counts = ConvertToTuple( tube.GetEnergySpectrumPoints() );
	auto axis = openAxis( GetPath( "test_spectrum_photoncount" ), true );
	addSingleObject( axis, "Spektrum", photon_counts, "$E$ in eV;$N_P$ in s$^{-1}$;Dots");
	closeAxis( axis );  

	
	vector<Tuple2D> energy;
	for( const auto& photons : photon_counts ){
		energy.emplace_back( photons.x, photons.y * photons.x / tube.GetSpectralEnergyResolution() * J_Per_eV );
	}

	axis = openAxis( GetPath( "test_spectrum_energy" ), true );
	addSingleObject( axis, "Spektrum", energy, "$E$ in eV;$P_S$ in W$\\cdot$eV$^{-1}$;" );
	closeAxis( axis );  


	XRayTubeProperties tube_properties_filter{ 140000., 0.4, XRayTubeProperties::Thungsten, 1, true, 16000., 3.5 };
	
	CoordinateSystem* tube_system_filter = GlobalSystem()->CreateCopy("Tube with filter system");
	
	XRayTube tube_filter{ tube_system_filter, tube_properties_filter };

	photon_counts = ConvertToTuple( tube_filter.GetEnergySpectrumPoints() );
	axis = openAxis( GetPath( "test_spectrum_photoncount_filter" ), true );
	addSingleObject( axis, "Spektrum", photon_counts, "$E$ in eV;$N_P$ in s$^{-1}$;Dots");
	closeAxis( axis );  

	
	energy.clear();
	for( const auto& photons : photon_counts ){
		energy.emplace_back( photons.x, photons.y * photons.x / tube_filter.GetSpectralEnergyResolution() * J_Per_eV );
	}

	axis = openAxis( GetPath( "test_spectrum_energy_filter" ), true );
	addSingleObject( axis, "Spektrum", energy, "$E$ in eV;$P_S$ in W$\\cdot$eV$^{-1}$;" );
	closeAxis( axis );  

}


void TestBeamCreation( void ){

	ProjectionsProperties projections_properties{ number_of_projections, number_of_pixel, measurefield_size };
	PhysicalDetectorProperties physical_detector_properties{ 25., 650 };
	XRayTubeProperties tube_properties{ 140000., 0.5, XRayTubeProperties::Material::Thungsten, 3, false, 20000., 3.5 };

	CoordinateSystem* gantry_system = GlobalSystem()->CreateCopy("Gantry system");

	Gantry gantry{ gantry_system, tube_properties, projections_properties, physical_detector_properties };
	gantry.RotateCounterClockwise( PI / 2. );
	//auto beams = gantry.tube().GetEmittedBeam( gantry.pixel_array(), measurefield_size * 1.2 );

	auto axis = openAxis( GetPath( "test_beam" ), true );
	addObject( axis, "beam", gantry, "r", GANTRY_SPECIFIERS::BEAMS | GANTRY_SPECIFIERS::DETECTOR_SURFACES | GANTRY_SPECIFIERS::DETECTOR_CORNERS);
	closeAxis( axis );

}