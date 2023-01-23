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
					Fmin( FOdd ( numberPoints_.row ), (size_t) 3 ) },
	distanceRange( Fpos( distanceRange_ ) ),
	resolution{ PI / (double) ( numberPoints.col - 1 ),
				distanceRange / (double) ( numberPoints.row - 1 ) },
	framesToFillSinogram( 3 * numberPoints.col - 3 )
{}

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

detectorIndipendentParameter::detectorIndipendentParameter( const double angle_, const double columnSize_, const bool structured_ ) :
	angle( ( angle_ >= 2. * PI * ( 40. / 360. ) && angle_ <= 2. * PI * ( 60. / 360. ) ) ? angle_ : 2. * PI * ( 50. / 360. )),
	columnSize( Fpos( columnSize_ ) ),
	structured( structured_ )
{}



/*!
 * detectorPhysicalParameter implementation
*/

detectorPhysicalParameter::detectorPhysicalParameter( detectorRadonParameter& radonParameter, detectorIndipendentParameter& indipendentParameter ) :
	structured( indipendentParameter.structured )
{
	number.col = radonParameter.numberPoints.row;		// Amount of pixel columns is equal to amount of points on distance (row) axis in sinogram
	number.row = 1;										// One detector row

	angle = ( (double) ( number.col - 1 ) * radonParameter.resolution.col );		// Angle between outer pixel normals


	int recalculate = 0;

	// Angle to small
	if( angle < 2. * PI * ( 40. / 360. ) ){
		angle = 2. * PI * ( 40. / 360. );
		recalculate = -1;
	}

	// Angle to big
	if( angle > 2. * PI * ( 60. / 360. ) ){
		angle = 2. * PI * ( 60. / 360. );
		recalculate = 1;
	}

	if( recalculate != 0 ){
		// New resolution
		radonParameter.resolution.col = angle / (double) ( number.col - 1 );	

		// New number of angles
		if( recalculate == -1 )
			radonParameter.numberPoints.col = (size_t) ceil( PI / radonParameter.resolution.col ) + 1;
		if( recalculate == 1 )
			radonParameter.numberPoints.col = (size_t) floor( PI / radonParameter.resolution.col ) + 1;

		// numberPonints.col is integer => recalculate resolution
		radonParameter.resolution.col = PI / (double) ( radonParameter.numberPoints.col - 1 );

		// Recalculate angle
		angle = ( (double) ( number.col - 1 ) * radonParameter.resolution.col );

	}

	indipendentParameter.angle = angle;

	detectorFocusDistance = radonParameter.distanceRange / ( sin( angle / 2 ) );	// Focus Detector distance

	pixelSize.col = indipendentParameter.columnSize;
	pixelSize.row = 2. * detectorFocusDistance * tan( angle / (double) ( number.col - 1 ) / 2. );	// Pixel size


}