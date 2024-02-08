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
							const ProjectionsProperties radonParameter, 
							const PhysicalDetectorProperties physical_properties ) :
	coordinate_system_( coordinate_system ),
	properties_{ radonParameter, physical_properties }
{

	// Important parameter
	const size_t number_of_distances = properties_.number_of_pixel.c;				// Amount of distances or pixel
	const double distance_range = static_cast<double>( number_of_distances - 1 ) * 
								  radonParameter.distances_resolution();	// Covered field of measure

	const double delta_theta = radonParameter.angles_resolution();		// Angle resolution
	const double delta_distance = radonParameter.distances_resolution();	// Distance resolution

	// Distance from middle pixel to origin
	const double detector_focus_distance = properties_.detector_focus_distance / 2.;		

	// Important vectors
	const UnitVector3D middle_normal_vector = coordinate_system_->GetEy();	// y-axis of coordinate system is the middle normal vector
	const UnitVector3D rotation_vector = coordinate_system_->GetEz();		// Pixel normals should lie in xy-plane. The middle normal vector will be rotated around this vector


	// Persistent variables
	Line previous_pixel_normal;			// Get normal of previous pixel
	double previous_pixel_size;		// Size of previous pixel


	// Iterate through half of pixel normals. Second half is created by symmetry
	// Normals will be created inside to outside
	for( size_t pixel_index = 0; pixel_index < number_of_distances / 2; pixel_index++ ){

		// Angle to rotate the middle normal vector by
		const double rotation_angle = ( static_cast<double>( pixel_index ) + 0.5 ) * delta_theta;

		// Middle normal vector rotation by rotation angle around rotation vector
		const UnitVector3D normal_vector = middle_normal_vector.RotateConstant( rotation_vector, rotation_angle );


		// Find a point with the distance corresponding to distance in sinogram
		// The point's origin vector must be perpendicular to the current normal vector

		// The lot is perpendicular to the current normal vector and it lies in xy-plane
		const UnitVector3D normal_lot = rotation_vector ^ normal_vector;

		// Distance from origin to normal. Is the distance in the sinogram
		const double current_distance = distance_range / 2. - ( static_cast<double>( number_of_distances ) / 2. - static_cast<double>( pixel_index ) - 1 ) * delta_distance;
															
		// Point which lies on the current normal and has the correct distance from the origin
		const Point3D lot_end_point = Vector3D{ normal_lot } * current_distance;

		// The current normal 
		const Line normal{ normal_vector, lot_end_point };

		Point3D pixel_origin;		// Origin of current pixel
		double pixel_size;		// Size of current pixel


		// "Middle" normal
		if( pixel_index == 0 ){
			// This is the starting point
			pixel_origin = normal.GetPoint( detector_focus_distance );

			// First pixel size so that the neighbooring pixel intersects at half angle
			pixel_size = 2 * tan( delta_theta / 2. ) * ( detector_focus_distance + current_distance / tan( delta_theta / 2. ) );

		}
		else{
			// Intersection point of pixel
			const Point3D intersection = previous_pixel_normal.origin() + ( previous_pixel_normal.direction() ^ rotation_vector ) * previous_pixel_size / 2.;

			// Lot vector from current normal to intersection point. Vector is pointing to the normal
			const Vector3D intersection_lot = normal.GetLot( intersection );

			// Get the pixel normal's origin_ which lies on the shortest Line connection the intersection with current normal
			pixel_origin = intersection + intersection_lot;

			// Pixel size is double the lot length
			pixel_size = 2 * intersection_lot.length();
		}

		// Create current pixel normal pointing to center
		const Line pixel_normal{ -normal_vector, pixel_origin };

		// Store for next pixel
		previous_pixel_normal = pixel_normal;
		previous_pixel_size = pixel_size;

		// Vector perpendicualr to the normal pointing to the next pixel
		const UnitVector3D surface_vector = -pixel_normal.direction() ^ rotation_vector;

		// Add pixel
		pixel_array_.emplace_back(  BoundedSurface{ 
								surface_vector,
								rotation_vector,
								pixel_normal.origin(),
								-pixel_size / 2,
								pixel_size / 2,
								-properties_.row_width / 2.,
								properties_.row_width / 2. } );

		// Mirror current normal around y-axis
		const Line mirroredPixelNormal{
			pixel_normal.direction().NegateXComponent(),
			pixel_normal.origin().NegateXComponent()
		};

		// Add mirrored pixel
		const UnitVector3D mirrored_surface_vector = -mirroredPixelNormal.direction() ^ rotation_vector;
		pixel_array_.emplace_back(	BoundedSurface{ 
								mirrored_surface_vector,
								rotation_vector,
								mirroredPixelNormal.origin(),
								-pixel_size / 2,
								pixel_size / 2,
								-properties_.row_width / 2.,
								properties_.row_width / 2. } );

		RadonCoordinates radon_coords { this->coordinate_system_, pixel_normal };

	}

	// After constructing converted poixel are identical
	converted_pixel_array_ = pixel_array_;
}


void XRayDetector::ResetDetectedRayPorperties( void ){
	for( DetectorPixel& currentPixel : pixel_array_ ) currentPixel.ResetDetectedRayProperties();
}

void XRayDetector::UpdateProperties( const ProjectionsProperties radon_properties, const PhysicalDetectorProperties physical_properties ){

	*this = XRayDetector{ coordinate_system_, radon_properties, physical_properties };

}

bool XRayDetector::DetectRay( Ray& r, mutex& allPixelLock ){

	const size_t expected_pixel_index = r.properties().expected_detector_pixel_index();

	size_t pixelIdx = expected_pixel_index;

	// Iterate all pixel indices. But start with the most likely pixel
	for( size_t counter = 0; counter < pixel_array_.size() + 1; counter++ ){
	
		// Not first iteration: index is counter - 1 becaus first iteration was the most likely pixel
		if( counter != 0 ){
			pixelIdx = counter - 1;

			// Skip detection if expected index would be tested again
			if( pixelIdx == expected_pixel_index )
				continue;
		}

		// Converted pixel
		const DetectorPixel& currentPixel = converted_pixel_array_.at( pixelIdx );

		// Check for intersection of Ray with current pixel
		const RayPixelIntersection pixelHit{ r, currentPixel };

		// Do they intersect?
		if( pixelHit.intersection_exists_ ){
			
			// If has_anti_scattering_structure and angle allowed by structure
			
			if( !properties_.has_anti_scattering_structure || ( PI / 2. - r.GetAngle( currentPixel ) ) <= properties_.max_ray_angle_allowed_by_structure ){
				allPixelLock.lock();
				pixel_array_.at( pixelIdx ).AddDetectedRayProperties( r.properties() );		// Add detected Ray properties to pixel
				allPixelLock.unlock();
				
				#ifdef TRANSMISSION_TRACKING 
				if( !r.ray_tracing().tracing_steps.empty() ){
					r.ray_tracing().tracing_steps.back().exit = pixelHit.intersection_point_;
					r.ray_tracing().tracing_steps.back().distance = (pixelHit.intersection_point_ - r.ray_tracing().tracing_steps.back().entrance).length();
				}
				#endif

			}

			// Only one pixel can intersect with Ray
			return true;
		}

	}

	return false;
}


void XRayDetector::ConvertPixelArray( const CoordinateSystem* const targetCSys ){

	// Iterate all pixel in detector
	for( size_t pixelIdx = 0; pixelIdx < pixel_array_.size(); pixelIdx++ ){
		converted_pixel_array_.at( pixelIdx ) = std::move( pixel_array_.at( pixelIdx ).ConvertTo( targetCSys ) );
	}
}