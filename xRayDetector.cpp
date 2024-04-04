/*********************************************************************
 * @file   detector.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/



 /*********************************************************************
   Includes
*********************************************************************/

#include "generel.h"
#include "vector3D.h"
#include "xRayDetector.h"
#include "projections.h"


/*********************************************************************
  Implementations
*********************************************************************/


XRayDetector::XRayDetector( CoordinateSystem* const coordinate_system, 
								const ProjectionsProperties projection_properties, 
								const PhysicalDetectorProperties physical_properties ) :
	coordinate_system_( coordinate_system ),
	properties_{ projection_properties, physical_properties }
{
	// amount of distances or pixel
	const size_t number_of_distances = properties_.number_of_pixel.c;		
	// measure field
	const double distance_range = static_cast<double>( number_of_distances - 1 ) * 
								  projection_properties.distances_resolution();	
	// angle resolution
	const double delta_theta = projection_properties.angles_resolution();			
	// distance resolution
	const double delta_distance = projection_properties.distances_resolution();

	// distance from middle pixel to origin
	const double detector_focus_distance = properties_.detector_focus_distance / 2.;		

	// y-axis of coordinate system is the middle normal vector
	const Unitvector3D middle_normal_vector = coordinate_system_->GetEy();
	// pixel normals should lie in xy-plane
	// the middle normal vector will be rotated around this vector
	const Unitvector3D rotation_vector = coordinate_system_->GetEz();		


	// persistent variables
	Line previous_pixel_normal;		// get normal of previous pixel
	double previous_pixel_size;		// size of previous pixel


	// iterate through half of pixel normals. Second half is created by symmetry
	// normals will be created inside to outside
	for( size_t pixel_index = 0; pixel_index < number_of_distances / 2; pixel_index++ ){

		// angle to rotate the middle normal vector by
		const double rotation_angle = ( static_cast<double>( pixel_index ) + 0.5 ) 
																	* delta_theta;

		// middle normal vector rotation by rotation angle around rotation vector
		const Unitvector3D normal_vector = 
			middle_normal_vector.RotateConstant( rotation_vector, rotation_angle );


		// find a point with the distance corresponding to distance in sinogram
		// the point's origin vector must be perpendicular to the current normal vector

		// the lot is perpendicular to the current normal vector and it lies in xy-plane
		const Unitvector3D normal_lot = rotation_vector ^ normal_vector;

		// distance from origin to normal. Is the distance in the sinogram
		const double current_distance = distance_range / 2. - 
			( static_cast<double>( number_of_distances ) / 2. 
			  - static_cast<double>( pixel_index ) - 1 ) * delta_distance;
															
		// point which lies on the current normal and has the correct distance from the origin
		const Point3D lot_end_point = vector3D{ normal_lot } * current_distance;

		// the current normal 
		const Line normal{ normal_vector, lot_end_point };

		Point3D pixel_origin;	// origin of current pixel
		double pixel_size;		// size of current pixel

		// "middle" normal
		if( pixel_index == 0 ){
			// this is the starting point
			pixel_origin = normal.GetPoint( detector_focus_distance );

			// first pixel size so that the neighbooring pixel intersects at half angle
			pixel_size = 2 * tan( delta_theta / 2. ) * 
				( detector_focus_distance + current_distance / tan( delta_theta / 2. ) );

		}
		else{
			// intersection point of pixel
			const Point3D intersection = previous_pixel_normal.origin() + 
				( previous_pixel_normal.direction() ^ rotation_vector ) * previous_pixel_size / 2.;

			// lot vector from current normal to intersection point
			// vector is pointing to the normal
			const vector3D intersection_lot = normal.GetLot( intersection );

			// get the pixel normal's origin which lies on the shortest Line connection the 
			// intersection with current normal
			pixel_origin = intersection + intersection_lot;

			// pixel size is double the lot length
			pixel_size = 2 * intersection_lot.length();
		}

		// create current pixel normal pointing to center
		const Line pixel_normal{ -normal_vector, pixel_origin };

		// store for next pixel
		previous_pixel_normal = pixel_normal;
		previous_pixel_size = pixel_size;

		// vector perpendicualr to the normal pointing to the next pixel
		const Unitvector3D surface_vector = -pixel_normal.direction() ^ rotation_vector;

		// add pixel
		pixel_array_.emplace_back(  BoundedSurface{ 
								surface_vector, rotation_vector,
								pixel_normal.origin(),
								-pixel_size / 2, pixel_size / 2,
								-properties_.row_width / 2.,
								properties_.row_width / 2. } );

		// mirror current normal around y-axis
		const Line mirroredPixelNormal{
			pixel_normal.direction().NegateXComponent(),
			pixel_normal.origin().NegateXComponent()
		};

		// add mirrored pixel
		const Unitvector3D mirrored_surface_vector = - mirroredPixelNormal.direction() 
																								 ^ rotation_vector;
		pixel_array_.emplace_back(	BoundedSurface{ 
								mirrored_surface_vector, rotation_vector,
								mirroredPixelNormal.origin(),
								-pixel_size / 2, pixel_size / 2,
								-properties_.row_width / 2.,
								properties_.row_width / 2. } );

	}

	// after constructing converted poixel are identical
	converted_pixel_array_ = pixel_array_;
}


void XRayDetector::ResetDetectedRayPorperties( void ){
	for( DetectorPixel& currentPixel : pixel_array_ ) currentPixel.ResetDetectedRayProperties();
}

void XRayDetector::UpdateProperties( const ProjectionsProperties radon_properties, const PhysicalDetectorProperties physical_properties ){

	*this = XRayDetector{ coordinate_system_, radon_properties, physical_properties };

}

optional<size_t> XRayDetector::GetHitPixelIndex( Ray& ray ) const{

	const size_t expected_pixel_index = ray.properties().expected_detector_pixel_index();

	if( ray.properties().definitely_hits_expected_pixel() ){
		return expected_pixel_index;
	}

	size_t pixel_index = expected_pixel_index;

	// iterate all pixel indices. but start with the most likely pixel
	for( size_t counter = 0; counter < pixel_array_.size() + 1; counter++ ){
	
		// not first iteration: index is counter - 1 because first iteration was the 
		// most likely pixel
		if( counter != 0 ){
			pixel_index = counter - 1;

			// skip detection if expected index would be tested again
			if( pixel_index == expected_pixel_index )
				continue;
		}

		// converted pixel
		const DetectorPixel& current_pixel = converted_pixel_array_.at( pixel_index );

		// check if ray hit anti scattering structure hit is possible
		// if detector has anti scattering structure and angle not allowed by structure
		// goto next pixel
		if (properties_.has_anti_scattering_structure &&
			(PI / 2. - ray.GetAngle(current_pixel)) >
			properties_.max_angle_allowed_by_structure) {
			continue;
		}

		// check for intersection of ray with current pixel
		const RayPixelIntersection pixel_hit{ ray, current_pixel };

		// do they intersect?
		if( pixel_hit.intersection_exists_ ){
			ray.SetExpectedPixelIndex( pixel_index, true );
			return pixel_index;
		}
	}

	return {};

}

#ifdef TRANSMISSION_TRACKING // only enabled for verification
bool XRayDetector::DetectRay( Ray& ray, mutex& pixel_array_mutex ){
#else
bool XRayDetector::DetectRay( Ray& ray, mutex& pixel_array_mutex ){
#endif

	optional<size_t> pixel_index = GetHitPixelIndex( ray );

	if( pixel_index.has_value() ){
		pixel_array_mutex.lock();
		pixel_array_.at( pixel_index.value() ).AddDetectedRayProperties( ray.properties() );		
		pixel_array_mutex.unlock();
		
		#ifdef TRANSMISSION_TRACKING // only enabled for verification
		Point3D intersection_point = 
			RayPixelIntersection{ ray, 
														pixel_array_.at( pixel_index.value() ) }.intersection_point_;
		if( !ray.ray_tracing().tracing_steps.empty() ){
			ray.ray_tracing().tracing_steps.back().exit = intersection_point;
			ray.ray_tracing().tracing_steps.back().distance = (intersection_point
			- ray.ray_tracing().tracing_steps.back().entrance).length();
		}
		#endif
	}

	return pixel_index.has_value();
}


bool XRayDetector::TryDetection( Ray& ray ) const{
	optional<size_t> pixel_index = GetHitPixelIndex( ray );
	
	if( pixel_index.has_value() ){
		ray.SetExpectedPixelIndex( pixel_index.value(), true );
	}

	return pixel_index.has_value();

}

void XRayDetector::ConvertPixelArray( const CoordinateSystem* const targetCSys ){

	// iterate all pixel in detector
	for( size_t pixelIdx = 0; pixelIdx < pixel_array_.size(); pixelIdx++ ){
		converted_pixel_array_.at( pixelIdx ) = std::move( pixel_array_.at( pixelIdx ).ConvertTo( targetCSys ) );
	}
}