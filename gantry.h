#pragma once
/*********************************************************************
 * @file   gantry.h
 * @brief  Gantry class
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/



 /*********************************************************************
	Includes
 *********************************************************************/
#include <vector>
using std::vector;

#include <mutex>
using std::mutex;

#include "generelMath.h"
#include "ray.h"
#include "xRayTube.h"
#include "detector.h"
#include "model.h"
#include "scattering.h"
#include "tomography.fwd.h"


/*********************************************************************
   Definitions
*********************************************************************/

/*!
 * @brief Class for a gantry with xRay source and detector
*/
class gantry {
	

	public:
	
	/*!
	 * @brief Constructor
	 * @param coordinate_system Coordinate system
	 * @param raysPerPixel_ Amount of rays per pixel in beam
	 * @param tubeParameters Parameter of xRay tube
	 * @param radonParameters Radon parameter of xRay detector
	 * @param indipendentParameter Other parameter
	*/
	gantry( CoordinateSystem* const coordinate_system,  const XRayTubeProperties tubeParameter_, const radonProperties radonParameter,
			const detectorIndipendentParameter indipendentParameter );

	/*!
	 * @brief Get all rays from tube
	 * @param exposureTime Exposure time
	 * @return Vector with all rays in beam
	*/
	vector<Ray> getBeam( const double exposureTime ) const;

	/*!
	 * @brief Get all pixel from detector 
	 * @return Vector with pixel
	*/
	vector<pixel> getPixel( void ) const{ return rayDetector.getPixel();  };

	/*!
	 * @brief Get radius of gantry
	 * @return Radius
	*/
	double Radius( void ) const{ return radius; };

	/*!
	 * @brief Get center of gantry
	 * @return Center point
	*/
	Point3D Center( void ) const{ return cSys->GetOriginPoint(); };

	/*!
	 * @brief Rotate gantry counter clockwise around ZAxis
	 * @param GetAngle Rotation GetAngle
	*/
	void rotateCounterClockwise( const double angle );

	/*!
	 * @brief Radiate model with beam
	 * @param radModel Model to radiate
	*/
	void radiate( const Model& radModel, const tomographyParameter voxel_data_ ) ;
	
	/*!
	 * @brief Reset ganrtry to its initial position
	*/
	void reset( void );

	/*!
	 * @brief Get the coordinate system of gantry
	 * @return Coordinate system of this gantry
	*/
	CoordinateSystem* CSys( void ) const{ return cSys; };

	/*!
	 * @brief Get the detector radon parameters
	 * @param coordinate_system_ Reference coordinate system for radon transform
	 * @return 
	*/
	radonProperties getDetectorParameter(void) const{ return rayDetector.getSignalParameter(); };

	/*!
	 * @brief Get reference to scattering member object
	 * @return Reference to member 
	*/
	rayScattering& RayScattering( void ){ return rayScatterAngles; };

	/*!
	 * @brief Get reference to detector instance
	 * @return Reference to detector instance
	*/
	const detector& Detector( void ) const{ return rayDetector; };

	/*!
	 * @brief Get reference to tube instance
	 * @return Reference to tube instance
	*/
	const XRayTube& Tube( void ) const{ return raySource; };


	private:
	
	CoordinateSystem* cSys;						/*!<Coordinate system*/
	PrimitiveCoordinateSystem resetPostition;	/*!<Initial position of coordinate system*/

	detector rayDetector;				/*!<Ray detector*/
	XRayTube raySource;					/*!<xRay source*/
	
	double radius;						/*!<Radius of gantry*/

	rayScattering rayScatterAngles;		/*!<Object with information about scattering and GetAngle propabilities*/

	/*!
	 * @brief Thread function to speed up transmission of multiple rays through model
	 * @param radModel Reference to model
	 * @param enableScattering Flag that enables scattering
	 * @param rayScatterAngles Reference to object with information about Ray scattering
	 * @param rays Reference to vector with rays to transmit
	 * @param sharedCurrentRayIndex Index of the next Ray in vector to transmit. Will be changed at each call
	 * @param currentRayIndexMutex Mutex instance for Ray index
	 * @param raysForNextIteration Reference to vector which hold the rays for the next iteration
	 * @param detectorMutex Mutex for the detector Object
	 * @param rayDetector Reference to Ray detector
	 * @param iterationMutex Mutex for vector with rays for next iteration
	*/
	static void transmitRays( const Model& radModel, const tomographyParameter& voxel_data_, const rayScattering& rayScatterAngles,
								const vector<Ray>& rays, size_t& sharedCurrentRayIndex, mutex& currentRayIndexMutex,
								vector<Ray>& raysForNextIteration, mutex& detectorMutex,
								detector& rayDetector, mutex& iterationMutex );

};
