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
#include "detectorParameter.h"



/*********************************************************************
   Definitions
*********************************************************************/



/*!
 * @brief Class for Ray detector
*/
class detector {

	public:

	/*!
	 * @brief Cnstructor
	 * @param coordinate_system Coordinate system
	 * @param radonParameter Radon parameter
	 * @param physical_properties Parameter indipendent of radon parameter
	*/
	detector(CoordinateSystem* const coordinate_system, const radonProperties radonParameter, const PhysicalDetectorProperties physical_properties );

	/*!
	 * @brief Get all detector pixel
	 * @return Vector of pixels in one row
	*/
	vector<pixel> getPixel( void ) const{ return allPixel; };

	/*!
	 * @brief Reset all pixel
	*/
	void reset( void );

	/*!
	 * @brief Detect Ray
	 * @param direction_ Ray to detect
	*/
	void detectRay( const Ray r, mutex& allPixelLock );

	/*!
	 * @brief Get the radon parameters of detector
	 * @return Radon parameters of detector
	*/
	//radonProperties getSignalParameter( void ) const{ return radonParameters; };

	/*!
	 * @brief Get the physical parameters of detector
	 * @return Physical parameters of detector
	*/
	DetectorProperties properties( void ) const{ return properties_; };

	/*!
	 * @brief Get coordinate system of detector
	 * @return Pointer to coordinate system
	*/
	//CoordinateSystem* GetCoordinateSystem( void ) const{ return coordinate_system_; };
	

	/*!
	 * @brief Convert all comnverted pixel to this system
	 * @param targetCSys Target
	*/
	void convertPixel( const CoordinateSystem* const targetCSys );


	private:

	CoordinateSystem* cSys;									/*!<Local coordinate system*/
	vector<pixel> allPixel;							/*!<Pixels of detector*/
	vector<pixel> allPixelConverted;				/*!<Pixel in different coordinate system*/

	DetectorProperties properties_;	/*!<Physical parameters*/		// Order is important since properties_ constructor changes radonParameters
	//radonProperties radonParameters;			/*!<Radon parameters*/

};

