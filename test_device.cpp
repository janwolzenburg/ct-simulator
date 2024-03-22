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
#include "coordinateSystemTree.h"
#include "test_device.h"
#include "xRayTube.h"
#include "xRayDetector.h"
#include "plotting.h"
#include "gantry.h"
#include "projections.h"
#include "backprojectionFilter.h"

XRayDetector getTestDetector( void ){
	// 64 x 32 points in radon space
	// 500mm measure field
	ProjectionsProperties radonParameter{
		63, 21,
		500
	};

	PhysicalDetectorProperties indipendentParameter{
		1000.,
		10
	};

	// 50 degree angle
	XRayDetector testDetector{ GlobalSystem()->CreateCopy( "Detector system" ), radonParameter, indipendentParameter };

	return testDetector;
}

bool test_tube(void) {

	XRayTubeProperties tubeParas{	53000,
									0.2,
									XRayTubeProperties::Thungsten,
									1, true, al_filter_cut_off_energy_eV, -al_filter_gradient_factor };

	XRayTube testTube{ GlobalSystem()->CreateCopy( "Tube system" ), tubeParas };


	XRayDetector test_detector = getTestDetector();

	vector<DetectorPixel> allPixel = test_detector.pixel_array();

	vector<Ray> beam = testTube.GetEmittedBeam( allPixel, test_detector.properties().detector_focus_distance );

	ofstream ax1 = openAxis( path( "./test_tube.txt" ), true );

	addObject( ax1, "Beam", beam, "r", 1. );

	closeAxis( ax1 );

	return true;
}


/*
bool test_nonUniformDetector( void ){

	ofstream ax1 = openAxis( path( "./test_nonUniformDetector.txt" ), true );

	CoordinateSystem* cSys = GlobalSystem()->CreateCopy( "Detector system" );

	const size_t nTheta = 100;
	const size_t nDistance = ForceOdd( 33 );

	const double distanceRange = 500;
	const double arcRadius = 1000;

	const double deltaTheta = PI / static_cast<double>( ( nTheta - 1 );
	const double deltaDistance = distanceRange / static_cast<double>( ( nDistance - 1 );

	const UnitVector3D middleNormalVec = cSys->GetEy();			// Middle normal is the negative y axis
	const UnitVector3D rotationAxis = cSys->GetEz();			//Rotation axis is z axis



	vector<Line> pixelNormals( nDistance );							// all normals

	// iterate one "half" of normals
	for( size_t currentIndex = 0; currentIndex <= ( nDistance - 1 ) / 2; currentIndex++ ){

		// Rotation GetAngle to rotate the middle normal by
		const double rotationAngle = static_cast<double>( ( currentIndex ) * deltaTheta;

		// Rotated normal
		const UnitVector3D currentNormalVec = middleNormalVec.RotateConstant( rotationAxis, rotationAngle );

		//addSingleObject( ax1, "RotatedNormalVec", currentNormalVec, "g", coordinate_system_->Origin() );

		// Lot on normal perpendicualr to rotation axis
		Vector3D normalLot =  rotationAxis ^ currentNormalVec;
		normalLot.Normalise();	// Set length_ to one

		//addSingleObject( ax1, "NormalLot", normalLot, "g", coordinate_system_->Origin() );

		// distance from origin_ to normal
		const double currentDistance = distanceRange / 2 - static_cast<double>( ( ( nDistance - 1 ) / 2 - currentIndex ) * deltaDistance;

		// Set lot length_ to distance
		normalLot.Scale( currentDistance );
		//addSingleObject( ax1, "NormalLotScaled", normalLot, "g", coordinate_system_->Origin() );

		// point on GetCenterNormal
		const Point3D normalPoint{ normalLot };
		//addSingleObject( ax1, "NormalPoint", normalPoint, "g" );

		// get point on pixel which lies on an arc with radius direction
		const PrimitiveVector3 o = normalPoint.GetComponents();
		const PrimitiveVector3 r = currentNormalVec.GetComponents();
		const double R = arcRadius;


		const double p = ( 2.*o.y*r.y + r.y*R + 2.*o.x*r.x ) / ( pow( r.x, 2. ) + pow( r.y, 2. ) );
		const double q = ( pow( o.y, 2. ) + pow( o.x, 2. ) + o.y*R - 3./4.* pow( R, 2. ) ) / ( pow( r.x, 2. ) + pow( r.y, 2. ) );

		const double lambda = -p / 2. + sqrt( pow( p / 2., 2. ) - q );

		// the normal Line of current pixel
		const Line currentNormal{ currentNormalVec, normalPoint };
		//addSingleObject( ax1, "GetCenterNormal", currentNormal, "c", arcRadius );

		// point on Pixel
		const Point3D pointOnPixel = currentNormal.GetPoint( lambda );
		//addSingleObject( ax1, "PixelPoint", pointOnPixel, "c" );
		
		const Line pixelNormal{ -currentNormalVec, pointOnPixel };

		

		// other side => flip x values
		if( currentIndex > 0 ){

			pixelNormals.at( ( nDistance - 1 ) / 2 - currentIndex ) = pixelNormal;

			const Line symPixelNormal{
				UnitVector3D { Tuple3D{ -pixelNormal.direction().X(), pixelNormal.direction().Y(), pixelNormal.direction().Z() }, pixelNormal.direction().GetCoordinateSystem() },
				Point3D { Tuple3D{ -pixelNormal.origin().X(), pixelNormal.origin().Y(), pixelNormal.origin().Z() }, pixelNormal.origin().GetCoordinateSystem() }
			};

			pixelNormals.at( ( nDistance - 1 ) / 2 + currentIndex ) = symPixelNormal;


		}
		else{
		// Middle normal
			pixelNormals.at( ( nDistance - 1 ) / 2 ) = pixelNormal;
		}
	}

	vector<BoundedSurface> surfaces;
	double rowSize = 40;

	// 
	for( auto currentNormalIt = pixelNormals.cbegin() + 1; currentNormalIt < pixelNormals.cend() - 1; currentNormalIt++ ){
		auto previousNormalIt = currentNormalIt - 1;
		auto nextNormalIt = currentNormalIt + 1;

		const Line lCurrentToPrevious{ -rotationAxis ^ currentNormalIt->direction(), currentNormalIt->origin() };
		const Line lPreviousToCurrent{ rotationAxis ^ previousNormalIt->direction(), previousNormalIt->origin() };

		LineLineIntersection currentPreviousIntersection = IntersectLines( lCurrentToPrevious, lPreviousToCurrent );
		const Point3D currentPreviousIntersectionPoint = currentPreviousIntersection.intersection_point_;

		const double currentPreviousParameter = -( currentPreviousIntersectionPoint - currentNormalIt->origin() ).length();


		const Line lCurrentToNext{ rotationAxis ^ currentNormalIt->direction(), currentNormalIt->origin() };
		const Line lNextToCurrent{ -rotationAxis ^ nextNormalIt->direction(), nextNormalIt->origin() };

		LineLineIntersection currentNextIntersection = IntersectLines( lCurrentToNext, lNextToCurrent );
		const Point3D currentNextIntersectionPoint = currentNextIntersection.intersection_point_;

		const double currentNextParameter = ( currentNextIntersectionPoint - currentNormalIt->origin() ).length();


		if( currentNormalIt == pixelNormals.cbegin() + 1 ){
			
			const BoundedSurface previousSurface{ rotationAxis ^ previousNormalIt->direction(),
											rotationAxis,
											previousNormalIt->origin(),
											NumberRange{ currentPreviousParameter,
											( currentPreviousIntersectionPoint - previousNormalIt->origin() ).length() },
											NumberRange{ -rowSize / 2,
											rowSize / 2 } };
			surfaces.push_back( previousSurface );
		}


		const BoundedSurface currentSurface{	rotationAxis ^ currentNormalIt->direction(), 
										rotationAxis,
										currentNormalIt->origin(),
										NumberRange{ currentPreviousParameter,
										currentNextParameter },
										NumberRange{ -rowSize/2,
										rowSize/2 } };
		surfaces.push_back( currentSurface );

		if( currentNormalIt == pixelNormals.cend() - 2 ){

			const BoundedSurface nextSurface{ rotationAxis ^ nextNormalIt->direction(),
											rotationAxis,
											nextNormalIt->origin(),
											NumberRange{ -( currentNextIntersectionPoint - nextNormalIt->origin() ).length(),
											currentNextParameter },
											NumberRange{ -rowSize / 2,
											rowSize / 2 } };
			surfaces.push_back( nextSurface );
		}

	
	}


	addObject( ax1, "Normals", pixelNormals, "r", arcRadius );
	addObject( ax1, "Pixel", surfaces, "g", 0.5 );

	closeAxis( ax1 );


	return true;

}*/

bool test_modifiedDetector( void ){

	ofstream ax1 = openAxis( path( "./test_modifiedDetector.txt" ), true );

	CoordinateSystem* cSys = GlobalSystem()->CreateCopy( "Detector system" );


	addSingleObject( ax1, "origin", cSys->GetOriginPoint(), "c" );

	const size_t nTheta = 700;
	const size_t nDistance = ForceOdd( 301 );

	const double distanceRange = 500;
	const double detectorCenterDistance = 500;

	const double deltaTheta = PI / static_cast<double>( nTheta - 1 );
	const double deltaDistance = distanceRange / static_cast<double>( nDistance - 1 );

	// important vectors
	const UnitVector3D middleNormalVector = cSys->GetEy();					// y-axis of coordinate system is the middle normal vector
	const UnitVector3D rotationVector = cSys->GetEz();						// pixel normals should lie in xy-plane. The middle normal vector will be rotated around this vector


	// all pixel normals

	vector<DetectorPixel> allPixel;

	Line previousNormal;			// getCenterNormal of previous pixel
	double previousPixelSize;		// Size of previous pixel


	// iterate through half of pixel normals. Second half is created by symmetry
	// Normals will be created inside to outside
	for( size_t currentIndex = 0; currentIndex <= ( nDistance - 1 ) / 2; currentIndex++ ){

		// angle to rotate the middle normal vector by
		const double rotationAngle = static_cast<double>( currentIndex ) * deltaTheta;

		// Middle normal vector rotation by rotation angle around rotation vector
		const UnitVector3D currentNormalVector = middleNormalVector.RotateConstant( rotationVector, rotationAngle );


		// find a point with the distance corresponding to distance in sinogram
		// the point's origin_ vector must be perpendicular to the current normal vector

		// the lot is perpendicular to the current normal vector and it lies in xy-plane
		const UnitVector3D normalLot = rotationVector ^ currentNormalVector;

		// distance from origin_ to normal. Is the distance in the sinogram
		const double currentDistance = distanceRange / 2. - ( static_cast<double>( nDistance - 1 ) / 2. - static_cast<double>( currentIndex ) ) * deltaDistance;

		// point which lies on the current normal and has the correct distance from the origin_ 
		const Point3D normalPoint = Vector3D{ normalLot } * currentDistance;

		// the current normal 
		const Line currentNormal{ currentNormalVector, normalPoint };

		// index of normal in vector
		//const size_t currentNormalIndex = ( nDistance - 1 ) / 2 - currentIndex;

		Point3D currentPixelOrigin;
		double currentPixelSize;


		// "Middle" normal
		if( currentIndex == 0 ){
			// this is the starting point
			currentPixelOrigin = currentNormal.GetPoint( detectorCenterDistance );
			
			// first pixel size so that the neighbooring pixel intersects at half angle
			currentPixelSize = 2 * tan( deltaTheta / 2. ) * ( detectorCenterDistance + deltaDistance / sin( deltaTheta ) );

		}
		else{
			// intersection point of pixel
			const Point3D pixelIntersection = previousNormal.origin() + ( previousNormal.direction() ^ rotationVector ) * previousPixelSize / 2.;

			// Lot vector from current normal to intersection point. Vector is pointing to the normal
			const Vector3D pixelIntersectionLot = currentNormal.GetLot( pixelIntersection );

			// get the pixel normal's origin_ which lies on the shortest Line connection the intersection with current normal
			currentPixelOrigin = pixelIntersection + pixelIntersectionLot;

			// pixel size is double the lot length
			currentPixelSize = 2 * pixelIntersectionLot.length();
		}

		// create current pixel normal pointing to center
		const Line pixelNormal{ -currentNormalVector, currentPixelOrigin };

		// Store for next pixel
		previousNormal = pixelNormal;
		previousPixelSize = currentPixelSize;

		// Vector perpendicualr to the normal pointing to the next pixel
		const UnitVector3D currentSurfaceVector = -pixelNormal.direction() ^ rotationVector;

		// add pixel
		allPixel.emplace_back( BoundedSurface{ 
								currentSurfaceVector,
							   rotationVector,
							   pixelNormal.origin(),
							   -currentPixelSize / 2,
							   currentPixelSize / 2,
							   -10,
							   10 } );

		// add mirrored when not middle pixel
		if( currentIndex > 0 ){

			// Mirror current normal around y-axis
			const Line mirroredPixelNormal{
				pixelNormal.direction().NegateXComponent(),
				pixelNormal.origin().NegateXComponent()
			};

			// add mirrored pixel
			const UnitVector3D mirroredSurfaceVector = -mirroredPixelNormal.direction() ^ rotationVector;
			allPixel.emplace_back( BoundedSurface{ mirroredSurfaceVector,
								   rotationVector,
								   mirroredPixelNormal.origin(),
								   -currentPixelSize / 2,
								   currentPixelSize / 2,
								   -10,
								   10 } );
		}

	}


	vector<Line> allNormals;
	for( auto& p : allPixel ){
		allNormals.push_back( p.NormalLine() );
	}


	addObject( ax1, "Pixel", allPixel, "g", 0.5 );
	addObject( ax1, "Normals", allNormals, "r", detectorCenterDistance * 2. );
	closeAxis( ax1 );
	return true;
}

bool test_detector(void) {


	XRayDetector test_detector = getTestDetector();

	vector<DetectorPixel> allPixel = test_detector.pixel_array();

	ofstream ax1 = openAxis(path("./test_detector.txt"), true);

	addObject( ax1, "Detector", allPixel, "r", .2 );
	
	vector<Line> pixelNormals;

	for( DetectorPixel currentPixel : allPixel ){
		pixelNormals.push_back( currentPixel.NormalLine() );
	}

	addObject( ax1, "DetectorNormals", pixelNormals, "r", test_detector.properties().detector_focus_distance );

	closeAxis(ax1);
	
	return true;

}

Gantry getTestGantry( const GridIndex sinogramSize, const size_t raysPerPixel ){

	XRayTubeProperties tubeParas{
								100000,
								0.2,
								XRayTubeProperties::Thungsten, 
								raysPerPixel, true, al_filter_cut_off_energy_eV, al_filter_gradient_factor };


	ProjectionsProperties radonParameter{
		sinogramSize.c,
		sinogramSize.r,
		500
	};

	PhysicalDetectorProperties indipendentParameter{
		1000.,
		50
	};

	Gantry testGantry{ GlobalSystem()->CreateCopy( "Gantry system" ), tubeParas, radonParameter, indipendentParameter };

	return testGantry;
}


bool test_gantry( void ){


	Gantry testGantry = getTestGantry( GridIndex{ 600, 200 }, 3 );
	const CoordinateSystem* const radonCSys = testGantry.coordinate_system()->CreateCopy( "Radon System" );

	ofstream ax1 = openAxis( path( "./test_gantry.txt" ), true );
	ofstream ax2 = openAxis( path( "./test_gantry_sinogram.txt" ), true );

	addObject( ax1, "Gantry", testGantry, "r", GANTRY_SPECIFIERS::ORIGIN | GANTRY_SPECIFIERS::BEAMS | GANTRY_SPECIFIERS::DETECTOR_SURFACES );
	

	vector<RadonPoint> points;
	for( auto& px : testGantry.pixel_array() ) points.emplace_back( RadonCoordinates{ radonCSys, px.NormalLine() }, 1. );
	

	//testGantry.rotateCounterClockwise( static_cast<double>( testGantry.getDetectorParameter().numberPoints.r - 1 ) / 2. * testGantry.getDetectorParameter().resolution.c );

	//addObject( ax1, "Gantry", testGantry, "g", GANTRY_SPECIFIERS::ORIGIN | GANTRY_SPECIFIERS::DETECTOR_NORMALS | GANTRY_SPECIFIERS::DETECTOR_SURFACES );

	//for( auto& px : testGantry.getPixel() ) points.emplace_back( radonCoords{ radonCSys, px.NormalLine() }, 2. );
	addSingleObject( ax2, "RadonPoints", points, "Angle;Distance;Energy;Dots" );

	closeAxis( ax2 );
	closeAxis( ax1 );


	return true;
}