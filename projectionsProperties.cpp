/*********************************************************************
 * @file   radonProperties.cpp
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
#include "projectionsProperties.h"



/*********************************************************************
  Implementations
*********************************************************************/


const string ProjectionsProperties::FILE_PREAMBLE{ "Ver03RADONPARAMETER_FILE_PREAMBLE" };


/*!
 * radonProperties implementation
*/

ProjectionsProperties::ProjectionsProperties(	const size_t number_of_projections, const size_t number_of_distances,
												const double measuring_field_size ) :
	number_of_projections_( ForceToMin( number_of_projections, static_cast<size_t>( 4 ) ) ),
	number_of_distances_( ForceToMin( ForceEven( number_of_distances ), static_cast<size_t>( 2 ) ) ),
	measuring_field_size_( ForcePositive( measuring_field_size ) ),
	angles_resolution_( PI / static_cast<double>( number_of_projections_ ) ),
	distances_resolution_( measuring_field_size_ / static_cast<double>( number_of_distances_ - 1 ) ),
	number_of_frames_to_fill_( number_of_projections_ + number_of_distances_ - 1 )
{

	// check angle
	const double current_angle = static_cast<double>( number_of_distances_ - 1 ) * angles_resolution_;

	// store current number_of_pixel of distances 
	size_t new_number_of_distances = number_of_distances_;

	// detector angle exceeds maximum or minimum
	if( current_angle > max_detetector_arc_angle_rad ) new_number_of_distances = static_cast<size_t>( floor( max_detetector_arc_angle_rad / angles_resolution_ ) ) + 1;
	if( current_angle < min_detetector_arc_angle_rad ) new_number_of_distances = static_cast<size_t>( ceil( min_detetector_arc_angle_rad / angles_resolution_ ) ) + 1;

	// recalculate if number_of_pixel of point changed
	if( new_number_of_distances != number_of_distances_ ){
		number_of_distances_ = ForceToMin( ForceEven( new_number_of_distances ), static_cast<size_t>( 3 ) );
		distances_resolution_ = measuring_field_size_ / static_cast<double>( number_of_distances_ - 1 );
		number_of_frames_to_fill_ = number_of_projections_ + number_of_distances_ - 1;
	}
}


ProjectionsProperties::ProjectionsProperties( const vector<char>& binary_data, vector<char>::const_iterator& current_byte ) :
	number_of_projections_( DeSerializeBuildIn<size_t>( 5, binary_data, current_byte ) ),
	number_of_distances_( DeSerializeBuildIn<size_t>( 4, binary_data, current_byte ) ),
	measuring_field_size_( DeSerializeBuildIn<double>( 400., binary_data, current_byte ) )
{
	*this = ProjectionsProperties{ number_of_projections_, number_of_distances_, measuring_field_size_ };
}
	


size_t ProjectionsProperties::Serialize( vector<char>& binary_data ) const{
	size_t number_of_bytes = 0;

	number_of_bytes += SerializeBuildIn<size_t>( number_of_projections_, binary_data );
	number_of_bytes += SerializeBuildIn<size_t>( number_of_distances_, binary_data );
	number_of_bytes += SerializeBuildIn<double>( measuring_field_size_, binary_data );

	return number_of_bytes;
}