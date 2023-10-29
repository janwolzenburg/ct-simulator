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

const string radonProperties::FILE_PREAMBLE{ "RADONPARAMETER_FILE_PREAMBLE" };


/*!
 * radonProperties implementation
*/

radonProperties::radonProperties( const GridIndex numberPoints_, const double distanceRange_ ) :
	distanceRange( ForcePositive( distanceRange_ ) ),
	numberPoints{	ForceToMin( numberPoints_.c, (size_t) 2 ),
					ForceToMin( ForceOdd ( numberPoints_.r ), (size_t) 3 ) },
	resolution{ PI / (double) ( numberPoints.c - 1 ),
				ForcePositive( distanceRange ) / (double) ( numberPoints.r - 1 ) },
	framesToFillSinogram( numberPoints.c - 1 + numberPoints.r - 1)
{

	// Check GetAngle
	double currentAngle = (double) ( numberPoints.r - 1 ) * resolution.c;

	// Store current number of distances 
	size_t newNumberPointsRow = numberPoints.r;

	// Detector GetAngle exceeds maximum or minimum
	if( currentAngle > max_detetector_arc_angle_rad ) newNumberPointsRow = (size_t) floor( max_detetector_arc_angle_rad / resolution.c ) + 1;
	if( currentAngle < min_detetector_arc_angle_rad ) newNumberPointsRow = (size_t) ceil( min_detetector_arc_angle_rad / resolution.c ) + 1;

	// Recalculate if number of point changed
	if( newNumberPointsRow != numberPoints.r ){
		numberPoints.r = ForceToMin( ForceOdd( newNumberPointsRow ), (size_t) 3 );
		resolution.r = distanceRange / (double) ( numberPoints.r - 1 );
		framesToFillSinogram = numberPoints.c - 1 + numberPoints.r - 1;
	}
}


radonProperties::radonProperties( const vector<char>& binary_data, vector<char>::const_iterator& it ) :
	distanceRange( DeSerializeBuildIn( 400., binary_data, it ) ),
	numberPoints( DeSerialize<GridIndex>( binary_data, it ) ),
	resolution( DeSerialize<GridCoordinates>( binary_data, it ) ),
	framesToFillSinogram( numberPoints.c - 1 + numberPoints.r - 1 ){
}


size_t radonProperties::Serialize( vector<char>& binary_data ) const{
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


const string detectorIndipendentParameter::FILE_PREAMBLE{ "Ver01DETECTORPARAMETER_FILE_PREAMBLE" };

detectorIndipendentParameter::detectorIndipendentParameter( const double arcRadius_, const double columnSize_, const bool structured_, const double maxRayAngleDetectable_ ) :
	arcRadius( arcRadius_ ),
	columnSize( ForcePositive( columnSize_ ) ),
	structured( structured_ ),
	maxRayAngleDetectable( ForceToMax( ForceToMin( maxRayAngleDetectable_, 0. ), PI / 2. ) )
{}

detectorIndipendentParameter::detectorIndipendentParameter( const vector<char>& binary_data, vector<char>::const_iterator& it ) :
	arcRadius( DeSerializeBuildIn( 1000., binary_data, it ) ),
	columnSize( DeSerializeBuildIn( 50., binary_data, it ) ),
	structured( DeSerializeBuildIn( true, binary_data, it ) ),
	maxRayAngleDetectable( DeSerializeBuildIn( 5. / 360. * 2. * PI, binary_data, it ) ){

}

size_t detectorIndipendentParameter::Serialize( vector<char>& binary_data ) const{
	size_t num_bytes = 0;


	num_bytes += SerializeBuildIn( FILE_PREAMBLE, binary_data );
	num_bytes += SerializeBuildIn( arcRadius, binary_data );
	num_bytes += SerializeBuildIn( columnSize, binary_data );
	num_bytes += SerializeBuildIn( structured, binary_data );
	num_bytes += SerializeBuildIn( maxRayAngleDetectable, binary_data );

	return num_bytes;
}


/*!
 * detectorPhysicalParameter implementation
*/

detectorPhysicalParameter::detectorPhysicalParameter( const radonProperties radonParameter, const detectorIndipendentParameter indipendentParameter ) :
	number{ radonParameter.numberPoints.r, 1 },
	angle( (double) ( radonParameter.numberPoints.r - 1 ) * radonParameter.resolution.c ),
	detectorFocusDistance( indipendentParameter.arcRadius ),
	structured( indipendentParameter.structured ),
	maxRayAngleDetectable( indipendentParameter.maxRayAngleDetectable )
{}