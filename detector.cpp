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

// TODO: add calculation of signal parameters
detector::detector( cartCSys* const cSys_, const double radius_, const detectorParameterPhysical parameter ) :
	cSys( cSys_ ),
	columns( Fpos( parameter.columns ) ),
	pxSize( v2CR{ parameter.colSize, parameter.rowSize } ),
	radius( Fpos( radius_ )),
	structured( parameter.structured )
{
	pxSize.r = Fpos( pxSize.r );
	pxSize.c = Fpos( pxSize.c );
	
	// Initialise vectors
	allPixel = vector<pixel>( columns );


	// Amount of detectors in one row must be odd
	if (isEven(columns)) columns++;

	// Calculate angle delta from pixel size along arc and arc radius 
	double angleDelta = 2 * atan( pxSize.c / ( 4 * radius ) );

	// Calaculate the frames per rotation based on the angle delta. This is the amount of steps necessary for a complete rotation when rotations by angleDelta each step
	size_t framesPerRotation = (size_t) ( 2. * floor( PI / angleDelta ) );

	// Since the frames per rotation is a whole number - angleDelta must be corrected
	angleDelta = 2 * PI / (double) framesPerRotation;

	// So must the pixel size be updated
	pxSize.c = tan( angleDelta / 2. ) * 4. * radius;

	// This is the amount of frames necessary for the first pixel normal to align with the last pixel normal's position in the first frame
	size_t fprSinogramFilled = framesPerRotation / 2 + columns - 1;


	// Iterate all columns
	for (size_t col = 0; col < columns; col++) {
		double rotAngle = 0;

		// Not the middle pixel
		if (col != (columns - 1) / 2) {
			// Rotation angle
			rotAngle = -angleDelta * ((double)(columns - 1) / 2) + (double)col * angleDelta;
		}
		
		const uvec3 n{ cSys->EyVec().rotZ(rotAngle) };		// Normal vector for row pixel in one column
		const uvec3 r2 = cSys->EzVec();						// z-axis is r2 of surface
		const pnt3 o = n * (2 * radius);					// Origin point of surface
		const uvec3 r1 = n ^ r2;							// First direction vector

		// Pixel with given normal vector centered at o + dZ
		const pixel px{ r1, r2, o,  -pxSize.c / 2, pxSize.c / 2, -pxSize.r / 2, pxSize.r / 2 };

		allPixel.at( col ) = px;

	}



}


vector<pixel> detector::getPixel(void) const {
	return allPixel;
}

void detector::reset( void ){

	for( pixel& currentPixel : allPixel ){

		currentPixel.reset();

	}

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


detectorRadonParameter detector::getSignalParameter( const cartCSys* const cSys ) const{
	
	detectorRadonParameter parameter;
	

	radonCoords firstPixel{ cSys, allPixel.front().NormalLine()};
	radonCoords secondPixel{ cSys, ( allPixel.begin() + 1 )->NormalLine()};
	radonCoords lastPixel{ cSys, allPixel.back().NormalLine()};

	// TODO: Acquire signal parameters from pixel collection
	parameter.deltaTheta = abs( secondPixel.theta - firstPixel.theta );
	parameter.deltaDistance = abs( secondPixel.distance - firstPixel.distance );
	
	if( firstPixel.distance < lastPixel.distance ) parameter.distanceRange = range{ firstPixel.distance, lastPixel.distance };
	else parameter.distanceRange = range{ lastPixel.distance, firstPixel.distance };


	size_t framesPerRotation = (size_t) ( 2. * floor( PI / parameter.deltaTheta ) );
	parameter.framesToFillSinogram = framesPerRotation / 2 + columns - 1;


	return parameter;
}