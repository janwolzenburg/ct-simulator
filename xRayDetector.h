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



/*********************************************************************
   Definitions
*********************************************************************/



/*!
 * @brief Class for Ray detector
*/
class XRayDetector{

	public:

	/*!
	 * @brief Constructor
	 * @param coordinate_system Coordinate system
	 * @param radon_properties Radon parameter
	 * @param physical_properties Parameter indipendent of radon parameter
	*/
	XRayDetector( CoordinateSystem* const coordinate_system, const ProjectionsProperties radon_properties, const PhysicalDetectorProperties physical_properties );

	/*!
	 * @brief Get all detector pixel
	 * @return Vector of pixels in one row
	*/
	vector<DetectorPixel> pixel_array( void ) const{ return pixel_array_; };
	
	/*!
	 * @brief Get the physical parameters of detector
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
	 * @brief Convert all comnverted pixel to this system
	 * @param target_coordinate_system Target
	*/
	void ConvertPixelArray( const CoordinateSystem* const target_coordinate_system );

	/*!
	 * @brief Reset all pixel
	*/
	void ResetDetectedRayPorperties( void );

	/*!
	 * @brief Detect Ray
	 * @param ray Ray to detect
	 * @param pixel_mutex Mutex for multi threaded access to pixel array
	 * @return True when ray hit the detector
	*/
	bool DetectRay( const Ray& ray, mutex& pixel_array_mutex );


	private:

	CoordinateSystem* coordinate_system_;			/*!<Local coordinate system*/
	vector<DetectorPixel> pixel_array_;				/*!<All pixel of detector*/
	vector<DetectorPixel> converted_pixel_array_;	/*!<Pixel in different coordinate system*/

	DetectorProperties properties_;					/*!<Properties*/

};

