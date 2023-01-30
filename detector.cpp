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
	const size_t nDistance = radonParameters.numberPoints.row;									// Amount of distances or pixel
	const double distanceRange = (double) ( nDistance - 1) * radonParameters.resolution.row;	// Covered field of measure

	const double deltaTheta = radonParameters.resolution.col;		// Angle resolution
	const double deltaDistance = radonParameters.resolution.row;	// Distance resolution


	// Important vectors
	const uvec3 middleNormalVector = cSys->EyVec();					// y-axis of coordinate system is the middle normal vector
	const uvec3 rotationVector = cSys->EzVec();						// Pixel normals should lie in xy-plane. The middle normal vector will be rotated around this vector


	// All pixel normals
	vector<line> allNormals( nDistance );


	// Iterate through half of pixel normals. Second half is created by symmetry
	// Normals will be created inside to outside
	for( size_t currentIndex = 0; currentIndex <= ( nDistance - 1 ) / 2; currentIndex++ ){
	
		// Angle to rotate the middle normal vector by
		const double rotationAngle = (double) (currentIndex) * deltaTheta;
	
		// Middle normal vector rotation by rotation angle around rotation vector
		const uvec3 currentNormalVector = middleNormalVector.rotN( rotationVector, rotationAngle );



		// Find a point with the distance corresponding to distance in sinogram
		// The point's origin vector must be perpendicular to the current normal vector

		// The lot is perpendicular to the current normal vector and it lies in xy-plane
		const uvec3 normalLot = rotationVector ^ currentNormalVector;

		// Distance from origin to normal. Is the distance in the sinogram
		const double currentDistance = distanceRange / 2 - (double) ( ( nDistance - 1 ) / 2 - currentIndex ) * deltaDistance;

		// Point which lies on the current normal and has the correct distance from the origin 
		const pnt3 normalPoint = vec3{ normalLot } * currentDistance;

		// The current normal 
		const line currentNormal{ currentNormalVector, normalPoint };


		
		// Calculate the point which lies on the pixel based on the normal vector, normal point and the arc radius
		// The point must lie on a circle in xy-plane with the arc radius and a center on y=arcRadius/2

		const v3 o = normalPoint.XYZ();						// Components of normal point
		const v3 r = currentNormalVector.XYZ();				// Components of normal vector
		const double R = indipendentParameter.arcRadius;	// The arc radius
	
		// Solve quadradic equation
		const double p = ( 2. * o.y * r.y + r.y * R + 2. * o.x * r.x ) / ( pow( r.x, 2. ) + pow( r.y, 2. ) );
		const double q = ( pow( o.y, 2. ) + pow( o.x, 2. ) + o.y * R - 3. / 4. * pow( R, 2. ) ) / ( pow( r.x, 2. ) + pow( r.y, 2. ) );

		// The line parameter where the point lies
		const double lambda = -p / 2. + sqrt( pow( p / 2., 2. ) - q );

		// Get the point which lies on the circla and the current normal
		const pnt3 pointOnPixel = currentNormal.getPnt( lambda );

		// Current pixel normal pointing to the center 
		const line pixelNormal{ -currentNormalVector, pointOnPixel };

		

		// Add pixel normal to vector
		// If it is not the middle normal create the symmetrical normal
	
		// Current normal is  the middle normal
		if( currentIndex == 0 ){
			const size_t currentNormalIndex = ( nDistance - 1 ) / 2;
			allNormals.at( currentNormalIndex ) = pixelNormal;
			continue;
		}

		// Add the current normal to vector
		const size_t currentNormalIndex = ( nDistance - 1 ) / 2 - currentIndex;
		allNormals.at( currentNormalIndex ) = pixelNormal;

		// Mirror current normal around y-axis
		const line mirroredPixelNormal{
			pixelNormal.R().negateX(),
			pixelNormal.O().negateX()
		};

		// Add mirrored normal
		const size_t mirroredNormalIndex = ( nDistance - 1 ) / 2 + currentIndex;
		allNormals.at( mirroredNormalIndex ) = mirroredPixelNormal;

	}



	// Create pixel surfaces from pixel normals

	// Iterate all normals from second to penultimate normal
	for( auto currentNormalIt = allNormals.cbegin() + 1; currentNormalIt < allNormals.cend() - 1; currentNormalIt++ ){
		
		auto previousNormalIt = currentNormalIt - 1;		// Previous normal
		auto nextNormalIt = currentNormalIt + 1;			// Next normal
	
		

		// To get the size of current pixel check intersection of neighbooring surface vectors

		// Vector which lies on current surface and in xy-plane
		const uvec3 currentSurfaceVector = currentNormalIt->R() ^ rotationVector;

		// Vector which lies on previous surface and in xy-plane
		const uvec3 previousSurfaceVector = -previousNormalIt->R() ^ rotationVector;

		// Intersection of previous and current surface
		const lineLine_Intersection currentPreviousIntersection{ 
			line{ currentSurfaceVector, currentNormalIt->O() },
			line{ previousSurfaceVector, previousNormalIt->O() }
		};

		// The point of intersection
		const pnt3 currentPreviousIntersectionPoint = currentPreviousIntersection.Result().intersectionPoint;

		// Small surface parameter as negative distance between intersection point and current surface's normal
		const double currentPreviousParameter = -( currentPreviousIntersectionPoint - currentNormalIt->O() ).Length();



		// Vector which lies on next surface and in xy-plane
		const uvec3 nextSurfaceVector = nextNormalIt->R() ^ rotationVector;

		// Intersection of next and current surface
		const lineLine_Intersection currentNextIntersection{
			line{ -currentSurfaceVector, currentNormalIt->O() },
			line{ nextSurfaceVector, nextNormalIt->O() }
		};

		// The point of intersection
		const pnt3 currentNextIntersectionPoint = currentNextIntersection.Result().intersectionPoint;

		// Large surface parameter as distance between intersection point and current surface's normal
		const double currentNextParameter = ( currentNextIntersectionPoint - currentNormalIt->O() ).Length();



		// Create surfaces with calculated parameters
		
		const double colSize = indipendentParameter.columnSize;

		// First surface. Add previous surface
		if( currentNormalIt == allNormals.cbegin() + 1 ){

			allPixel.emplace_back(	previousSurfaceVector,
									rotationVector,
									previousNormalIt->O(),
									currentPreviousParameter,
									( currentPreviousIntersectionPoint - previousNormalIt->O() ).Length(),
									-colSize / 2,
								    colSize / 2 );
		}


		// Add the current pixel
		allPixel.emplace_back(		-currentSurfaceVector,
									rotationVector,
									currentNormalIt->O(),
									currentPreviousParameter,
									currentNextParameter,
									-colSize / 2,
									colSize / 2 );


		// Last surface. Add next surface			
		if( currentNormalIt == allNormals.cend() - 2 ){

			allPixel.emplace_back(	-nextSurfaceVector,
									rotationVector,
									nextNormalIt->O(),
									-( currentNextIntersectionPoint - nextNormalIt->O() ).Length(),
									currentNextParameter,
									-colSize / 2,
									colSize / 2 );
		}

	}
}


vector<pixel> detector::getPixel(void) const {
	return allPixel;
}


void detector::reset( void ){
	for( pixel& currentPixel : allPixel ) currentPixel.reset();
}


void detector::detectRay( const ray r ){

	// Iterate all pixel in detector
	for( pixel& currentPixel : allPixel){

		// Check for intersection of ray with current pixel
		rayPix_Intersection pixelHit{ r, currentPixel };

		// Do they intersect?
		if( pixelHit.Result().hasSolution ){
			currentPixel.detectedRayProperties.push_back( r.Properties() );		// Add detected ray properties to pixel
		}
	
		//TODO: Check anti scattering structuring

	}
}


cartCSys* detector::CSys( void ) const{
	return cSys;
}

detectorRadonParameter detector::getSignalParameter( void ) const{
	return radonParameters;
}


detectorPhysicalParameter detector::getPhysicalParameters( void ) const{
	return physicalParameters;
}