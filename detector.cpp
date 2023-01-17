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



detector::detector( cartCSys* const cSys_, const detectorRadonParameter parameter, size_t numRows_, const double angle_, const double columnSize, const bool structered ) :
	cSys( cSys_ ),
	radonParameters( parameter ),
	physicalParameters( parameter, numRows_, angle_, columnSize, structered )
{
	// Initialise vectors
	allPixel = vector<pixel>( physicalParameters.numberColumns );


	// Amount of detectors in one row must be odd
	//if (isEven(columns)) columns++;

	// Calculate angle delta from pixel size along arc and arc radius 
	double angleDelta =  parameter.resolution.c;// 2 * atan( pxSize.r / ( 4 * radius ) );

	// Calaculate the frames per rotation based on the angle delta. This is the amount of steps necessary for a complete rotation when rotations by angleDelta each step
	//size_t framesPerRotation = (size_t) ( 2. * floor( PI / angleDelta ) );

	// Since the frames per rotation is a whole number - angleDelta must be corrected
	//angleDelta = 2 * PI / (double) framesPerRotation;

	// So must the pixel size be updated
	//pxSize.r = tan( angleDelta / 2. ) * 4. * radius;

	// This is the amount of frames necessary for the first pixel normal to align with the last pixel normal's position in the first frame
	//size_t fprSinogramFilled = framesPerRotation / 2 + columns - 1;


	// Iterate all columns
	size_t columns = physicalParameters.numberColumns;
	v2CR pxSize{ physicalParameters.colSize, physicalParameters.rowSize };

	for (size_t col = 0; col < columns; col++) {
		double rotAngle = 0;

		// Not the middle pixel
		if (col != (columns - 1) / 2) {
			// Rotation angle
			rotAngle = -angleDelta * ((double)(columns - 1) / 2) + (double)col * angleDelta;
		}
		
		const uvec3 n{ cSys->EyVec().rotZ(rotAngle) };		// Normal vector for row pixel in one column
		const uvec3 r2 = cSys->EzVec();						// z-axis is r2 of surface
		const pnt3 o = n * ( 2 * physicalParameters.radius );					// Origin point of surface
		const uvec3 r1 = n ^ r2;							// First direction vector

		// Pixel with given normal vector centered at o + dZ
		const pixel px{ r1, r2, o,    -pxSize.r / 2, pxSize.r / 2, -pxSize.c / 2, pxSize.c / 2 };

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
	
	/*
	// Parameters of detector in sinogram
	detectorRadonParameter parameter;

	radonCoords firstPixel{ cSys, allPixel.front().NormalLine()};				// First pixel
	radonCoords secondPixel{ cSys, ( allPixel.begin() + 1 )->NormalLine()};		// Seconds pixel
	radonCoords lastPixel{ cSys, allPixel.back().NormalLine()};					// Last pixel

	parameter.deltaTheta = abs( secondPixel.theta - firstPixel.theta );			// Angle resolution
	parameter.deltaDistance = abs( secondPixel.distance - firstPixel.distance );// Distance resolution
	
	// Get distance range
	if( firstPixel.distance > lastPixel.distance ) parameter.distanceRange = range{ -abs( lastPixel.distance ), abs( lastPixel.distance ) };
	else parameter.distanceRange = range{ -abs( firstPixel.distance ), abs( firstPixel.distance ) };

	// Recalculate the distance range for an odd number of distances in sinogram
	size_t numDistances = (size_t) ( ( parameter.distanceRange.end - parameter.distanceRange.start ) / parameter.deltaDistance ) + 1;
	if ( isEven( numDistances ) ) numDistances--;
	parameter.deltaDistance = ( parameter.distanceRange.end - parameter.distanceRange.start ) / (double) ( numDistances - 1 );

	// Calculate the amount of frames necessary to fill sinogram
	size_t framesPerRotation = (size_t) ( 2. * floor( PI / parameter.deltaTheta ) );
	parameter.framesToFillSinogram = framesPerRotation / 2 + columns - 1;
	*/

	return radonParameters;// parameter;
}


detectorParameterPhysical detector::getPhysicalParameters( void ) const{
	return physicalParameters;
}