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
#include "xRayDetector.h"
#include "model.h"
#include "rayScattering.h"
#include "tomography.fwd.h"


/*********************************************************************
   Definitions
*********************************************************************/

/*!
 * @brief Class for a gantry with xRay source and detector
*/
class Gantry{
	
	public:
	
	/*!
	 * @brief Constructor
	 * @param coordinate_system Coordinate system
	 * @param tube_properties Parameter of xRay tube
	 * @param radon_properties Radon parameter of xRay detector
	 * @param physical_detector_properties Other parameter
	*/
	Gantry( CoordinateSystem* const coordinate_system,  const XRayTubeProperties tube_properties, const ProjectionsProperties radon_properties,
			const PhysicalDetectorProperties physical_detector_properties );

	/*!
	 * @brief Get the coordinate system of gantry
	 * @return Coordinate system of this gantry
	*/
	const CoordinateSystem* coordinate_system( void ) const{ return coordinate_system_; };

	/*!
	 * @brief Get reference to detector instance
	 * @return Reference to detector instance
	*/
	XRayDetector detector( void ) const{ return detector_; };

	/*!
	 * @brief Get reference to tube instance
	 * @return Reference to tube instance
	*/
	XRayTube tube( void ) const{ return tube_; };

	/*!
	 * @brief Get all pixel from detector 
	 * @return Vector with pixel
	*/
	vector<DetectorPixel> pixel_array( void ) const{ return detector_.pixel_array();  };

	/*!
	 * @brief Get center of gantry
	 * @return Center point
	*/
	Point3D GetCenter( void ) const{ return coordinate_system_->GetOriginPoint(); };

	/*!
	 * @brief Update tube and detector properties
	 * @param tube_properties New tube proerties
	 * @param radon_properties New radon properties
	 * @param physical_detector_properties New detector properties
	*/
	void UpdateTubeAndDetectorProperties( const XRayTubeProperties tube_properties, const ProjectionsProperties radon_properties,
								const PhysicalDetectorProperties physical_detector_properties );

	/*!
	 * @brief Rotate gantry counter clockwise around ZAxis
	 * @param angle Rotation angle
	*/
	void RotateCounterClockwise( const double angle );

	/*!
	 * @brief Translate gantry in z-direction
	 * @param distance Distance to translate
	*/
	void TranslateInZDirection( const double distance );

	/*!
	 * @brief Radiate model with beam
	 * @param model Model to radiate
	 * @param tomography_properties tomogrpahy properties
	*/
	void RadiateModel( const Model& model, TomographyProperties tomography_properties ) ;

	/*!
	 * @brief Reset gantry to its initial position and reset detector
	*/
	void ResetGantry( void );


	private:
	
	CoordinateSystem* coordinate_system_;			/*!< Coordinate system*/
	PrimitiveCoordinateSystem initial_position_;	/*!< Initial position of coordinate system*/

	XRayDetector detector_;							/*!< Ray detector*/
	XRayTube tube_;									/*!< xRay source*/
	

	/*!
	 * @brief Thread function to speed up transmission of multiple rays through model
	 * @param model Model to radiate through
	 * @param tomography_properties Properties of tomography
	 * @param rayScatterAngles Reference to object with information about Ray scattering
	 * @param rays Reference to vector with rays to transmit
	 * @param repeat_transmission_after_scattering Flag to enable re-transmission of ray, when it has been scattered
	 * @param current_ray_index Index of the next Ray in vector to transmit. Will be changed at each call
	 * @param current_ray_index_mutex Mutex instance for Ray index
	 * @param rays_for_next_iteration Reference to vector which hold the rays for the next iteration
	 * @param rays_for_next_iteration_mutex Mutex for vector with rays for next iteration 
	 * @param detector Reference to Ray detector
	 * @param detector_mutexMutex for the detector Object
	*/
	static void TransmitRaysThreaded(	const Model& model,						const TomographyProperties& tomography_properties, 
										const RayScattering& scatter_angles,	const vector<Ray>& rays, 
										const bool repeat_transmission_after_scattering,
										size_t& current_ray_index,				mutex& current_ray_index_mutex,
										vector<Ray>& rays_for_next_iteration,	mutex& rays_for_next_iteration_mutex,
										XRayDetector& detector,					mutex& detector_mutex );

};
