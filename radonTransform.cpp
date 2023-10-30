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

radonTransformed::radonTransformed( const RadonTransformationProperties properties ) :
	grid<>{		properties.GetTransformationSize(),
				GridCoordinates{ 0., 
								 -( (double) ( properties.number_of_distances() - 1 ) * properties.distances_resolution() ) / 2.},
				properties.GetTransformationResolution()
		  },

	gridErrors( vector<vector<GridCoordinates>>( Size().c, vector<GridCoordinates>( Size().r, GridCoordinates{ INFINITY, INFINITY } ) ) )
{}

void radonTransformed::assignData( const RadonPoint dataPoint ){

	GridCoordinates point{ dataPoint.theta, dataPoint.distance };
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