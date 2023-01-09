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



/*********************************************************************
   Definitions
*********************************************************************/

/*!
 * @brief Struct for detector parameters
*/
struct detectorParameter{

	size_t columns;		/*!<Amount of columns*/
			
	double rowSize;		/*!<Size of pixel in direction of row*/
	double colSize;		/*!<Size of pixel in direction of columns*/

	bool structured;	/*!<Flag for anti scatter structure*/

};



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
	detector(cartCSys* const cSys_, const double radius_, const detectorParameter parameter );

	/*!
	 * @brief Get all detector pixel
	 * @return Vector of pixels in one row
	*/
	vector<pixel> getPixel( void ) const;


	private:
	cartCSys* cSys;								/*!<Local coordinate system*/
	vector<pixel> allPixel;						/*!<Pixels of detector*/
	vector<vector<rayProperties>> detectedRays;	/*!<Properties of detected rays*/

	size_t columns;								/*!<Amount of columns ( pixel along one arc ) */

	v2RC pxSize;								/*!<Size of one pixel*/

	double radius;								/*!<Radius of arc*/
		
	bool structured;							/*!<Anti scatter structure*/

};

