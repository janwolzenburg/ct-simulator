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
#include "simulation.h"
#include "serialization.h"



/*********************************************************************
  Implementations
*********************************************************************/

const string detectorRadonParameter::FILE_PREAMBLE{ "RADONPARAMETER_FILE_PREAMBLE" };


/*!
 * detectorRadonParameter implementation
*/

detectorRadonParameter::detectorRadonParameter( const GridIndex numberPoints_, const double distanceRange_ ) :
	distanceRange( Fpos( distanceRange_ ) ),
	numberPoints{	Fmin( numberPoints_.c, (size_t) 2 ),
					Fmin( FOdd ( numberPoints_.r ), (size_t) 3 ) },
	resolution{ PI / (double) ( numberPoints.c - 1 ),
				Fpos( distanceRange ) / (double) ( numberPoints.r - 1 ) },
	framesToFillSinogram( numberPoints.c - 1 + numberPoints.r - 1)
{

	// Check angle
	double currentAngle = (double) ( numberPoints.r - 1 ) * resolution.c;

	// Store current number of distances 
	size_t newNumberPointsRow = numberPoints.r;

	// Detector angle exceeds maximum or minimum
	if( currentAngle > MAX_DETECTOR_ANGLE ) newNumberPointsRow = (size_t) floor( MAX_DETECTOR_ANGLE / resolution.c ) + 1;
	if( currentAngle < MIN_DETECTOR_ANGLE ) newNumberPointsRow = (size_t) ceil( MIN_DETECTOR_ANGLE / resolution.c ) + 1;

	// Recalculate if number of point changed
	if( newNumberPointsRow != numberPoints.r ){
		numberPoints.r = Fmin( FOdd( newNumberPointsRow ), (size_t) 3 );
		resolution.r = distanceRange / (double) ( numberPoints.r - 1 );
		framesToFillSinogram = numberPoints.c - 1 + numberPoints.r - 1;
	}
}


detectorRadonParameter::detectorRadonParameter( const vector<char>& binary_data, vector<char>::const_iterator& it ) :
	distanceRange( DeSerializeBuildIn( 400., binary_data, it ) ),
	numberPoints( DeSerialize<GridIndex>( binary_data, it ) ),
	resolution( DeSerialize<GridCoordinates>( binary_data, it ) ),
	framesToFillSinogram( numberPoints.c - 1 + numberPoints.r - 1 ){
}


size_t detectorRadonParameter::Serialize( vector<char>& binary_data ) const{
	size_t num_bytes = 0;


	num_bytes += SerializeBuildIn( FILE_PREAMBLE, binary_data );
	num_bytes += SerializeBuildIn( distanceRange, binary_data );
	num_bytes += numberPoints.Serialize( binary_data );
	num_bytes += resolution.Serialize( binary_data );

	return num_bytes;
}


/*!
 * detectorIndipendentParameter implementation
*/


const string detectorIndipendentParameter::FILE_PREAMBLE{ "DETECTORPARAMETER_FILE_PREAMBLE" };

detectorIndipendentParameter::detectorIndipendentParameter( const size_t raysPerPixel_, const double arcRadius_, const double columnSize_, const bool structured_, const double maxRayAngleDetectable_ ) :
	raysPerPixel( raysPerPixel_ ),
	arcRadius( arcRadius_ ),
	columnSize( Fpos( columnSize_ ) ),
	structured( structured_ ),
	maxRayAngleDetectable( Fmax( Fmin( maxRayAngleDetectable_, 0. ), PI / 2. ) )
{}

detectorIndipendentParameter::detectorIndipendentParameter( const vector<char>& binary_data, vector<char>::const_iterator& it ) :

	raysPerPixel( DeSerializeBuildIn( (size_t) 1, binary_data, it ) ),
	arcRadius( DeSerializeBuildIn( 1000., binary_data, it ) ),
	columnSize( DeSerializeBuildIn( 50., binary_data, it ) ),
	structured( DeSerializeBuildIn( true, binary_data, it ) ),
	maxRayAngleDetectable( DeSerializeBuildIn( 5. / 360. * 2. * PI, binary_data, it ) ){

}

size_t detectorIndipendentParameter::Serialize( vector<char>& binary_data ) const{
	size_t num_bytes = 0;


	num_bytes += SerializeBuildIn( FILE_PREAMBLE, binary_data );
	num_bytes += SerializeBuildIn( raysPerPixel, binary_data );
	num_bytes += SerializeBuildIn( arcRadius, binary_data );
	num_bytes += SerializeBuildIn( columnSize, binary_data );
	num_bytes += SerializeBuildIn( structured, binary_data );
	num_bytes += SerializeBuildIn( maxRayAngleDetectable, binary_data );

	return num_bytes;
}


/*!
 * detectorPhysicalParameter implementation
*/

detectorPhysicalParameter::detectorPhysicalParameter( const detectorRadonParameter radonParameter, const detectorIndipendentParameter indipendentParameter ) :
	number{ radonParameter.numberPoints.r, 1 },
	angle( (double) ( radonParameter.numberPoints.r - 1 ) * radonParameter.resolution.c ),
	detectorFocusDistance( indipendentParameter.arcRadius ),
	structured( indipendentParameter.structured ),
	maxRayAngleDetectable( indipendentParameter.maxRayAngleDetectable )
{}