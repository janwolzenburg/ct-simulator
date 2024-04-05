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
#include "tomography.h"



/*********************************************************************
  Implementations
*********************************************************************/

/*!
 * Projections implementation
*/


const string Projections::FILE_PREAMBLE{ "Ver03RADON_TRANSFORMED_FILE_PREAMBLE" };

Projections::Projections( void ) :
	DataGrid<>{}
{
	grid_errors_ = vector<vector<GridCoordinates>>( size().c, vector<GridCoordinates>( size().r, GridCoordinates{ INFINITY, INFINITY } ) );
}

Projections::Projections( const ProjectionsProperties properties, const TomographyProperties tomography_properties ) :
	DataGrid<>{		properties.GetTransformationSize(),
				GridCoordinates{ properties.angles_resolution() / 2.,
								 -( static_cast<double>( properties.number_of_distances() - 1 ) * properties.distances_resolution() ) / 2.},
				properties.GetTransformationResolution() },
	properties_( properties ),
	tomography_properties_( tomography_properties ),
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


TomographyProperties Projections::tomography_properties( void ) const{ 
	return tomography_properties_; 
};

size_t Projections::Serialize( vector<char>& binary_data ) const{
	size_t number_of_bytes = 0;

	number_of_bytes += DataGrid<>::Serialize( binary_data );
	number_of_bytes += properties_.Serialize( binary_data );
	number_of_bytes += tomography_properties_.Serialize( binary_data );
	number_of_bytes += SerializeBuildIn<vector<vector<GridCoordinates>>>( grid_errors_, binary_data );
	return number_of_bytes;
}


Projections::Projections( const vector<char>& binary_data, vector<char>::const_iterator& current_byte ) : 
	DataGrid<>{ binary_data, current_byte },
	properties_{ binary_data, current_byte },
	tomography_properties_{ binary_data, current_byte },
	grid_errors_( DeSerialize< vector<vector<GridCoordinates>> >( binary_data, current_byte ) )
{}