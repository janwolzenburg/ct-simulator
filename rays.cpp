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
#include <string>
using std::string;

#include "rays.h"
#include "cSysTree.h"

/*********************************************************************
   Implementations
*********************************************************************/

/*
	ray implementation
*/

ray::ray( const vec3 v_, const pnt3 p_, const rayProperties properties_ )
	: line( v_, p_ ),
	properties( properties_ ){};

ray::ray( const line line_, const rayProperties  properties_ )
	: line( line_ ),
	properties( properties_ ){};

ray::ray( void ) :
	ray(vec3{ v3{ 1, 0, 0 }, DUMMY_CSYS()}, pnt3{v3{ 0, 0, 0}, DUMMY_CSYS() }, rayProperties{})
{

}

ray ray::convertTo( const cartCSys* const target ) const{
	return ray{ this->line::convertTo( target ), properties };
};

double ray::getPara( const pnt3 p, bool* const success ) const{
	double t = line::getPara( p, success );
	*success = *success && ( t >= 0 );
	return t;
}

ray ray::projectOnXYPlane( const cartCSys* const cSys ) const{
	return ray{ this->line::projectOnXYPlane( cSys ), this->properties };
}

void ray::updateProperties( const voxData data, const double distance ){

	properties.attenuateSpectrum( data.kAtRefE(), distance );


	// Equally scale powerspectrum
	//scaleSpectrum( exp( -data.k * distance ) );

}

bool ray::paraInBounds(const double para) const { return para >= 0; };



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

void ray::scaleSpectrum( const double factor ){



	properties.powerSpectrum.scale( factor );
}

double ray::getMeanFrequency( void ) const{
	return properties.powerSpectrum.getMean();
}

rayProperties::rayProperties( const spectrum spectrum_ ) : powerSpectrum( spectrum_ ){};
rayProperties::rayProperties( void ) : powerSpectrum( spectrum{} ){};

spectrum rayProperties::PowerSpectrum( void ) const{
	return powerSpectrum;
}


void rayProperties::attenuateSpectrum( const double kAtRefE, const double distance ){

	powerSpectrum.attenuate( kAtRefE, distance);

}