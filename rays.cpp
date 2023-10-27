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
	RayProperties implementation
*/

void RayProperties::AttenuateSpectrum( const voxData& voxelData, const double distance ){

	energy_spectrum_.modify(  [ & ] ( Tuple2D& spectrumPoint ) -> void {
		
		const double k = voxelData.attenuationAt( spectrumPoint.x );
		spectrumPoint.y *= exp( -k * distance );

	} );
}


/*
	Ray implementation
*/

Ray Ray::ConvertTo( const CoordinateSystem* const target ) const{
	return Ray{ this->Line::ConvertTo( target ), properties_ };
};

double Ray::GetLineParameter( const Point3D p, bool* const solution_found_ ) const{
	double t = Line::GetLineParameter( p, solution_found_ );
	*solution_found_ = *solution_found_ && ( t >= 0 );
	return t;
}

Ray Ray::ProjectOnXYPlane( const CoordinateSystem* const cSys ) const{
	return Ray{ this->Line::ProjectOnXYPlane( cSys ), this->properties_ };
}

void Ray::UpdateProperties( const voxData& data, const double distance ){
	properties_.AttenuateSpectrum( data, distance );
}

vector<FACE_ID> Ray::GetPossibleVoxelExits( void ) const{

	vector<FACE_ID> possibleFaces;

	// Iterate all faces of voxel
	for( FACE_ID currentFace = FACE_ID::BEGIN; currentFace < FACE_ID::END; ++currentFace ){

		// Check if face can be an exit_ face of the tRay
		switch( currentFace ){
			case FACE_ID::YZ_Xp:
				if( direction_.X() > 0 ) possibleFaces.push_back( currentFace ); break;
			case FACE_ID::YZ_Xm:
				if( direction_.X() < 0 ) possibleFaces.push_back( currentFace ); break;

			case FACE_ID::XZ_Yp:
				if( direction_.Y() > 0 ) possibleFaces.push_back( currentFace ); break;
			case FACE_ID::XZ_Ym:
				if( direction_.Y() < 0 ) possibleFaces.push_back( currentFace ); break;

			case FACE_ID::XY_Zp:
				if( direction_.Z() > 0 ) possibleFaces.push_back( currentFace ); break;
			case FACE_ID::XY_Zm:
				if( direction_.Z() < 0 ) possibleFaces.push_back( currentFace ); break;

			default: break;
		}
	}
		return possibleFaces;

}