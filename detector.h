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

class detectorRadonParameter{

	public:
	
	detectorRadonParameter( const idx2CR numberPoints_, const double maxAbsoluteDistance_ ) :
		numberPoints(  numberPoints_ ),
		distanceRange( 2. * Fpos( maxAbsoluteDistance_ ) ),
		resolution{ v2CR{	PI / ( 2. * (double) ( numberPoints.c - 1 ) ), 
							distanceRange / (double) ( numberPoints.r - 1 ) }},
		framesToFillSinogram( 3 * numberPoints.c - 3 )
	{};

	public:

	idx2CR numberPoints;
	double distanceRange;
	v2CR resolution;
	//double deltaTheta;
	//double deltaDistance;
	
	size_t framesToFillSinogram;
};

/*!
 * @brief Struct for detector parameters
*/
class detectorParameterPhysical{

	public:
	detectorParameterPhysical( const detectorRadonParameter radonParameter, size_t numRows_, const double radius_, const double colSize_, const bool structured_ ) :
		numberColumns( radonParameter.numberPoints.r ),
		numberRows( numRows_ ),
		radius( radius_ ),
		angle( 2. *  asin( radonParameter.distanceRange / ( 4. * radius ) )),
		rowSize( 4. * radius * tan( radonParameter.resolution.c / 2. ) ),
		colSize( colSize_ ),
		structured( structured_ )
	{};

	public:

	size_t numberColumns;		/*!<Amount of columns*/
	size_t numberRows;

	double radius;
	double angle;
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
	detector(cartCSys* const cSys_, const detectorRadonParameter parameter, size_t numRows_, const double radius_, const double columnSize, const bool structered );

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

	detectorRadonParameter getSignalParameter( const cartCSys* const cSys ) const;
	//range getDistanceRange( void ) const;

	//v2CR getResolution( void ) const;

	private:
	cartCSys* cSys;								/*!<Local coordinate system*/
	vector<pixel> allPixel;						/*!<Pixels of detector*/

	//size_t columns;								/*!<Amount of columns ( pixel along one arc ) */

	//v2CR pxSize;								/*!<Size of one pixel*/

	//double radius;								/*!<Radius of arc*/
	
	detectorParameterPhysical physicalParameters;
	detectorRadonParameter radonParameters;

	//bool structured;							/*!<Anti scatter structure*/

};

