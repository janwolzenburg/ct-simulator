/******************************************************************
* @file   slicePlane.cpp
* @brief  Implementations
*
* @author Jan Wolzenburg
* @date   April 2023
* ********************************************************************/



/*********************************************************************
  Includes
*********************************************************************/

#include "slicePlane.h"
#include "serialization.h"


/*********************************************************************
  Implementations
*********************************************************************/


SlicePlane::SlicePlane( void ) :
	coordinate_system( CoordinateSystems().AddSystem( "Slice plane system" ) ),
	surface{ UnitVector3D{ Tuple3D{ 1, 0, 0 }, coordinate_system },
				UnitVector3D{ Tuple3D{ 0, 1, 0 }, coordinate_system },
				Point3D{  Tuple3D{0, 0, 0}, coordinate_system } },
	rotation_angle_x( 0. ),
	rotation_angle_y( 0. ),
	position_z( 0. ){
}

SlicePlane::SlicePlane( const vector<char>& binary_data, vector<char>::const_iterator& it ) :
	coordinate_system{ CoordinateSystems().AddSystem( binary_data, it ) },
	surface{ binary_data, it, coordinate_system },
	rotation_angle_x( DeSerializeBuildIn<double>( 0., binary_data, it ) ),
	rotation_angle_y( DeSerializeBuildIn<double>( 0., binary_data, it ) ),
	position_z( DeSerializeBuildIn<double>( 0., binary_data, it ) )
{}

size_t SlicePlane::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;
	num_bytes += coordinate_system->Serialize( binary_data );
	num_bytes += surface.Serialize( binary_data );
	num_bytes += SerializeBuildIn<double>( rotation_angle_x, binary_data );
	num_bytes += SerializeBuildIn<double>( rotation_angle_y, binary_data );
	num_bytes += SerializeBuildIn<double>( position_z, binary_data );

	return num_bytes;

}