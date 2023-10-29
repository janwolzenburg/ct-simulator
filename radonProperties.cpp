/*********************************************************************
 * @file   radonProperties.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   October 2023
 *********************************************************************/



 /*********************************************************************
   Includes
*********************************************************************/

#include "generel.h"
#include "generelMath.h"
#include "simulation.h"
#include "serialization.h"
#include "radonProperties.h"



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
				distanceRange / (double) ( numberPoints.r - 1 ) },
	framesToFillSinogram( numberPoints.c - 1 + numberPoints.r - 1)
{

	// Check angle
	double currentAngle = (double) ( numberPoints.r - 1 ) * resolution.c;

	// Store current number_of_pixel of distances 
	size_t newNumberPointsRow = numberPoints.r;

	// Detector angle exceeds maximum or minimum
	if( currentAngle > max_detetector_arc_angle_rad ) newNumberPointsRow = (size_t) floor( max_detetector_arc_angle_rad / resolution.c ) + 1;
	if( currentAngle < min_detetector_arc_angle_rad ) newNumberPointsRow = (size_t) ceil( min_detetector_arc_angle_rad / resolution.c ) + 1;

	// Recalculate if number_of_pixel of point changed
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