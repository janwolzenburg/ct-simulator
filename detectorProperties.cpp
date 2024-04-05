/*********************************************************************
 * @file   physical_detector_properties_.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/



 /*********************************************************************
   Includes
*********************************************************************/

#include "detectorProperties.h"
#include "projectionsProperties.h"


/*********************************************************************
  Implementations
*********************************************************************/


const string PhysicalDetectorProperties::FILE_PREAMBLE{ "PHYSICAL_DETECTOR_PROPERTIES_FILE_PREAMBLE" };

PhysicalDetectorProperties::PhysicalDetectorProperties( void ) :
	row_width( 1. ), detector_focus_distance( 1000. ), has_anti_scattering_structure( false ), 
	max_angle_allowed_by_structure( default_max_ray_angle_allowed_by_structure )
{}

PhysicalDetectorProperties::PhysicalDetectorProperties( const double row_width, const double detector_focus_distance, 
														const bool has_anti_scattering_structure, const double max_angle_allowed_by_structure ) :
	row_width( ForcePositive( row_width ) ), detector_focus_distance( ForcePositive( detector_focus_distance ) ),
	has_anti_scattering_structure( has_anti_scattering_structure ), max_angle_allowed_by_structure( ForceRange( max_angle_allowed_by_structure, 0., PI/2 ) )
{};

PhysicalDetectorProperties::PhysicalDetectorProperties( const vector<char>& binary_data, vector<char>::const_iterator& current_byte ) :
	row_width( DeSerializeBuildIn<double>( 1., binary_data, current_byte ) ),
	detector_focus_distance( DeSerializeBuildIn<double>( 1000., binary_data, current_byte ) ),
	has_anti_scattering_structure( DeSerializeBuildIn<bool>( false, binary_data, current_byte ) ),
	max_angle_allowed_by_structure( DeSerializeBuildIn<double>( default_max_ray_angle_allowed_by_structure, binary_data, current_byte ) )
{}

size_t PhysicalDetectorProperties::Serialize( vector<char>& binary_data ) const{

	size_t number_of_bytes = 0;
	number_of_bytes += SerializeBuildIn<double>( row_width, binary_data );
	number_of_bytes += SerializeBuildIn<double>( detector_focus_distance, binary_data );
	number_of_bytes += SerializeBuildIn<bool>( has_anti_scattering_structure, binary_data );
	number_of_bytes += SerializeBuildIn<double>( max_angle_allowed_by_structure, binary_data );

	return number_of_bytes;
}

/*!
 * DetectorProperties implementation
*/

DetectorProperties::DetectorProperties( const ProjectionsProperties projections_properties, const PhysicalDetectorProperties physical_properties ) :
	number_of_pixel{ projections_properties.number_of_distances(), 1},
	row_width( physical_properties.row_width ),
	rotation_angle( static_cast<double>( projections_properties.number_of_distances() - 1 ) * projections_properties.angles_resolution() ),
	detector_focus_distance( physical_properties.detector_focus_distance ),
	has_anti_scattering_structure( physical_properties.has_anti_scattering_structure ),
	max_angle_allowed_by_structure( physical_properties.max_angle_allowed_by_structure )
{}