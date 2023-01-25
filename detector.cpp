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


detector::detector( cartCSys* const cSys_, detectorRadonParameter& radonParameter, detectorIndipendentParameter& indipendentParameter ) :
	cSys( cSys_ ),
	physicalParameters{ radonParameter, indipendentParameter },
	radonParameters( radonParameter )
{

	const size_t nTheta = 20;
	const size_t nDistance = FOdd( 11 );

	const double distanceRange = 10;
	const double arcRadius = 20;

	const double deltaTheta = PI / (double) ( nTheta - 1 );
	const double deltaDistance = distanceRange / (double) ( nDistance - 1 );

	const uvec3 middleNormal = -cSys->EyVec();			// Middle normal is the negative y axis
	const uvec3 rotationAxis = cSys->EzVec();				//Rotation axis is z axis

	vector<line> pixelNormals;						// All normals
	pixelNormals.push_back( line{ middleNormal, pnt3{ vec3{ -middleNormal } * arcRadius / 2 } } );			// The middle normal pointing from pixel to origin

	//allPixel = vector<pixel>( nDistance );
	//allPixel.push_back( pixel{  } )

	// Iterate one "half" of normals
	for( size_t currentIndex = 0; currentIndex < ( nDistance - 1 ) / 2; currentIndex++ ){

		// Rotation angle to rotate the middle normal by
		const double rotationAngle = (double) ( currentIndex + 1 ) * deltaTheta;

		// Rotated normal
		const uvec3 currentNormalVec = middleNormal.rotN( rotationAxis, rotationAngle );

		// Lot on normal perpendicualr to rotation axis
		vec3 normalLot = currentNormalVec ^ rotationAxis;
		normalLot.normalize();	// Set length to one

		// Distance from origin to normal
		const double currentDistance = distanceRange / 2 - (double) ( ( nDistance - 1 ) / 2  - currentIndex - 1 ) * deltaDistance;

		// Set lot length to distance
		normalLot.scale( currentDistance );

		// Point on Normal
		const pnt3 normalPoint{ normalLot };

		// Get point on pixel which lies on an arc with radius R
		const primitiveVec3 o = normalPoint.XYZ();
		const primitiveVec3 r = currentNormalVec.XYZ();
		const double R = arcRadius;

		const double p = ( 2*o.y*r.y + r.y*R + r.x ) / ( pow( r.y, 2 ) );
		const double q = ( o.x + o.y*R + pow( o.y, 2 ) - 3/4*pow( R, 2 ) ) / pow( r.y, 2 );

		const double lambda = -p/2 + sqrt( pow( p / 2, 2 ) - q );
		
		// The normal line of current pixel
		const line currentNormal{ currentNormalVec, normalPoint };
		pixelNormals.push_back( currentNormal );

		// Point on Pixel
		//const pnt3 pointOnPixel = currentNormal.getPnt( lambda );

		//const uvec3 surfVec1 = rotationAxis;
		//const uvec3 surfVec2 = surfVec1 ^ currentNormalVec;

		//const surf pixelSurface{ surfVec1, surfVec2, pointOnPixel };

	}

	// Initialise vectors
	/*allPixel = vector<pixel>(physicalParameters.number.col);

	// Calculate angle delta from pixel size along arc and arc radius 
	double angleDelta = radonParameters.resolution.col;// 2 * atan( pxSize.r / ( 4 * radius ) );

	// Iterate all columns
	size_t columns = physicalParameters.number.col;
	v2CR pxSize = physicalParameters.pixelSize;

	for (size_t col = 0; col < columns; col++) {
		double rotAngle = 0;

		// Not the middle pixel
		if (col != (columns - 1) / 2) {
			// Rotation angle
			rotAngle = -angleDelta * ((double)(columns - 1) / 2) + (double)col * angleDelta;
		}
		
		const uvec3 n{ cSys->EyVec().rotZ(rotAngle) };		// Normal vector for row pixel in one column
		const uvec3 r2 = cSys->EzVec();						// z-axis is r2 of surface
		const pnt3 o = n * ( physicalParameters.detectorFocusDistance );					// Origin point of surface
		const uvec3 r1 = n ^ r2;							// First direction vector

		// Pixel with given normal vector centered at o + dZ
		const pixel px{ r1, r2, o,    -pxSize.row / 2, pxSize.row / 2, -pxSize.col / 2, pxSize.col / 2 };

		allPixel.at( col ) = px;

	}*/


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