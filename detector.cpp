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
	radonParameters( radonParameter ),
	physicalParameters{ radonParameter, indipendentParameter }
{

	// Check parameters


	// Initialise vectors
	allPixel = vector<pixel>( physicalParameters.number.col );

	// Calculate angle delta from pixel size along arc and arc radius 
	double angleDelta = radonParameter.resolution.col;// 2 * atan( pxSize.r / ( 4 * radius ) );

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


detectorRadonParameter detector::getSignalParameter( void ) const{
	return radonParameters;
}


detectorPhysicalParameter detector::getPhysicalParameters( void ) const{
	return physicalParameters;
}