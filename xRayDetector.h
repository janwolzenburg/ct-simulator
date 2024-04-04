#pragma once
/*********************************************************************
 * @file   detector.h
 * @brief  Detector class
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/



/*********************************************************************
	Includes
*********************************************************************/

#include <vector>
using std::vector;

#include "generelMath.h"
#include "detectorPixel.h"
#include "detectorProperties.h"
#include "projections.fwd.h"


/*********************************************************************
   Definitions
*********************************************************************/



/*!
 * @brief class for Ray detector
*/
class XRayDetector{

	public:

	/*!
	 * @brief constructor
	 * @param coordinate_system Coordinate system
	 * @param projections_properties Projection properties
	 * @param physical_properties Parameter indipendent of projection properties
	*/
	XRayDetector( CoordinateSystem* const coordinate_system, const ProjectionsProperties projections_properties, const PhysicalDetectorProperties physical_properties );

	/*!
	 * @brief get all detector pixel
	 * @return vector of pixels in one row
	*/
	vector<DetectorPixel> pixel_array( void ) const{ return pixel_array_; };
	
	/*!
	 * @brief get the physical parameters of detector
	 * @return Physical parameters of detector
	*/
	DetectorProperties properties( void ) const{ return properties_; };

	/*!
	 * @brief Update detector properties
	 * @param radon_properties New radon properties
	 * @param physical_properties New physical properties
	*/
	void UpdateProperties( const ProjectionsProperties radon_properties, const PhysicalDetectorProperties physical_properties );

	/*!
	 * @brief convert all comnverted pixel to this system
	 * @param target_coordinate_system Target
	*/
	void ConvertPixelArray( const CoordinateSystem* const target_coordinate_system );

	/*!
	 * @brief reset all pixel
	*/
	void ResetDetectedRayPorperties( void );

	/*!
	 * @brief detect Ray
	 * @param ray Ray to detect
	 * @param pixel_mutex Mutex for multi threaded access to pixel array
	 * @return true when ray hit the detector
	*/
	#ifdef TRANSMISSION_TRACKING
	bool DetectRay( Ray& ray, mutex& pixel_array_mutex );
	#else
	bool DetectRay( Ray& ray, mutex& pixel_array_mutex );
	#endif

	/*!
	 * @brief check if ray is detectable
	 * @param ray Ray to check. Expected pixel index is adjusted
	 * @return true when ray can reach the detector
	 */
	bool TryDetection( Ray& ray ) const;


	private:

	CoordinateSystem* coordinate_system_;			/*!< Local coordinate system*/
	vector<DetectorPixel> pixel_array_;				/*!< all pixel of detector*/
	vector<DetectorPixel> converted_pixel_array_;	/*!< pixel in different coordinate system*/

	DetectorProperties properties_;					/*!< properties*/

	optional<size_t> GetHitPixelIndex( Ray& ray ) const;

};

