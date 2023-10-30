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
#include "radonTransformationProperties.h"



/*********************************************************************
  Implementations
*********************************************************************/


const string RadonTransformationProperties::FILE_PREAMBLE{ "Ver01RADONPARAMETER_FILE_PREAMBLE" };


/*!
 * radonProperties implementation
*/

RadonTransformationProperties::RadonTransformationProperties( const size_t number_of_angles, const size_t number_of_distances, const double measuring_field_size ) :
	number_of_angles_( ForceToMin( number_of_angles, (size_t) 2 ) ),
	number_of_distances_( ForceToMin( ForceOdd ( number_of_distances ), (size_t) 3 ) ),
	measuring_field_size_( ForcePositive( measuring_field_size ) ),
	angles_resolution_( PI / (double) ( number_of_angles_ ) ),
	distances_resolution_( measuring_field_size_ / (double) (number_of_distances_ - 1 ) ),
	number_of_frames_to_fill_( number_of_angles_ - 1 + number_of_distances_ - 1 )
{

	// Check angle
	double currentAngle = (double) ( number_of_distances_ - 1 ) * angles_resolution_;

	// Store current number_of_pixel of distances 
	size_t newNumberPointsRow = number_of_distances_;

	// Detector angle exceeds maximum or minimum
	if( currentAngle > max_detetector_arc_angle_rad ) newNumberPointsRow = (size_t) floor( max_detetector_arc_angle_rad / angles_resolution_ ) + 1;
	if( currentAngle < min_detetector_arc_angle_rad ) newNumberPointsRow = (size_t) ceil( min_detetector_arc_angle_rad / angles_resolution_ ) + 1;

	// Recalculate if number_of_pixel of point changed
	if( newNumberPointsRow != number_of_distances_ ){
		number_of_distances_ = ForceToMin( ForceOdd( newNumberPointsRow ), (size_t) 3 );
		distances_resolution_ = measuring_field_size_ / (double) ( number_of_distances_ - 1 );
		number_of_frames_to_fill_ = number_of_angles_ - 1 + number_of_distances_ - 1;
	}
}


RadonTransformationProperties::RadonTransformationProperties( const vector<char>& binary_data, vector<char>::const_iterator& it ) :
	number_of_angles_( DeSerializeBuildIn<size_t>( 3, binary_data, it ) ),
	number_of_distances_( DeSerializeBuildIn<size_t>( 2, binary_data, it ) ),
	measuring_field_size_( DeSerializeBuildIn<double>( 400., binary_data, it ) )
{
	*this = RadonTransformationProperties{ number_of_angles_, number_of_distances_, measuring_field_size_ };
}
	


size_t RadonTransformationProperties::Serialize( vector<char>& binary_data ) const{
	size_t num_bytes = 0;

	num_bytes += SerializeBuildIn( FILE_PREAMBLE, binary_data );
	num_bytes += SerializeBuildIn( number_of_angles_, binary_data );
	num_bytes += SerializeBuildIn( number_of_distances_, binary_data );
	num_bytes += SerializeBuildIn( measuring_field_size_, binary_data );

	return num_bytes;
}