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
 * @brief class for a gantry with xRay source and detector
*/
class Gantry{
	
	public:
	
	/*!
	 * @brief constructor
	 * @param coordinate_system coordinate system
	 * @param tube_properties parameter of x-ray tube
	 * @param projections_properties radon parameter of x-ray detector
	 * @param physical_detector_properties other parameter
	*/
	Gantry( CoordinateSystem* const coordinate_system,  const XRayTubeProperties tube_properties, const ProjectionsProperties projections_properties,
			const PhysicalDetectorProperties physical_detector_properties );

	/*!
	 * @brief get the coordinate system of gantry
	 * @return coordinate system of this gantry
	*/
	const CoordinateSystem* coordinate_system( void ) const{ return coordinate_system_; };

	/*!
	 * @brief get reference to detector instance
	 * @return reference to detector instance
	*/
	XRayDetector detector( void ) const{ return detector_; };

	/*!
	 * @brief get reference to tube instance
	 * @return reference to tube instance
	*/
	XRayTube tube( void ) const{ return tube_; };

	/*!
	 * @brief get all pixel from detector 
	 * @return vector with pixel
	*/
	vector<DetectorPixel> pixel_array( void ) const{ return detector_.pixel_array();  };

	/*!
	 * @brief get center of gantry
	 * @return center point
	*/
	Point3D GetCenter( void ) const{ return coordinate_system_->GetOriginPoint(); };

	/*!
	 * @brief update tube and detector properties
	 * @param tube_properties new tube proerties
	 * @param projections_properties new radon properties
	 * @param physical_detector_properties new detector properties
	*/
	void UpdateTubeAndDetectorProperties( const XRayTubeProperties tube_properties, const ProjectionsProperties projections_properties,
								const PhysicalDetectorProperties physical_detector_properties );

	/*!
	 * @brief rotate gantry counter clockwise around ZAxis
	 * @param angle rotation angle
	*/
	void RotateCounterClockwise( const double angle );

	/*!
	 * @brief translate gantry in z-direction
	 * @param distance distance to translate
	*/
	void TranslateInZDirection( const double distance );

	/*!
	 * @brief radiate model with beam
	 * @param model model to radiate
	 * @param tomography_properties tomogrpahy properties
	*/
	void RadiateModel( const Model& model, TomographyProperties tomography_properties,
										 RayScattering& scattering_information) ;

	/*!
	 * @brief reset gantry to its initial position and reset detector
	*/
	void ResetGantry( void );


	private:
	
	CoordinateSystem* coordinate_system_;					/*!< coordinate system*/
	PrimitiveCoordinateSystem initial_position_;	/*!< initial position of coordinate system*/

	XRayDetector detector_;												/*!< x-ray detector*/
	XRayTube tube_;																/*!< x-ray source*/
	

	/*!
	 * @brief thread function to speed up transmission of multiple rays through model
	 * @param model model to radiate through
	 * @param tomography_properties properties of tomography
	 * @param ray_scattering reference to object with information about Ray scattering
	 * @param scattering_properties_mutex mutex for ray scattering
	 * @param rays reference to vector with rays to transmit
	 * @param second_to_last_iteration flag to indicate that this is the last iteration
	 * @param current_ray_index index of the next Ray in vector to transmit. Will be changed at each call
	 * @param current_ray_index_mutex mutex instance for Ray index
	 * @param rays_for_next_iteration reference to vector which hold the rays for the next iteration
	 * @param rays_for_next_iteration_mutex mutex for vector with rays for next iteration 
	 * @param detector reference to ray detector
	 * @param detector_mutex mutex for the detector instance
	 * @param dedicated_rng a dedicated RNG with exclusive access
	*/
	static void TransmitRaysThreaded(	const Model& model,						const TomographyProperties& tomography_properties, 
										RayScattering& ray_scattering, 
										mutex& scattering_properties_mutex,	const vector<Ray>& rays, const bool second_to_last_iteration,
										size_t& current_ray_index,				mutex& current_ray_index_mutex,
										vector<Ray>& rays_for_next_iteration,	mutex& rays_for_next_iteration_mutex,
										XRayDetector& detector,					mutex& detector_mutex,
										RandomNumberGenerator& dedicated_rng);

};
