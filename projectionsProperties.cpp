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
#include "projectionsProperties.h"



/*********************************************************************
  Implementations
*********************************************************************/


const string ProjectionsProperties::FILE_PREAMBLE{ "Ver03RADONPARAMETER_FILE_PREAMBLE" };


/*!
 * radonProperties implementation
*/

ProjectionsProperties::ProjectionsProperties(	const size_t number_of_angles, const size_t number_of_distances, 
												const double measuring_field_size, const string name ) :
	number_of_projections_( ForceToMin( number_of_angles, static_cast<size_t>( 4 ) ) ),
	number_of_distances_( ForceToMin( ForceEven( number_of_distances ), static_cast<size_t>( 2 ) ) ),
	measuring_field_size_( ForcePositive( measuring_field_size ) ),
	angles_resolution_( PI / static_cast<double>( number_of_projections_ ) ),
	distances_resolution_( measuring_field_size_ / static_cast<double>( number_of_distances_ - 1 ) ),
	number_of_frames_to_fill_( number_of_projections_ + number_of_distances_ - 1 ),
	tube_mean_energy_( reference_energy_for_mu_eV ),
	name_( name )
{

	// Check angle
	double currentAngle = static_cast<double>( number_of_distances_ - 1 ) * angles_resolution_;

	// Store current number_of_pixel of distances 
	size_t newNumberPointsRow = number_of_distances_;

	// Detector angle exceeds maximum or minimum
	if( currentAngle > max_detetector_arc_angle_rad ) newNumberPointsRow = static_cast<size_t>( floor( max_detetector_arc_angle_rad / angles_resolution_ ) ) + 1;
	if( currentAngle < min_detetector_arc_angle_rad ) newNumberPointsRow = static_cast<size_t>( ceil( min_detetector_arc_angle_rad / angles_resolution_ ) ) + 1;

	// Recalculate if number_of_pixel of point changed
	if( newNumberPointsRow != number_of_distances_ ){
		number_of_distances_ = ForceToMin( ForceEven( newNumberPointsRow ), static_cast<size_t>( 3 ) );
		distances_resolution_ = measuring_field_size_ / static_cast<double>( number_of_distances_ - 1 );
		number_of_frames_to_fill_ = number_of_projections_ + number_of_distances_ - 1;
	}
}


ProjectionsProperties::ProjectionsProperties( const vector<char>& binary_data, vector<char>::const_iterator& it ) :
	number_of_projections_( DeSerializeBuildIn<size_t>( 5, binary_data, it ) ),
	number_of_distances_( DeSerializeBuildIn<size_t>( 4, binary_data, it ) ),
	measuring_field_size_( DeSerializeBuildIn<double>( 400., binary_data, it ) ),
	tube_mean_energy_( DeSerializeBuildIn<double>( reference_energy_for_mu_eV, binary_data, it ) ),
	name_( DeSerializeBuildIn<string>( "Unnamed", binary_data, it ) )
{
	*this = ProjectionsProperties{ number_of_projections_, number_of_distances_, measuring_field_size_, name_ };
}
	


size_t ProjectionsProperties::Serialize( vector<char>& binary_data ) const{
	size_t num_bytes = 0;

	num_bytes += SerializeBuildIn<size_t>( number_of_projections_, binary_data );
	num_bytes += SerializeBuildIn<size_t>( number_of_distances_, binary_data );
	num_bytes += SerializeBuildIn<double>( measuring_field_size_, binary_data );
	num_bytes += SerializeBuildIn<double>( tube_mean_energy_, binary_data );
	num_bytes += SerializeBuildIn<string>( name_, binary_data );

	return num_bytes;
}