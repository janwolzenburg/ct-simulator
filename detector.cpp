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
#include "vec3D.h"
#include "detector.h"
#include "radonTransform.h"


/*********************************************************************
  Implementations
*********************************************************************/


detector::detector( cartCSys* const cSys_, const detectorRadonParameter radonParameter, const detectorIndipendentParameter indipendentParameter ) :
	cSys( cSys_ ),
	physicalParameters{ radonParameter, indipendentParameter },
	radonParameters( radonParameter )
{

	// Important parameter
	const size_t nDistance = radonParameters.numberPoints.r;									// Amount of distances or pixel
	const double distanceRange = (double) ( nDistance - 1) * radonParameters.resolution.r;	// Covered field of measure

	const double deltaTheta = radonParameters.resolution.c;		// Angle resolution
	const double deltaDistance = radonParameters.resolution.r;	// Distance resolution

	const double detectorCenterDistance = physicalParameters.detectorFocusDistance / 2.;		// Distance from middle pixel to origin

	// Important vectors
	const uvec3 middleNormalVector = cSys->EyVec();					// y-axis of coordinate system is the middle normal vector
	const uvec3 rotationVector = cSys->EzVec();						// Pixel normals should lie in xy-plane. The middle normal vector will be rotated around this vector


	// Persistent variables
	line previousNormal;			// Normal of previous pixel
	double previousPixelSize;		// Size of previous pixel


	// Iterate through half of pixel normals. Second half is created by symmetry
	// Normals will be created inside to outside
	for( size_t currentIndex = 0; currentIndex <= ( nDistance - 1 ) / 2; currentIndex++ ){

		// Angle to rotate the middle normal vector by
		const double rotationAngle = (double) (currentIndex) *deltaTheta;

		// Middle normal vector rotation by rotation angle around rotation vector
		const uvec3 currentNormalVector = middleNormalVector.rotN( rotationVector, rotationAngle );


		// Find a point with the distance corresponding to distance in sinogram
		// The point's origin vector must be perpendicular to the current normal vector

		// The lot is perpendicular to the current normal vector and it lies in xy-plane
		const uvec3 normalLot = rotationVector ^ currentNormalVector;

		// Distance from origin to normal. Is the distance in the sinogram
		const double currentDistance = distanceRange / 2 - (double) ( ( nDistance - 1 ) / 2 - currentIndex ) * deltaDistance;

		// Point which lies on the current normal and has the correct distance from the origin 
		const pnt3 normalPoint = vec3{ normalLot } *currentDistance;

		// The current normal 
		const line currentNormal{ currentNormalVector, normalPoint };

		// Index of normal in vector
		//const size_t currentNormalIndex = ( nDistance - 1 ) / 2 - currentIndex;


		pnt3 currentPixelOrigin;		// Origin of current pixel
		double currentPixelSize;		// Size of current pixel


		// "Middle" normal
		if( currentIndex == 0 ){
			// This is the starting point
			currentPixelOrigin = currentNormal.getPnt( detectorCenterDistance );

			// First pixel size so that the neighbooring pixel intersects at half angle
			currentPixelSize = 2 * tan( deltaTheta / 2. ) * ( detectorCenterDistance + deltaDistance / sin( deltaTheta ) );

		}
		else{
			// Intersection point of pixel
			const pnt3 pixelIntersection = previousNormal.O() + ( previousNormal.R() ^ rotationVector ) * previousPixelSize / 2.;

			// Lot vector from current normal to intersection point. Vector is pointing to the normal
			const vec3 pixelIntersectionLot = currentNormal.getLot( pixelIntersection );

			// Get the pixel normal's origin which lies on the shortest line connection the intersection with current normal
			currentPixelOrigin = pixelIntersection + pixelIntersectionLot;

			// Pixel size is double the lot length
			currentPixelSize = 2 * pixelIntersectionLot.Length();
		}

		// Create current pixel normal pointing to center
		const line pixelNormal{ -currentNormalVector, currentPixelOrigin };

		// Store for next pixel
		previousNormal = pixelNormal;
		previousPixelSize = currentPixelSize;

		// Vector perpendicualr to the normal pointing to the next pixel
		const uvec3 currentSurfaceVector = -pixelNormal.R() ^ rotationVector;

		// Add pixel
		allPixel.emplace_back( currentSurfaceVector,
							   rotationVector,
							   pixelNormal.O(),
							   -currentPixelSize / 2,
							   currentPixelSize / 2,
							   -indipendentParameter.columnSize / 2.,
							   indipendentParameter.columnSize / 2. );

		// Add mirrored when not middle pixel
		if( currentIndex > 0 ){

			// Mirror current normal around y-axis
			const line mirroredPixelNormal{
				pixelNormal.R().negateX(),
				pixelNormal.O().negateX()
			};

			// Add mirrored pixel
			const uvec3 mirroredSurfaceVector = -mirroredPixelNormal.R() ^ rotationVector;
			allPixel.emplace_back( mirroredSurfaceVector,
								   rotationVector,
								   mirroredPixelNormal.O(),
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


void detector::detectRay( const ray r, mutex& allPixelLock ){


	// Iterate all pixel indices
	for( size_t pixelIdx = 0; pixelIdx < allPixel.size(); pixelIdx++ ){
	
		// Converted pixel
		const pixel currentPixel = allPixelConverted.at( pixelIdx );

	
		// Check for intersection of ray with current pixel
		const rayPix_Intersection pixelHit{ r, currentPixel };

		// Do they intersect?
		if( pixelHit.Result().hasSolution ){
			
			// If structured and angle allowed by structure
			if( !physicalParameters.structured || ( PI / 2. - r.getAngle( (surf) currentPixel ) ) <= physicalParameters.maxRayAngleDetectable ){
				allPixelLock.lock();
				allPixel.at( pixelIdx ).addDetectedProperties( r.Properties() );		// Add detected ray properties to pixel
				allPixelLock.unlock();
			}


			// Only one pixel can intersect with ray
			break;
		}


	}

}


void detector::convertPixel( const cartCSys* const targetCSys ){

	// Iterate all pixel in detector
	for( size_t pixelIdx = 0; pixelIdx < allPixel.size(); pixelIdx++ ){

		allPixelConverted.at( pixelIdx ) = allPixel.at( pixelIdx ).convertTo( targetCSys );

	}

}