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
#include "radonTransformation.h"
#include "generelMath.h"
#include "serialization.h"



/*********************************************************************
  Implementations
*********************************************************************/

/*!
 * RadonTransformation implementation
*/


const string RadonTransformation::FILE_PREAMBLE{ "RADON_TRANSFORMED_FILE_PREAMBLE" };

RadonTransformation::RadonTransformation( void ) :
	grid<>()
{
	grid_errors_ = vector<vector<GridCoordinates>>( Size().c, vector<GridCoordinates>( Size().r, GridCoordinates{ INFINITY, INFINITY } ) );
}

RadonTransformation::RadonTransformation( const RadonTransformationProperties properties ) :
	grid<>{		properties.GetTransformationSize(),
				GridCoordinates{ 0., 
								 -( (double) ( properties.number_of_distances() - 1 ) * properties.distances_resolution() ) / 2.},
				properties.GetTransformationResolution()
		  },

	grid_errors_( vector<vector<GridCoordinates>>( Size().c, vector<GridCoordinates>( Size().r, GridCoordinates{ INFINITY, INFINITY } ) ) )
{}

void RadonTransformation::assignData( const RadonPoint dataPoint ){

	GridCoordinates point{ dataPoint.theta, dataPoint.distance };
	GridIndex index = getIndex( point );
	GridCoordinates gridPoint{ getCoordinates( index )};

	GridCoordinates error{
		gridPoint.c - point.c,
		gridPoint.r - point.r
	};

	grid_errors_.at( index.c ).at( index.r ) = error;
	this->setData( index, dataPoint.value );
}


size_t RadonTransformation::Serialize( vector<char>& binary_data ) const{
	size_t num_bytes = 0;

	num_bytes += SerializeBuildIn( FILE_PREAMBLE, binary_data );
	num_bytes += grid<>::Serialize( binary_data );
	num_bytes += SerializeBuildIn( grid_errors_, binary_data );
	return num_bytes;
}


RadonTransformation::RadonTransformation( const vector<char>& binary_data, vector<char>::const_iterator& it ) : 
	grid<>{ binary_data, it },
	grid_errors_( DeSerialize< vector<vector<GridCoordinates>> >( binary_data, it ) )
{}