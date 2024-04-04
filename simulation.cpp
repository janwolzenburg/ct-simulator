/*********************************************************************
 * @file   simulation.cpp
 * @brief  
 *
 * @author Jan Wolzenburg
 * @date   March 2024
 *********************************************************************/


 /*********************************************************************
  Includes
*********************************************************************/

#include "simulation.h"
#include "serialization.h"


/*********************************************************************
   Implementations
*********************************************************************/

SimulationProperties simulation_properties{ 20 };

const string SimulationProperties::FILE_PREAMBLE{ "SimulationProperties_FILE_PREAMBLE" };

SimulationProperties::SimulationProperties( const size_t simulation_quality ) :
quality( ForceRange( simulation_quality, 0ull, 99ull) ),
	ray_step_size_mm( ForceToMin( 1e-2 / ( 4. * static_cast<double>( quality + 1 ) ), 1e-3 ) ),
	number_of_points_in_spectrum( 8 + 3 * quality / 2),
	number_of_energies_for_scattering( 16 * ( quality + 1 ) ),
	number_of_scatter_angles( ForceOdd( 16 + 2 * quality ) ),
	bins_per_energy( 2 + 3 * static_cast<size_t>( static_cast<double>( quality ) ) / 2 )
{
}


size_t SimulationProperties::Serialize( vector<char>& binary_data ) const{
	size_t num_bytes = 0;

	num_bytes += SerializeBuildIn<double>( ray_step_size_mm, binary_data );
	num_bytes += SerializeBuildIn<size_t>( number_of_points_in_spectrum, binary_data );
	num_bytes += SerializeBuildIn<size_t>( number_of_energies_for_scattering, binary_data );
	num_bytes += SerializeBuildIn<size_t>( number_of_scatter_angles, binary_data );
	num_bytes += SerializeBuildIn<size_t>( bins_per_energy, binary_data );

	return num_bytes;
}


SimulationProperties::SimulationProperties( const vector<char>& binary_data, vector<char>::const_iterator& current_byte ) : 
	ray_step_size_mm( DeSerializeBuildIn<double>( 1e-3, binary_data, current_byte ) ),
	number_of_points_in_spectrum( DeSerializeBuildIn<size_t>( 32, binary_data, current_byte ) ),
	number_of_energies_for_scattering( DeSerializeBuildIn<size_t>( 128, binary_data, current_byte ) ),
	number_of_scatter_angles( DeSerializeBuildIn<size_t>( 63, binary_data, current_byte ) ),
	bins_per_energy( DeSerializeBuildIn<size_t>( 16, binary_data, current_byte ) )
{}