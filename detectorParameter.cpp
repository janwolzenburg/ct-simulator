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



const string detectorRadonParameter::FILE_PREAMBLE{ "RADONPARAMETER_FILE_PREAMBLE" };

const string detectorIndipendentParameter::FILE_PREAMBLE{ "DETECTORPARAMETER_FILE_PREAMBLE" };


/*!
 * detectorRadonParameter implementation
*/
detectorRadonParameter::detectorRadonParameter( const idx2CR numberPoints_, const double distanceRange_ ) :
	distanceRange( Fpos( distanceRange_ ) ),
	numberPoints{	Fmin( numberPoints_.col, (size_t) 2 ),
					Fmin( FOdd ( numberPoints_.row ), (size_t) 3 ) },
	resolution{ PI / (double) ( numberPoints.col - 1 ),
				Fpos( distanceRange ) / (double) ( numberPoints.row - 1 ) },
	framesToFillSinogram( numberPoints.col - 1 + numberPoints.row - 1)
{

	// Check angle
	double currentAngle = (double) ( numberPoints.row - 1 ) * resolution.col;

	// Store current number of distances 
	size_t newNumberPointsRow = numberPoints.row;

	// Detector angle exceeds maximum or minimum
	if( currentAngle > MAX_DETECTOR_ANGLE ) newNumberPointsRow = (size_t) floor( MAX_DETECTOR_ANGLE / resolution.col ) + 1;
	if( currentAngle < MIN_DETECTOR_ANGLE ) newNumberPointsRow = (size_t) ceil( MIN_DETECTOR_ANGLE / resolution.col ) + 1;

	// Recalculate if number of point changed
	if( newNumberPointsRow != numberPoints.row ){
		numberPoints.row = Fmin( FOdd( newNumberPointsRow ), (size_t) 3 );
		resolution.row = distanceRange / (double) ( numberPoints.row - 1 );
		framesToFillSinogram = numberPoints.col - 1 + numberPoints.row - 1;
	}


}

detectorRadonParameter::detectorRadonParameter( void ) :
	detectorRadonParameter{ idx2CR{ 32, 16 }, 500. }
{};


detectorRadonParameter::detectorRadonParameter( const vector<char>& binData, vector<char>::const_iterator& it ) :
	distanceRange( deSerializeBuildIn( 400., binData, it ) ),
	numberPoints( deSerialize<idx2CR>( binData, it ) ),
	resolution( deSerialize<v2CR>( binData, it ) ),
	framesToFillSinogram( numberPoints.col - 1 + numberPoints.row - 1 ){
}


size_t detectorRadonParameter::serialize( vector<char>& binData ) const{
	size_t numBytes = 0;


	numBytes += serializeBuildIn( FILE_PREAMBLE, binData );
	numBytes += serializeBuildIn( distanceRange, binData );
	numBytes += numberPoints.serialize( binData );
	numBytes += resolution.serialize( binData );

	return numBytes;
}


/*!
 * detectorIndipendentParameter implementation
*/

detectorIndipendentParameter::detectorIndipendentParameter( const size_t raysPerPixel_, const double arcRadius_, const double columnSize_, const bool structured_, const double maxRayAngleDetectable_ ) :
	raysPerPixel( raysPerPixel_ ),
	arcRadius( arcRadius_ ),
	columnSize( Fpos( columnSize_ ) ),
	structured( structured_ ),
	maxRayAngleDetectable( Fmax( Fmin( maxRayAngleDetectable_, 0. ), PI / 2. ) )
{}


detectorIndipendentParameter::detectorIndipendentParameter( void ) :
	detectorIndipendentParameter{ 1, 1000., 5., true, 5. / 360. * 2. * PI }
{}


detectorIndipendentParameter::detectorIndipendentParameter( const vector<char>& binData, vector<char>::const_iterator& it ) :

	raysPerPixel( deSerializeBuildIn( (size_t) 1, binData, it ) ),
	arcRadius( deSerializeBuildIn( 1000., binData, it ) ),
	columnSize( deSerializeBuildIn( 50., binData, it ) ),
	structured( deSerializeBuildIn( true, binData, it ) ),
	maxRayAngleDetectable( deSerializeBuildIn( 5. / 360. * 2. * PI, binData, it ) ){

}

size_t detectorIndipendentParameter::serialize( vector<char>& binData ) const{
	size_t numBytes = 0;


	numBytes += serializeBuildIn( FILE_PREAMBLE, binData );
	numBytes += serializeBuildIn( raysPerPixel, binData );
	numBytes += serializeBuildIn( arcRadius, binData );
	numBytes += serializeBuildIn( columnSize, binData );
	numBytes += serializeBuildIn( structured, binData );
	numBytes += serializeBuildIn( maxRayAngleDetectable, binData );

	return numBytes;
}



/*!
 * detectorPhysicalParameter implementation
*/

detectorPhysicalParameter::detectorPhysicalParameter( const detectorRadonParameter radonParameter, const detectorIndipendentParameter indipendentParameter ) :
	number{ radonParameter.numberPoints.row, 1 },
	angle( (double) ( radonParameter.numberPoints.row - 1 ) * radonParameter.resolution.col ),
	detectorFocusDistance( indipendentParameter.arcRadius ),
	structured( indipendentParameter.structured ),
	maxRayAngleDetectable( indipendentParameter.maxRayAngleDetectable )
{
	
}