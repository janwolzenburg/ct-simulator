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

//  TODO: Add detailed comments about calculations
// CleanUp

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
	double currentAngle = (double) ( numberPoints.row - 1 ) * resolution.row;

	// Store current number of angles 
	size_t newNumberPointsCol = numberPoints.col;

	// Angle exceeds maximum or minimum
	if( currentAngle > MAX_DETECTOR_ANGLE ) newNumberPointsCol = (size_t) MAX_DETECTOR_ANGLE / resolution.row + 1.;
	if( currentAngle < MIN_DETECTOR_ANGLE ) newNumberPointsCol = (size_t) MIN_DETECTOR_ANGLE / resolution.row + 1.;

	// Recalculate if number of point changed
	if( newNumberPointsCol != numberPoints.col ){
		numberPoints.col = Fmin( FOdd( newNumberPointsCol ), (size_t) 2 );
		resolution.col = PI / (double) ( numberPoints.col - 1 );
		framesToFillSinogram = numberPoints.col - 1 + numberPoints.row - 1;
	}


}

/*
double detectorRadonParameter::getDetectorFocusDistance( const double angle ) const{
	return distanceRange / ( sin( angle / 2 ) );
};*/

//double detectorRadonParameter::getRowSize( const double detectorFocusDistance ) const{
	//return 2. * detectorFocusDistance * tan( resolution.col / 2. );
//};



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
	number{ radonParameter.numberPoints.col, 1 },
	angle( (double) ( radonParameter.numberPoints.row - 1 )* radonParameter.resolution.row ),
	detectorFocusDistance( indipendentParameter.arcRadius ),
	structured( indipendentParameter.structured )
{
	
}