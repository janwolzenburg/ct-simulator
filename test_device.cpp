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
#include "radonTransform.h"

detector getTestDetector( void ){
	// 64 x 32 points in radon space
	// 500mm measure field
	detectorRadonParameter radonParameter{
		idx2CR{ 63, 21 },
		500
	};

	detectorIndipendentParameter indipendentParameter{
		1000.,
		10,
		false
	};

	// 50 degree angle
	detector testDetector{ GLOBAL_CSYS()->createCopy( "Detector system" ), radonParameter, indipendentParameter };

	return testDetector;
}

bool test_tube(void) {

	tubeParameter tubeParas{	.anodeVoltage_V = 53000,
								.anodeCurrent_A = 0.2,
								.anodeAtomicNumber = 74 };

	tube testTube{ GLOBAL_CSYS()->createCopy( "Tube system" ), tubeParas };


	detector test_detector = getTestDetector();

	vector<pixel> allPixel = test_detector.getPixel();

	vector<ray> beam = testTube.getBeam( allPixel, test_detector.getPhysicalParameters().detectorFocusDistance, 2);

	ofstream ax1 = openAxis( path( "./test_tube.txt" ), true );

	addObject( ax1, "Beam", beam, "r", 1. );

	closeAxis( ax1 );

	return true;
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



	vector<line> pixelNormals( nDistance );							// All normals

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
	}

	vector<surfLim> surfaces;
	double rowSize = 40;

	// 
	for( auto currentNormalIt = pixelNormals.cbegin() + 1; currentNormalIt < pixelNormals.cend() - 1; currentNormalIt++ ){
		auto previousNormalIt = currentNormalIt - 1;
		auto nextNormalIt = currentNormalIt + 1;

		const line lCurrentToPrevious{ -rotationAxis ^ currentNormalIt->R(), currentNormalIt->O() };
		const line lPreviousToCurrent{ rotationAxis ^ previousNormalIt->R(), previousNormalIt->O() };

		lineLine_Intersection currentPreviousIntersection{ lCurrentToPrevious, lPreviousToCurrent };
		const pnt3 currentPreviousIntersectionPoint = currentPreviousIntersection.Result().intersectionPoint;

		const double currentPreviousParameter = -( currentPreviousIntersectionPoint - currentNormalIt->O() ).Length();


		const line lCurrentToNext{ rotationAxis ^ currentNormalIt->R(), currentNormalIt->O() };
		const line lNextToCurrent{ -rotationAxis ^ nextNormalIt->R(), nextNormalIt->O() };

		lineLine_Intersection currentNextIntersection{ lCurrentToNext, lNextToCurrent };
		const pnt3 currentNextIntersectionPoint = currentNextIntersection.Result().intersectionPoint;

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

bool test_modifiedDetector( void ){

	ofstream ax1 = openAxis( path( "./test_modifiedDetector.txt" ), true );

	cartCSys* cSys = GLOBAL_CSYS()->createCopy( "Detector system" );


	addSingleObject( ax1, "Origin", cSys->OPnt(), "c" );

	const size_t nTheta = 700;
	const size_t nDistance = FOdd( 301 );

	const double distanceRange = 500;
	const double detectorCenterDistance = 500;

	const double deltaTheta = PI / (double) ( nTheta - 1 );
	const double deltaDistance = distanceRange / (double) ( nDistance - 1 );

	// Important vectors
	const uvec3 middleNormalVector = cSys->EyVec();					// y-axis of coordinate system is the middle normal vector
	const uvec3 rotationVector = cSys->EzVec();						// Pixel normals should lie in xy-plane. The middle normal vector will be rotated around this vector


	// All pixel normals

	vector<pixel> allPixel;

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
		const pnt3 normalPoint = vec3{ normalLot } * currentDistance;

		// The current normal 
		const line currentNormal{ currentNormalVector, normalPoint };

		// Index of normal in vector
		const size_t currentNormalIndex = ( nDistance - 1 ) / 2 - currentIndex;

		pnt3 currentPixelOrigin;
		double currentPixelSize;


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
							   -10,
							   10 );

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
								   -10,
								   10 );
		}

	}


	vector<line> allNormals;
	for( auto& p : allPixel ){
		allNormals.push_back( p.NormalLine() );
	}


	addObject( ax1, "Pixel", allPixel, "g", 0.5 );
	addObject( ax1, "Normals", allNormals, "r", detectorCenterDistance * 2. );
	closeAxis( ax1 );
	return true;
}

bool test_detector(void) {


	detector test_detector = getTestDetector();

	vector<pixel> allPixel = test_detector.getPixel();

	ofstream ax1 = openAxis(path("./test_detector.txt"), true);

	addObject( ax1, "Detector", allPixel, "r", .2 );
	
	vector<line> pixelNormals;

	for( pixel currentPixel : allPixel ){
		pixelNormals.push_back( currentPixel.NormalLine() );
	}

	addObject( ax1, "DetectorNormals", pixelNormals, "r", test_detector.getPhysicalParameters().detectorFocusDistance );

	closeAxis(ax1);
	
	return true;

}

gantry getTestGantry( const idx2CR sinogramSize, const size_t raysPerPixel ){

	tubeParameter tubeParas{ .anodeVoltage_V = 100000,
								.anodeCurrent_A = 0.2,
								.anodeAtomicNumber = 74 };


	detectorRadonParameter radonParameter{
		sinogramSize,
		500
	};

	detectorIndipendentParameter indipendentParameter{
		1000.,
		50,
		false
	};

	gantry testGantry{ GLOBAL_CSYS()->createCopy( "Gantry system" ), raysPerPixel, tubeParas, radonParameter, indipendentParameter };

	return testGantry;
}


bool test_gantry( void ){

	gantry testGantry = getTestGantry( idx2CR{ 600, 200 }, 3 );
	const cartCSys* const radonCSys = testGantry.CSys()->createCopy( "Radon System" );

	ofstream ax1 = openAxis( path( "./test_gantry.txt" ), true );
	ofstream ax2 = openAxis( path( "./test_gantry_sinogram.txt" ), true );

	addObject( ax1, "Gantry", testGantry, "r", GANTRY_SPECIFIERS::ORIGIN | GANTRY_SPECIFIERS::BEAMS | GANTRY_SPECIFIERS::DETECTOR_SURFACES );
	

	vector<radonPoint> points;
	for( auto& px : testGantry.getPixel() ) points.emplace_back( radonCoords{ radonCSys, px.NormalLine() }, 1. );
	

	testGantry.rotateCounterClockwise( ( testGantry.getDetectorParameter().numberPoints.row - 1 ) / 2. * testGantry.getDetectorParameter().resolution.col );

	//addObject( ax1, "Gantry", testGantry, "g", GANTRY_SPECIFIERS::ORIGIN | GANTRY_SPECIFIERS::DETECTOR_NORMALS | GANTRY_SPECIFIERS::DETECTOR_SURFACES );

	for( auto& px : testGantry.getPixel() ) points.emplace_back( radonCoords{ radonCSys, px.NormalLine() }, 2. );
	addSingleObject( ax2, "RadonPoints", points, "Angle;Distance;Energy;Dots" );

	closeAxis( ax2 );
	closeAxis( ax1 );


	return true;
}