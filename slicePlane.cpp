/******************************************************************
* @file   slicePlane.cpp
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
	coordinate_system( GetCoordinateSystemTree().AddSystem( "Slice plane system" ) ),
	surface{ Unitvector3D{ Tuple3D{ 1, 0, 0 }, coordinate_system },
				Unitvector3D{ Tuple3D{ 0, 1, 0 }, coordinate_system },
				Point3D{  Tuple3D{0, 0, 0}, coordinate_system } },
	rotation_angle_x( 0. ),
	rotation_angle_y( 0. ),
	position_z( 0. ){
}

SlicePlane::SlicePlane( const vector<char>& binary_data, vector<char>::const_iterator& current_byte ) :
	coordinate_system{ GetCoordinateSystemTree().AddSystem( binary_data, current_byte ) },
	surface{ binary_data, current_byte, coordinate_system },
	rotation_angle_x( DeSerializeBuildIn<double>( 0., binary_data, current_byte ) ),
	rotation_angle_y( DeSerializeBuildIn<double>( 0., binary_data, current_byte ) ),
	position_z( DeSerializeBuildIn<double>( 0., binary_data, current_byte ) )
{}

size_t SlicePlane::Serialize( vector<char>& binary_data ) const{

	size_t number_of_bytes = 0;
	number_of_bytes += coordinate_system->Serialize( binary_data );
	number_of_bytes += surface.Serialize( binary_data );
	number_of_bytes += SerializeBuildIn<double>( rotation_angle_x, binary_data );
	number_of_bytes += SerializeBuildIn<double>( rotation_angle_y, binary_data );
	number_of_bytes += SerializeBuildIn<double>( position_z, binary_data );

	return number_of_bytes;

}