/*********************************************************************
 * @file   detectorParameter.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/



 /*********************************************************************
   Includes
*********************************************************************/

#include "detectorParameter.h"


/*********************************************************************
  Implementations
*********************************************************************/



/*!
 * detectorRadonParameter implementation
*/

detectorRadonParameter::detectorRadonParameter( const idx2CR numberPoints_, const double maxAbsoluteDistance_ ) :
	numberPoints( numberPoints_ ),
	distanceRange( 2. * Fpos( maxAbsoluteDistance_ ) ),
	resolution{ v2CR{	PI / ( 2. * (double) ( numberPoints.c - 1 ) ),
						distanceRange / (double) ( numberPoints.r - 1 ) } },
	framesToFillSinogram( 3 * numberPoints.c - 3 )
{}

double detectorRadonParameter::getRadius( const double angle ) const{
	return distanceRange / 2. / ( 2. * sin( angle / 2 ) );
};

double detectorRadonParameter::getRowSize( const double radius ) const{
	return 4. * radius * tan( resolution.c / 2. );
};



/*!
 * detectorIndipendentParameter implementation
*/

detectorIndipendentParameter::detectorIndipendentParameter( const double angle_, const double columnSize_, const bool structured_ ) :
	angle( angle_ ),
	columnSize( columnSize_ ),
	structured( structured_ )
{}



/*!
 * detectorPhysicalParameter implementation
*/

detectorPhysicalParameter::detectorPhysicalParameter( const detectorRadonParameter radonParameter, const detectorIndipendentParameter indipendentParameter ) :
	number{ radonParameter.numberPoints.c, 1 },
	angle( indipendentParameter.angle ),
	radius( radonParameter.getRadius( angle ) ),
	pixelSize{ indipendentParameter.columnSize, radonParameter.getRowSize( radius ) },
	structured( indipendentParameter.structured )
{}