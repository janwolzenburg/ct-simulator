/*********************************************************************
 * @file   rays.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


/*********************************************************************
	Includes
 *********************************************************************/
#include "rays.h"


/*********************************************************************
   Implementations
*********************************************************************/


/*
	rayProperties implementation
*/

void rayProperties::attenuateSpectrum( const voxData& voxelData, const double distance ){

	energySpectrum.modify(  [ & ] ( Tuple2D& spectrumPoint ) -> void {
		
		const double k = voxelData.attenuationAt( spectrumPoint.x );
		spectrumPoint.y *= exp( -k * distance );

	} );
}


/*
	ray implementation
*/

ray ray::convertTo( const CoordinateSystem* const target ) const{
	return ray{ this->line::convertTo( target ), properties };
};

double ray::getPara( const Point3D p, bool* const success ) const{
	double t = line::getPara( p, success );
	*success = *success && ( t >= 0 );
	return t;
}

ray ray::projectOnXYPlane( const CoordinateSystem* const cSys ) const{
	return ray{ this->line::projectOnXYPlane( cSys ), this->properties };
}

void ray::updateProperties( const voxData& data, const double distance ){
	properties.attenuateSpectrum( data, distance );
}

vector<FACE_ID> ray::getPossibleVoxelExits( void ) const{

	vector<FACE_ID> possibleFaces;

	// Iterate all faces of voxel
	for( FACE_ID currentFace = FACE_ID::BEGIN; currentFace < FACE_ID::END; ++currentFace ){

		// Check if face can be an exit face of the tRay
		switch( currentFace ){
			case FACE_ID::YZ_Xp:
				if( r.X() > 0 ) possibleFaces.push_back( currentFace ); break;
			case FACE_ID::YZ_Xm:
				if( r.X() < 0 ) possibleFaces.push_back( currentFace ); break;

			case FACE_ID::XZ_Yp:
				if( r.Y() > 0 ) possibleFaces.push_back( currentFace ); break;
			case FACE_ID::XZ_Ym:
				if( r.Y() < 0 ) possibleFaces.push_back( currentFace ); break;

			case FACE_ID::XY_Zp:
				if( r.Z() > 0 ) possibleFaces.push_back( currentFace ); break;
			case FACE_ID::XY_Zm:
				if( r.Z() < 0 ) possibleFaces.push_back( currentFace ); break;

			default: break;
		}
	}
		return possibleFaces;

}