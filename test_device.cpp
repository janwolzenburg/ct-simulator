/*********************************************************************
 * @file   tests_device.cpp
 * @brief  Implementation
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include <iostream>
using std::cerr; using std::cout; using std::endl;

#include <vector>
using std::vector;

#include "generelMath.h"
#include "cSysTree.h"
#include "test_device.h"
#include "tube.h"
#include "detector.h"
#include "plotting.h"
#include "gantry.h"


bool test_tube(void) {

	tubeParameter tubeParas{	.anodeVoltage_V = 53000,
								.anodeCurrent_A = 0.2,
								.anodeAtomicNumber = 74 };

	tube testTube{ GLOBAL_CSYS()->createCopy( "Tube system" ), tubeParas };

	vector<ray> beam = testTube.getBeam( PI/4, 20 );

	ofstream ax1 = openAxis( path( "./test_tube.txt" ), true );

	addObject( ax1, "Beam", beam, "r", 1. );

	closeAxis( ax1 );

	return true;
}

detector getTestDetector( void ){
	// 64 x 32 points in radon space
	// 500mm measure field
	detectorRadonParameter radonParameter{
		idx2CR{ 63, 31 },
		500
	};

	detectorIndipendentParameter indipendentParameter{
		2 * PI * ( 50. / 360. ),
		1, 
		false
	};

	// 50 degree angle
	detector testDetector{ GLOBAL_CSYS()->createCopy( "Detector system" ), radonParameter, indipendentParameter };

	return testDetector;
}

bool test_nonUniformDetector( void ){

	ofstream ax1 = openAxis( path( "./test_nonUniformDetector.txt" ), true );

	cartCSys* cSys = GLOBAL_CSYS()->createCopy( "Detector system" );

	const size_t nTheta = 100;
	const size_t nDistance = FOdd( 33 );

	const double distanceRange = 500;
	const double arcRadius = 1000;

	const double deltaTheta = PI / (double) ( nTheta - 1 );
	const double deltaDistance = distanceRange / (double) ( nDistance - 1 );

	const uvec3 middleNormalVec = cSys->EyVec();			// Middle normal is the negative y axis
	const uvec3 rotationAxis = cSys->EzVec();			//Rotation axis is z axis

	const line middleNormal{ middleNormalVec, cSys->OPnt() };


	vector<line> pixelNormals( nDistance );							// All normals
	//pixelNormals.push_back( line{ middleNormal, pnt3{ vec3{ middleNormal } * arcRadius / 2 } } );			// The middle normal pointing from pixel to origin

	//allPixel = vector<pixel>( nDistance );
	//allPixel.push_back( pixel{  } )

	// Iterate one "half" of normals
	for( size_t currentIndex = 0; currentIndex <= ( nDistance - 1 ) / 2; currentIndex++ ){

		// Rotation angle to rotate the middle normal by
		const double rotationAngle = (double) ( currentIndex ) * deltaTheta;

		// Rotated normal
		const uvec3 currentNormalVec = middleNormalVec.rotN( rotationAxis, rotationAngle );

		//addSingleObject( ax1, "RotatedNormalVec", currentNormalVec, "g", cSys->OPnt() );

		// Lot on normal perpendicualr to rotation axis
		vec3 normalLot =  rotationAxis ^ currentNormalVec;
		normalLot.normalize();	// Set length to one

		//addSingleObject( ax1, "NormalLot", normalLot, "g", cSys->OPnt() );

		// Distance from origin to normal
		const double currentDistance = distanceRange / 2 - (double) ( ( nDistance - 1 ) / 2 - currentIndex ) * deltaDistance;

		// Set lot length to distance
		normalLot.scale( currentDistance );
		//addSingleObject( ax1, "NormalLotScaled", normalLot, "g", cSys->OPnt() );

		// Point on Normal
		const pnt3 normalPoint{ normalLot };
		//addSingleObject( ax1, "NormalPoint", normalPoint, "g" );

		// Get point on pixel which lies on an arc with radius R
		const primitiveVec3 o = normalPoint.XYZ();
		const primitiveVec3 r = currentNormalVec.XYZ();
		const double R = arcRadius;

		//const double p = ( 2. * o.y * r.y + r.y * R + r.x ) / ( pow( r.y, 2. ) );
		//const double q = ( o.x + o.y * R + pow( o.y, 2. ) - 3. / 4. * pow( R, 2 ) ) / pow( r.y, 2. );

		const double p = ( 2.*o.y*r.y + r.y*R + 2.*o.x*r.x ) / ( pow( r.x, 2. ) + pow( r.y, 2. ) );
		const double q = ( pow( o.y, 2. ) + pow( o.x, 2. ) + o.y*R - 3./4.* pow( R, 2. ) ) / ( pow( r.x, 2. ) + pow( r.y, 2. ) );

		const double lambda = -p / 2. + sqrt( pow( p / 2., 2. ) - q );

		// The normal line of current pixel
		const line currentNormal{ currentNormalVec, normalPoint };
		//addSingleObject( ax1, "NormalLine", currentNormal, "c", arcRadius );

		// Point on Pixel
		const pnt3 pointOnPixel = currentNormal.getPnt( lambda );
		//addSingleObject( ax1, "PixelPoint", pointOnPixel, "c" );
		
		const line pixelNormal{ -currentNormalVec, pointOnPixel };

		

		// Other side => flip x values
		if( currentIndex > 0 ){

			pixelNormals.at( ( nDistance - 1 ) / 2 - currentIndex ) = pixelNormal;

			const line symPixelNormal{
				uvec3 { v3{ -pixelNormal.R().X(), pixelNormal.R().Y(), pixelNormal.R().Z() }, pixelNormal.R().CSys() },
				pnt3 { v3{ -pixelNormal.O().X(), pixelNormal.O().Y(), pixelNormal.O().Z() }, pixelNormal.O().CSys() }
			};

			pixelNormals.at( ( nDistance - 1 ) / 2 + currentIndex ) = symPixelNormal;


		}
		else{
		// Middle normal
			pixelNormals.at( ( nDistance - 1 ) / 2 ) = pixelNormal;
		}

		//pixelNormals.push_back( line{ currentNormalVec, pointOnPixel } );


		//const uvec3 surfVec1 = rotationAxis;
		//const uvec3 surfVec2 = surfVec1 ^ currentNormalVec;

		//const surf pixelSurface{ surfVec1, surfVec2, pointOnPixel };



	}


	

	// TODO: Create pixel from normals
	
	vector<surfLim> surfaces;
	double rowSize = 40;

	// 
	for( auto currentNormalIt = pixelNormals.cbegin() + 1; currentNormalIt < pixelNormals.cend() - 1; currentNormalIt++ ){
		auto previousNormalIt = currentNormalIt - 1;
		auto nextNormalIt = currentNormalIt + 1;
		
		// point in z-direction
		//const uvec3 surfVec2 = rotationAxis;

		// Point to the next normal
		//const uvec3 surfVec1 = surfVec2 ^ currentNormalIt->R();

		// Center point of surface
		//const pnt3 centerPoint = currentNormalIt->O();


		// Find size of pixel

		// Where do surface vector of neighbooring pixel meet?

		//addSingleObject( ax1, "CurrentNormalLine", *currentNormalIt, "c", arcRadius );

		const line lCurrentToPrevious{ -rotationAxis ^ currentNormalIt->R(), currentNormalIt->O() };
		const line lPreviousToCurrent{ rotationAxis ^ previousNormalIt->R(), previousNormalIt->O() };
		//addSingleObject( ax1, "CurrentToPrevious", lCurrentToPrevious, "b", ( previousNormalIt->O() - currentNormalIt->O() ).Length() );
		//addSingleObject( ax1, "PreviousToCurrent", lPreviousToCurrent, "b", ( previousNormalIt->O() - currentNormalIt->O() ).Length() );

		lineLine_Intersection currentPreviousIntersection{ lCurrentToPrevious, lPreviousToCurrent };
		const pnt3 currentPreviousIntersectionPoint = currentPreviousIntersection.Result().intersectionPoint;
		//addSingleObject( ax1, "InterssectionPoint", currentPreviousIntersectionPoint, "b" );

		const double currentPreviousParameter = -( currentPreviousIntersectionPoint - currentNormalIt->O() ).Length();


		const line lCurrentToNext{ rotationAxis ^ currentNormalIt->R(), currentNormalIt->O() };
		const line lNextToCurrent{ -rotationAxis ^ nextNormalIt->R(), nextNormalIt->O() };
		//addSingleObject( ax1, "CurrentToPrevious", lCurrentToNext, "b", ( nextNormalIt->O() - currentNormalIt->O() ).Length() );
		//addSingleObject( ax1, "PreviousToCurrent", lNextToCurrent, "b", ( nextNormalIt->O() - currentNormalIt->O() ).Length() );

		lineLine_Intersection currentNextIntersection{ lCurrentToNext, lNextToCurrent };
		const pnt3 currentNextIntersectionPoint = currentNextIntersection.Result().intersectionPoint;
		//addSingleObject( ax1, "InterssectionPoint", currentNextIntersectionPoint, "b" );

		const double currentNextParameter = ( currentNextIntersectionPoint - currentNormalIt->O() ).Length();


		if( currentNormalIt == pixelNormals.cbegin() + 1 ){
			
			const surfLim previousSurface{ rotationAxis ^ previousNormalIt->R(),
											rotationAxis,
											previousNormalIt->O(),
											currentPreviousParameter,
											( currentPreviousIntersectionPoint - previousNormalIt->O() ).Length(),
											-rowSize / 2,
											rowSize / 2 };
			surfaces.push_back( previousSurface );
		}




		const surfLim currentSurface{	rotationAxis ^ currentNormalIt->R(), 
										rotationAxis,
										currentNormalIt->O(),
										currentPreviousParameter,
										currentNextParameter,
										-rowSize/2,
										rowSize/2 };
		surfaces.push_back( currentSurface );

		if( currentNormalIt == pixelNormals.cend() - 2 ){

			const surfLim nextSurface{ rotationAxis ^ nextNormalIt->R(),
											rotationAxis,
											nextNormalIt->O(),
											-( currentNextIntersectionPoint - nextNormalIt->O() ).Length(),
											currentNextParameter,
											-rowSize / 2,
											rowSize / 2 };
			surfaces.push_back( nextSurface );
		}

	
	}


	addObject( ax1, "Normals", pixelNormals, "r", arcRadius );
	addObject( ax1, "Pixel", surfaces, "g", 0.5 );

	closeAxis( ax1 );


	return true;

}

bool test_detector(void) {


	detector test_detector = getTestDetector();

	vector<pixel> allPixel = test_detector.getPixel();

	ofstream ax1 = openAxis(path("./test_detector.txt"), true);

	addObject( ax1, "Detector", allPixel, "r", .2 );
	
	closeAxis(ax1);
	
	return true;

}

gantry getTestGantry( const idx2CR sinogramSize, const size_t raysPerPixel ){

	tubeParameter tubeParas{ .anodeVoltage_V = 53000,
								.anodeCurrent_A = 0.2,
								.anodeAtomicNumber = 74 };


	detectorRadonParameter radonParameter{
		sinogramSize,
		500
	};

	detectorIndipendentParameter indipendentParameter{
		2 * PI * ( 50. / 360. ),
		15.,
		false
	};

	gantry testGantry{ GLOBAL_CSYS()->createCopy( "Gantry system" ), raysPerPixel, tubeParas, radonParameter, indipendentParameter };

	return testGantry;
}


bool test_gantry( void ){

	gantry testGantry = getTestGantry( idx2CR{ 125, 31 }, 2 );

	ofstream ax1 = openAxis( path( "./test_gantry.txt" ), true );

	addObject( ax1, "Gantry", testGantry, "r", GANTRY_SPECIFIERS::ORIGIN | GANTRY_SPECIFIERS::BEAMS | GANTRY_SPECIFIERS::DETECTOR_SURFACES );

	testGantry.rotateCounterClockwise( 2.*PI/3 );

	//addObject( ax1, "Gantry", testGantry, "r", GANTRY_SPECIFIERS::ORIGIN | GANTRY_SPECIFIERS::DETECTOR_NORMALS | GANTRY_SPECIFIERS::DETECTOR_SURFACES );

	closeAxis( ax1 );

	return true;
}