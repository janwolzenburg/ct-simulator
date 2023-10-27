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
#include "detector.h"
#include "radonTransform.h"


/*********************************************************************
  Implementations
*********************************************************************/


detector::detector( CoordinateSystem* const coordinate_system, const detectorRadonParameter radonParameter, const detectorIndipendentParameter indipendentParameter ) :
	cSys( coordinate_system ),
	physicalParameters{ radonParameter, indipendentParameter },
	radonParameters( radonParameter )
{

	// Important parameter
	const size_t nDistance = radonParameters.numberPoints.r;									// Amount of distances or pixel
	const double distanceRange = (double) ( nDistance - 1) * radonParameters.resolution.r;	// Covered field of measure

	const double deltaTheta = radonParameters.resolution.c;		// Angle resolution
	const double deltaDistance = radonParameters.resolution.r;	// Distance resolution

	const double detectorCenterDistance = physicalParameters.detectorFocusDistance / 2.;		// Distance from middle pixel to origin_

	// Important vectors
	const UnitVector3D middleNormalVector = cSys->GetEy();					// y-axis of coordinate system is the middle normal vector
	const UnitVector3D rotationVector = cSys->GetEz();						// Pixel normals should lie in xy-plane. The middle normal vector will be rotated around this vector


	// Persistent variables
	Line previousNormal;			// GetCenterNormal of previous pixel
	double previousPixelSize;		// Size of previous pixel


	// Iterate through half of pixel normals. Second half is created by symmetry
	// Normals will be created inside to outside
	for( size_t currentIndex = 0; currentIndex <= ( nDistance - 1 ) / 2; currentIndex++ ){

		// Angle to rotate the middle normal vector by
		const double rotationAngle = (double) (currentIndex) *deltaTheta;

		// Middle normal vector rotation by rotation GetAngle around rotation vector
		const UnitVector3D currentNormalVector = middleNormalVector.RotateConstant( rotationVector, rotationAngle );


		// Find a point with the distance corresponding to distance in sinogram
		// The point's origin_ vector must be perpendicular to the current normal vector

		// The lot is perpendicular to the current normal vector and it lies in xy-plane
		const UnitVector3D normalLot = rotationVector ^ currentNormalVector;

		// Distance from origin_ to normal. Is the distance in the sinogram
		const double currentDistance = distanceRange / 2 - (double) ( ( nDistance - 1 ) / 2 - currentIndex ) * deltaDistance;

		// Point which lies on the current normal and has the correct distance from the origin_ 
		const Point3D normalPoint = Vector3D{ normalLot } *currentDistance;

		// The current normal 
		const Line currentNormal{ currentNormalVector, normalPoint };

		// Index of normal in vector
		//const size_t currentNormalIndex = ( nDistance - 1 ) / 2 - currentIndex;


		Point3D currentPixelOrigin;		// Origin of current pixel
		double currentPixelSize;		// Size of current pixel


		// "Middle" normal
		if( currentIndex == 0 ){
			// This is the starting point
			currentPixelOrigin = currentNormal.GetPoint( detectorCenterDistance );

			// First pixel size so that the neighbooring pixel intersects at half GetAngle
			currentPixelSize = 2 * tan( deltaTheta / 2. ) * ( detectorCenterDistance + deltaDistance / sin( deltaTheta ) );

		}
		else{
			// Intersection point of pixel
			const Point3D pixelIntersection = previousNormal.origin() + ( previousNormal.direction() ^ rotationVector ) * previousPixelSize / 2.;

			// Lot vector from current normal to intersection point. Vector is pointing to the normal
			const Vector3D pixelIntersectionLot = currentNormal.GetLot( pixelIntersection );

			// Get the pixel normal's origin_ which lies on the shortest Line connection the intersection with current normal
			currentPixelOrigin = pixelIntersection + pixelIntersectionLot;

			// Pixel size is double the lot length_
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
		allPixel.emplace_back( currentSurfaceVector,
							   rotationVector,
							   pixelNormal.origin(),
							   -currentPixelSize / 2,
							   currentPixelSize / 2,
							   -indipendentParameter.columnSize / 2.,
							   indipendentParameter.columnSize / 2. );

		// Add mirrored when not middle pixel
		if( currentIndex > 0 ){

			// Mirror current normal around y-axis
			const Line mirroredPixelNormal{
				pixelNormal.direction().NegateXComponent(),
				pixelNormal.origin().NegateXComponent()
			};

			// Add mirrored pixel
			const UnitVector3D mirroredSurfaceVector = -mirroredPixelNormal.direction() ^ rotationVector;
			allPixel.emplace_back( mirroredSurfaceVector,
								   rotationVector,
								   mirroredPixelNormal.origin(),
								   -currentPixelSize / 2,
								   currentPixelSize / 2,
								   -indipendentParameter.columnSize / 2.,
								   indipendentParameter.columnSize / 2. );
		}

	}

	// After constructing converted poixel are identcal to all
	allPixelConverted = allPixel;
}


void detector::reset( void ){
	for( pixel& currentPixel : allPixel ) currentPixel.reset();
}


void detector::detectRay( const Ray r, mutex& allPixelLock ){


	// Iterate all pixel indices
	for( size_t pixelIdx = 0; pixelIdx < allPixel.size(); pixelIdx++ ){
	
		// Converted pixel
		const pixel currentPixel = allPixelConverted.at( pixelIdx );

	
		// Check for intersection of Ray with current pixel
		const RayPixelIntersection pixelHit{ r, currentPixel };

		// Do they intersect?
		if( pixelHit.intersection_exists_ ){
			
			// If structured and GetAngle allowed by structure
			if( !physicalParameters.structured || ( PI / 2. - r.GetAngle( (Surface) currentPixel ) ) <= physicalParameters.maxRayAngleDetectable ){
				allPixelLock.lock();
				allPixel.at( pixelIdx ).addDetectedProperties( r.properties() );		// Add detected Ray properties_ to pixel
				allPixelLock.unlock();
			}


			// Only one pixel can intersect with Ray
			break;
		}


	}

}


void detector::convertPixel( const CoordinateSystem* const targetCSys ){

	// Iterate all pixel in detector
	for( size_t pixelIdx = 0; pixelIdx < allPixel.size(); pixelIdx++ ){

		allPixelConverted.at( pixelIdx ) = allPixel.at( pixelIdx ).ConvertTo( targetCSys );

	}

}