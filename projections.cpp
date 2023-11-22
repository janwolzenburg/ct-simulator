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
#include "projections.h"
#include "generelMath.h"
#include "serialization.h"



/*********************************************************************
  Implementations
*********************************************************************/

/*!
 * Projections implementation
*/


const string Projections::FILE_PREAMBLE{ "Ver01RADON_TRANSFORMED_FILE_PREAMBLE" };

Projections::Projections( void ) :
	DataGrid<>{}
{
	grid_errors_ = vector<vector<GridCoordinates>>( size().c, vector<GridCoordinates>( size().r, GridCoordinates{ INFINITY, INFINITY } ) );
}

Projections::Projections( const ProjectionsProperties properties ) :
	DataGrid<>{		properties.GetTransformationSize(),
				GridCoordinates{ properties.angles_resolution() / 2.,
								 -( static_cast<double>( properties.number_of_distances() - 1 ) * properties.distances_resolution() ) / 2.},
				properties.GetTransformationResolution() },
	properties_( properties ),

	grid_errors_( vector<vector<GridCoordinates>>( size().c, vector<GridCoordinates>( size().r, GridCoordinates{ INFINITY, INFINITY } ) ) )
{}

void Projections::AssignData( const RadonPoint dataPoint ){

	GridCoordinates point{ dataPoint.theta, dataPoint.distance };
	GridIndex index = GetIndex( point );
	GridCoordinates gridPoint{ GetCoordinates( index )};

	GridCoordinates error{
		gridPoint.c - point.c,
		gridPoint.r - point.r
	};

	grid_errors_.at( index.c ).at( index.r ) = error;
	this->SetData( index, dataPoint.value );
}


size_t Projections::Serialize( vector<char>& binary_data ) const{
	size_t num_bytes = 0;

	num_bytes += DataGrid<>::Serialize( binary_data );
	num_bytes += DataGrid<>::Serialize( binary_data );
	num_bytes += SerializeBuildIn<vector<vector<GridCoordinates>>>( grid_errors_, binary_data );
	return num_bytes;
}


Projections::Projections( const vector<char>& binary_data, vector<char>::const_iterator& it ) : 
	DataGrid<>{ binary_data, it },
	properties_{ binary_data, it },
	grid_errors_( DeSerialize< vector<vector<GridCoordinates>> >( binary_data, it ) )
{}