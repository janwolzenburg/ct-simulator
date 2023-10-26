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


slicePlane::slicePlane( void ) :
	cSys( CSYS_TREE().addCSys( "Slice plane system" ) ),
	surface{ uvec3{ Tuple3D{ 1, 0, 0 }, cSys },
				uvec3{ Tuple3D{ 0, 1, 0 }, cSys },
				pnt3{  Tuple3D{0, 0, 0}, cSys } },
	rotationAngleX( 0. ),
	rotationAngleY( 0. ),
	positionZ( 0. ){
};

slicePlane::slicePlane( const vector<char>& binary_data, vector<char>::const_iterator& it ) :
	cSys{ CSYS_TREE().addCSys( binary_data, it ) },
	surface{ binary_data, it, cSys },
	rotationAngleX( DeSerializeBuildIn<double>( (double) 0., binary_data, it ) ),
	rotationAngleY( DeSerializeBuildIn<double>( (double) 0., binary_data, it ) ),
	positionZ( DeSerializeBuildIn<double>( (double) 0., binary_data, it ) )
{



}


size_t slicePlane::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;
	num_bytes += SerializeBuildIn( FILE_PREAMBLE, binary_data );
	num_bytes += cSys->Serialize( binary_data );
	num_bytes += surface.Serialize( binary_data );
	num_bytes += SerializeBuildIn( rotationAngleX, binary_data );
	num_bytes += SerializeBuildIn( rotationAngleY, binary_data );
	num_bytes += SerializeBuildIn( positionZ, binary_data );

	return num_bytes;

};