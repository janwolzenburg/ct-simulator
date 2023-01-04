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


class detector {

	public:

	/*!
	 * @brief Constructor
	 * @param cSys_ Coordinate system
	 * @param rows_ Amount of pixel rows
	 * @param columns_ Amount of pixel columns
	 * @param pxSize_ Size of one pixel
	 * @param radius_ Radius of detector arc
	 * @param structured_ Add anti scatter structure
	*/
	detector(cartCSys* const cSys_, const size_t rows_, const size_t columns_, const t2 pxSize_, const double radius_, const bool structured_);

	vector<vector<pixel>> getPixel( void ) const;


	private:
	cartCSys* cSys;							/*!<Local coordinate system*/
	vector<vector<pixel>> allPixel;			/*!<Pixels of detector*/

	size_t rows;							/*!<Amount of rows ( arc along z- axis )*/
	size_t columns;							/*!<Amount of columns ( pixel along one arc ) */

	t2 pxSize;								/*!<Size of one pixel*/

	double radius;							/*!<Radius of arc*/
	//double angle;							/*!<Angle of arc*/
		
	bool structured;						/*!<Anti scatter structure*/

};

