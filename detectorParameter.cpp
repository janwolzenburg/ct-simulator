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

/*!
 * detectorRadonParameter implementation
*/

detectorRadonParameter::detectorRadonParameter( const idx2CR numberPoints_, const double distanceRange_ ) :
	numberPoints{	Fmin( numberPoints_.col, (size_t) 2 ),
					Fmin( numberPoints_.row, (size_t) 2 ) },
	distanceRange( Fpos( distanceRange_ ) ),
	resolution{ PI / (double) ( numberPoints.col - 1 ),
				distanceRange / (double) ( numberPoints.row - 1 ) },
	framesToFillSinogram( 3 * numberPoints.col - 3 )
{}

double detectorRadonParameter::getDetectorFocusDistance( const double angle ) const{
	return distanceRange / ( sin( angle / 2 ) );
};

//double detectorRadonParameter::getRowSize( const double detectorFocusDistance ) const{
	//return 2. * detectorFocusDistance * tan( resolution.col / 2. );
//};



/*!
 * detectorIndipendentParameter implementation
*/

detectorIndipendentParameter::detectorIndipendentParameter( const double angle_, const double columnSize_, const bool structured_ ) :
	angle( ( angle_ >= 2. * PI * ( 40. / 360. ) && angle_ <= 2. * PI * ( 60. / 360. ) ) ? angle_ : 2. * PI * ( 50. / 360. )),
	columnSize( Fpos( columnSize_ ) ),
	structured( structured_ )
{}



/*!
 * detectorPhysicalParameter implementation
*/

detectorPhysicalParameter::detectorPhysicalParameter( const detectorRadonParameter radonParameter, const detectorIndipendentParameter indipendentParameter ) :
	number{ Fmin( (size_t) ( indipendentParameter.angle / radonParameter.resolution.col ), (size_t) 2 ), 1 },
	angle( (double) ( number.col - 1 ) * radonParameter.resolution.col ),
	detectorFocusDistance( radonParameter.getDetectorFocusDistance( angle ) ),
	pixelSize{ indipendentParameter.columnSize, 2. * detectorFocusDistance * tan( angle / (double) ( number.col - 1 ) / 2. ) },
	structured( indipendentParameter.structured )
{}