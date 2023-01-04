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



/*********************************************************************
  Implementations
*********************************************************************/

detector::detector( cartCSys* const cSys_, const size_t rows_, const size_t columns_, const t2 pxSize_, const double radius_, const bool structured_ ) : 
	cSys( cSys_ ),
	rows( Fpos( rows_ ) ),
	columns( Fpos( columns_ ) ),
	pxSize( pxSize_ ),
	radius( Fpos( radius_ )),
	structured( structured_ )
{
	pxSize.r = Fpos( pxSize.r );
	pxSize.c = Fpos( pxSize.c );
	
	// Initialise 2D vector
	allPixel = vector<vector<pixel>>( rows, vector<pixel>( columns ) );

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

		// Normal vector for row pixel in one column
		uvec3 n{ cSys->EyVec().rotZ(rotAngle) };

		// Pixel in one column and all rows
		vector<pixel> rowPixel;


		const uvec3 r2 = cSys->EzVec();						// z-axis is r2 of surface
		const pnt3 o = n * (2 * radius);					// Origin point of surface
		const uvec3 r1 = n ^ r2;							// First direction vector

		vector<surfLim> rowPixel;
		for (size_t row = 0; row < rows; row++) {
			const pnt3 oTrans = o.addZ((double)row * pxSize.r);			// Translate pixel in z-direction

			// Pixel with given normal vector centered at o + dZ
			const pixel px{ r1, r2, oTrans,  -pxSize.c / 2, pxSize.c / 2, -pxSize.r / 2, pxSize.r / 2 };

			allPixel.at( row ).at( col ) = px;

		}

	}


};