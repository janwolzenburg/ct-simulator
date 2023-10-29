/*********************************************************************
 * @file   radonTransform.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/



 /*********************************************************************
   Includes
*********************************************************************/

#include "vectorAlgorithm.h"
#include "radonTransform.h"
#include "generelMath.h"
#include "serialization.h"



/*********************************************************************
  Implementations
*********************************************************************/

/*!
 * radonTransformed implementation
*/


const string radonTransformed::FILE_PREAMBLE{ "RADON_TRANSFORMED_FILE_PREAMBLE" };

radonTransformed::radonTransformed( void ) :
	grid<>()
{
	gridErrors = vector<vector<GridCoordinates>>( Size().c, vector<GridCoordinates>( Size().r, GridCoordinates{ INFINITY, INFINITY } ) );
}

radonTransformed::radonTransformed( const radonProperties detectorParameter ) :
	grid<>{		GridIndex{ detectorParameter.numberPoints.c - 1,
						detectorParameter.numberPoints.r }, 
				GridCoordinates{	0, 
						-( (double) ( detectorParameter.numberPoints.r - 1 ) * detectorParameter.resolution.r ) / 2. },
				detectorParameter.resolution,
				0 }
{
	gridErrors = vector<vector<GridCoordinates>>( Size().c, vector<GridCoordinates>( Size().r, GridCoordinates{ INFINITY, INFINITY }));
}

void radonTransformed::assignData( const radonPoint dataPoint ){

	GridCoordinates point{ dataPoint.coordinates.theta, dataPoint.coordinates.distance };
	GridIndex index = getIndex( point );
	GridCoordinates gridPoint{ getCoordinates( index )};

	GridCoordinates error{
		gridPoint.c - point.c,
		gridPoint.r - point.r
	};

	gridErrors.at( index.c ).at( index.r ) = error;
	this->setData( index, dataPoint.value );
}


size_t radonTransformed::Serialize( vector<char>& binary_data ) const{
	size_t num_bytes = 0;

	num_bytes += SerializeBuildIn( FILE_PREAMBLE, binary_data );
	num_bytes += grid<>::Serialize( binary_data );
	num_bytes += SerializeBuildIn( gridErrors, binary_data );
	return num_bytes;
}


radonTransformed::radonTransformed( const vector<char>& binary_data, vector<char>::const_iterator& it ) : 
	grid<>( binary_data, it ),
	gridErrors( DeSerialize< vector<vector<GridCoordinates>> >( binary_data, it ) )
{}