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

//TODO: Add description
struct detectorParameter{

	size_t rows;
	size_t columns;

	double rowSize;
	double colSize;

	//double radius;

	bool structured;

};


class detector {

	public:
	// TODO: Change comments
	/*!
	 * @brief Constructor
	 * @param cSys_ Coordinate system
	 * @param rows_ Amount of pixel rows
	 * @param columns_ Amount of pixel columns
	 * @param pxSize_ Size of one pixel
	 * @param radius_ Radius of detector arc
	 * @param structured_ Add anti scatter structure
	*/
	detector(cartCSys* const cSys_, const double radius_, const detectorParameter parameter );

	/*!
	 * @brief Get all detector pixel
	 * @return Vector of pixels in one row
	*/
	vector<vector<pixel>> getPixel( void ) const;


	private:
	cartCSys* cSys;							/*!<Local coordinate system*/
	vector<vector<pixel>> allPixel;			/*!<Pixels of detector. Is a vector of pixels in one row*/

	size_t rows;							/*!<Amount of rows ( arc along z- axis )*/
	size_t columns;							/*!<Amount of columns ( pixel along one arc ) */

	v2RC pxSize;								/*!<Size of one pixel*/

	double radius;							/*!<Radius of arc*/
	//double angle;							/*!<Angle of arc*/
		
	bool structured;						/*!<Anti scatter structure*/

};

