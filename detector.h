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
 * @brief Class for ray detector
*/
class detector {

	public:

	/*!
	 * @brief Constructor
	 * @param cSys_ Coordinate system
	 * @param parameter Detector parameter
	*/
	detector(cartCSys* const cSys_, detectorRadonParameter& radonParameter, detectorIndipendentParameter& indipendentParameter );

	/*!
	 * @brief Get all detector pixel
	 * @return Vector of pixels in one row
	*/
	vector<pixel> getPixel( void ) const;

	/*!
	 * @brief Reset all pixel
	*/
	void reset( void );

	/*!
	 * @brief Detect ray
	 * @param r Ray to detect
	*/
	void detectRay( const ray r );

	/*!
	 * @brief Get the radon parameters of detector
	 * @return Radon parameters of detector
	*/
	detectorRadonParameter getSignalParameter( void ) const;

	/*!
	 * @brief Get the physical parameters of detector
	 * @return Physical parameters of detector
	*/
	detectorPhysicalParameter getPhysicalParameters( void ) const;

	cartCSys* CSys( void ) const;

	private:

	cartCSys* cSys;									/*!<Local coordinate system*/
	vector<pixel> allPixel;							/*!<Pixels of detector*/

	detectorPhysicalParameter physicalParameters;	/*!<Physical parameters*/		// Order is importanct since physicalParameters constructor changes radonParameters
	detectorRadonParameter radonParameters;			/*!<Radon parameters*/


};

