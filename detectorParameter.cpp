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
detectorRadonParameter::detectorRadonParameter( const idx2CR numberPoints_, const double distanceRange_ ) :
	numberPoints{	Fmin( numberPoints_.col, (size_t) 2 ),
					Fmin( FOdd ( numberPoints_.row ), (size_t) 3 ) },
	//distanceRange( Fpos( distanceRange_ ) ),
	resolution{ PI / (double) ( numberPoints.col - 1 ),
				Fpos( distanceRange_ ) / (double) ( numberPoints.row - 1 ) },
	framesToFillSinogram( numberPoints.col - 1 + numberPoints.row - 1)
{

	// Check angle
	double currentAngle = (double) ( numberPoints.row - 1 ) * resolution.col;

	// Store current number of distances 
	size_t newNumberPointsRow = numberPoints.row;

	// Detector angle exceeds maximum or minimum
	if( currentAngle > MAX_DETECTOR_ANGLE ) newNumberPointsRow = (size_t) floor( MAX_DETECTOR_ANGLE / resolution.col ) + 1;
	if( currentAngle < MIN_DETECTOR_ANGLE ) newNumberPointsRow = (size_t) ceil( MIN_DETECTOR_ANGLE / resolution.col ) + 1;
	//newNumberPointsRow = FOdd( newNumberPointsRow );

	// Recalculate if number of point changed
	if( newNumberPointsRow != numberPoints.row ){
		numberPoints.row = Fmin( FOdd( newNumberPointsRow ), (size_t) 3 );
		resolution.row = Fpos( distanceRange_ ) / (double) ( numberPoints.row - 1 );
		framesToFillSinogram = numberPoints.col - 1 + numberPoints.row - 1;
	}


}



/*!
 * detectorIndipendentParameter implementation
*/

detectorIndipendentParameter::detectorIndipendentParameter( const double arcRadius_, const double columnSize_, const bool structured_ ) :
	arcRadius( arcRadius_ ),
	columnSize( Fpos( columnSize_ ) ),
	structured( structured_ )
{}



/*!
 * detectorPhysicalParameter implementation
*/

detectorPhysicalParameter::detectorPhysicalParameter( const detectorRadonParameter radonParameter, const detectorIndipendentParameter indipendentParameter ) :
	number{ radonParameter.numberPoints.row, 1 },
	angle( (double) ( radonParameter.numberPoints.row - 1 ) * radonParameter.resolution.col ),
	detectorFocusDistance( indipendentParameter.arcRadius ),
	structured( indipendentParameter.structured )
{
	
}