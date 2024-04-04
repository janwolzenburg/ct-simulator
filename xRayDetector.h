#pragma once
/*********************************************************************
 * @file   xRayDetector.h
 * @brief  detector class
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
	 * @param coordinate_system coordinate system
	 * @param projections_properties projection properties
	 * @param physical_properties physical properties indipendent of projection properties
	*/
	XRayDetector( CoordinateSystem* const coordinate_system, const ProjectionsProperties projections_properties, const PhysicalDetectorProperties physical_properties );

	/*!
	 * @brief get all detector pixel
	 * @return vector of pixels in one row
	*/
	vector<DetectorPixel> pixel_array( void ) const{ return pixel_array_; };
	
	/*!
	 * @brief get the physical parameters of detector
	 * @return physical parameters of detector
	*/
	DetectorProperties properties( void ) const{ return properties_; };

	/*!
	 * @brief update detector properties
	 * @param projections_properties new radon properties
	 * @param physical_properties new physical properties
	*/
	void UpdateProperties( const ProjectionsProperties projections_properties, const PhysicalDetectorProperties physical_properties );

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
	 * @brief detect ray
	 * @param ray ray to detect
	 * @param pixel_mutex mutex for multi threaded access to pixel array
	 * @return true when ray hit the detector
	*/
	#ifdef TRANSMISSION_TRACKING
	bool DetectRay( Ray& ray, mutex& pixel_array_mutex );
	#else
	bool DetectRay( Ray& ray, mutex& pixel_array_mutex );
	#endif

	/*!
	 * @brief check if ray is detectable
	 * @param ray Ray to check. expected pixel index is adjusted
	 * @return true when ray can reach the detector
	 */
	bool TryDetection( Ray& ray ) const;


	private:

	CoordinateSystem* coordinate_system_;					/*!< local coordinate system*/
	vector<DetectorPixel> pixel_array_;						/*!< all pixel of detector*/
	vector<DetectorPixel> converted_pixel_array_;	/*!< pixel in different coordinate system*/

	DetectorProperties properties_;								/*!< properties*/

	/*!
	 * @brief get the index of the pixel which the ray will hit
	 * @param ray ray to check
	 * @return index when a pixel is hit. empty when no pixel is hit
	 */
	optional<size_t> GetHitPixelIndex( Ray& ray ) const;

};

