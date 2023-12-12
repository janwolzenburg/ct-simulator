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


XRayDetector::XRayDetector( CoordinateSystem* const coordinate_system, const ProjectionsProperties radonParameter, const PhysicalDetectorProperties physical_properties ) :
	coordinate_system_( coordinate_system ),
	properties_{ radonParameter, physical_properties }
{

	// Important parameter
	const size_t nDistance = properties_.number_of_pixel.c;									// Amount of distances or pixel
	const double distanceRange = static_cast<double>( nDistance - 1 ) * radonParameter.distances_resolution();	// Covered field of measure

	const double deltaTheta = radonParameter.angles_resolution();		// Angle resolution
	const double deltaDistance = radonParameter.distances_resolution();	// Distance resolution

	const double detector_focus_distance = properties_.detector_focus_distance / 2.;		// Distance from middle pixel to origin_

	// Important vectors
	const UnitVector3D middleNormalVector = coordinate_system_->GetEy();					// y-axis of coordinate system is the middle normal vector
	const UnitVector3D rotationVector = coordinate_system_->GetEz();						// Pixel normals should lie in xy-plane. The middle normal vector will be rotated around this vector


	// Persistent variables
	Line previousNormal;			// GetCenterNormal of previous pixel
	double previousPixelSize;		// Size of previous pixel


	// Iterate through half of pixel normals. Second half is created by symmetry
	// Normals will be created inside to outside
	for( size_t currentIndex = 0; currentIndex < nDistance / 2; currentIndex++ ){

		// Angle to rotate the middle normal vector by
		const double rotationAngle = ( static_cast<double>( currentIndex ) + 0.5 ) * deltaTheta;

		// Middle normal vector rotation by rotation angle around rotation vector
		const UnitVector3D currentNormalVector = middleNormalVector.RotateConstant( rotationVector, rotationAngle );


		// Find a point with the distance corresponding to distance in sinogram
		// The point's origin vector must be perpendicular to the current normal vector

		// The lot is perpendicular to the current normal vector and it lies in xy-plane
		const UnitVector3D normalLot = rotationVector ^ currentNormalVector;

		// Distance from origin to normal. Is the distance in the sinogram
		const double currentDistance = distanceRange / 2. - ( static_cast<double>( nDistance ) / 2. - static_cast<double>( currentIndex ) - 1 ) * deltaDistance;
															
		// Point which lies on the current normal and has the correct distance from the origin_ 
		const Point3D normalPoint = Vector3D{ normalLot } * currentDistance;

		// The current normal 
		const Line currentNormal{ currentNormalVector, normalPoint };

		Point3D currentPixelOrigin;		// Origin of current pixel
		double currentPixelSize;		// Size of current pixel


		// "Middle" normal
		if( currentIndex == 0 ){
			// This is the starting point
			currentPixelOrigin = currentNormal.GetPoint( detector_focus_distance );

			// First pixel size so that the neighbooring pixel intersects at half angle
			currentPixelSize = 2 * tan( deltaTheta / 2. ) * ( detector_focus_distance + currentDistance / tan( deltaTheta / 2. ) );

		}
		else{
			// Intersection point of pixel
			const Point3D pixelIntersection = previousNormal.origin() + ( previousNormal.direction() ^ rotationVector ) * previousPixelSize / 2.;

			// Lot vector from current normal to intersection point. Vector is pointing to the normal
			const Vector3D pixelIntersectionLot = currentNormal.GetLot( pixelIntersection );

			// Get the pixel normal's origin_ which lies on the shortest Line connection the intersection with current normal
			currentPixelOrigin = pixelIntersection + pixelIntersectionLot;

			// Pixel size is double the lot length
			currentPixelSize = 2 * pixelIntersectionLot.length();
		}

		// Create current pixel normal pointing to center
		const Line pixelNormal{ -currentNormalVector, currentPixelOrigin };

		// Store for next pixel
		previousNormal = pixelNormal;
		previousPixelSize = currentPixelSize;

		// Vector perpendicualr to the normal pointing to the next pixel
		const UnitVector3D currentSurfaceVector = -pixelNormal.direction() ^ rotationVector;

		// Add pixel
		pixel_array_.emplace_back(  BoundedSurface{ 
								currentSurfaceVector,
								rotationVector,
								pixelNormal.origin(),
								-currentPixelSize / 2,
								currentPixelSize / 2,
								-properties_.row_width / 2.,
								properties_.row_width / 2. } );

		// Mirror current normal around y-axis
		const Line mirroredPixelNormal{
			pixelNormal.direction().NegateXComponent(),
			pixelNormal.origin().NegateXComponent()
		};

		// Add mirrored pixel
		const UnitVector3D mirroredSurfaceVector = -mirroredPixelNormal.direction() ^ rotationVector;
		pixel_array_.emplace_back(	BoundedSurface{ 
								mirroredSurfaceVector,
								rotationVector,
								mirroredPixelNormal.origin(),
								-currentPixelSize / 2,
								currentPixelSize / 2,
								-properties_.row_width / 2.,
								properties_.row_width / 2. } );

		RadonCoordinates radon_coords { this->coordinate_system_, pixelNormal };

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