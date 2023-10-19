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
 #include <mutex>
 using std::mutex;

#include "generelMath.h"
#include "detectorPixel.h"
#include "detectorParameter.h"



/*********************************************************************
   Definitions
*********************************************************************/



/*!
 * @brief Class for ray detector
*/
class detector {

	public:

	/*!
	 * @brief Cnstructor
	 * @param cSys_ Coordinate system
	 * @param radonParameter Radon parameter
	 * @param indipendentParameter Parameter indipendent of radon parameter
	*/
	detector(cartCSys* const cSys_, const detectorRadonParameter radonParameter, const detectorIndipendentParameter indipendentParameter );

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
	 * @brief Detect ray
	 * @param r Ray to detect
	*/
	void detectRay( const ray r, mutex& allPixelLock );

	/*!
	 * @brief Get the radon parameters of detector
	 * @return Radon parameters of detector
	*/
	detectorRadonParameter getSignalParameter( void ) const{ return radonParameters; };

	/*!
	 * @brief Get the physical parameters of detector
	 * @return Physical parameters of detector
	*/
	detectorPhysicalParameter getPhysicalParameters( void ) const{ return physicalParameters; };

	/*!
	 * @brief Get coordinate system of detector
	 * @return Pointer to coordinate system
	*/
	cartCSys* CSys( void ) const{ return cSys; };
	

	/*!
	 * @brief Convert all comnverted pixel to this system
	 * @param targetCSys Target
	*/
	void convertPixel( const cartCSys* const targetCSys );


	private:

	cartCSys* cSys;									/*!<Local coordinate system*/
	vector<pixel> allPixel;							/*!<Pixels of detector*/
	vector<pixel> allPixelConverted;				/*!<Pixel in different coordinate system*/

	detectorPhysicalParameter physicalParameters;	/*!<Physical parameters*/		// Order is important since physicalParameters constructor changes radonParameters
	detectorRadonParameter radonParameters;			/*!<Radon parameters*/

};

