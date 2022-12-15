/*********************************************************************
 * @file   operations.cpp
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

 #include "vec3D.h"
 #include "line.h"
 #include "surf.h"
 #include "equationSystem.h"
 #include "operations.h"



/*********************************************************************
   Implementations
*********************************************************************/


/*
	linSurf_Intersection_Result
*/

linSurf_Intersection_Result::linSurf_Intersection_Result( void )
	: hasSolution( false ),
	linPara( 0 ), surfParaA( 0 ), surfParaB( 0 ),
	isectPnt( pnt3{} ){}

string linSurf_Intersection_Result::toStr( unsigned int newLineTabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newLineTabulators; i++ ) newLine += '\t';

	str += "solution=" + hasSolution;
	str += newLine + isectPnt.toStr();
	return str;
}



/*
	rayVox_Intersection_Result
*/

rayVox_Intersection_Result::rayVox_Intersection_Result( const linSurf_Intersection_Result res_ ) :
	linSurf_Intersection_Result( res_ ),
	face( FACE_ID::INVALID )
{

}

rayVox_Intersection_Result::rayVox_Intersection_Result( void ) :
	//linSurf_Intersection_Result( linSurf_Intersection_Result{} ),
	face( FACE_ID::INVALID )
{}



/*
	rayVoxelIntersection
*/

rayVoxelIntersection::rayVoxelIntersection( const vox v_, const ray r_ ) :
	v( v_ ),
	r( r_ )
{

	// Components of ray trajectory in voxel coordinate system
	v3 comps = r.R().XYZ( v.O() );
	bool facePossible = true;

	// Find Entrance

	// Ray origin inside voxel
	if(  v.contains( r.O() ) ){
		entrance.hasSolution = true;
		entrance.linPara = 0;
		entrance.isectPnt = r.O();
		entrance.face = FACE_ID::INVALID;
	}
	else{

		// Iterate all faces
		for( FACE_ID i = FACE_ID::BEGIN; i < FACE_ID::END; ++i ){
			facePossible = true;

			// Check if face can be an entrance face of the tRay
			switch( i ){
				case FACE_ID::YZ_Xp:
				if( comps.x >= 0 ) facePossible = false; break;
				case FACE_ID::YZ_Xm:
				if( comps.x <= 0 ) facePossible = false; break;

				case FACE_ID::XZ_Yp:
				if( comps.y >= 0 ) facePossible = false; break;
				case FACE_ID::XZ_Ym:
				if( comps.y <= 0 ) facePossible = false; break;

				case FACE_ID::XY_Zp:
				if( comps.z >= 0 ) facePossible = false; break;
				case FACE_ID::XY_Zm:
				if( comps.z <= 0 ) facePossible = false; break;

				default: break;
			}

			if( facePossible ){
				// Check  if tRay intersects current face
				entrance = linSurfIntersection{ r, v.getFace( i ) }.Result();
				if( entrance.hasSolution ){
					entrance.face = i; break;
				}
			}
		}
	}


	// Find exit

	for( FACE_ID i = FACE_ID::BEGIN; i < FACE_ID::END; ++i ){
		facePossible = true;

		// Check if face can be an exit face of the tRay
		switch( i ){
			case FACE_ID::YZ_Xp:
			if( comps.x <= 0 ) facePossible = false; break;
			case FACE_ID::YZ_Xm:
			if( comps.x >= 0 ) facePossible = false; break;

			case FACE_ID::XZ_Yp:
			if( comps.y <= 0 ) facePossible = false; break;
			case FACE_ID::XZ_Ym:
			if( comps.y >= 0 ) facePossible = false; break;

			case FACE_ID::XY_Zp:
			if( comps.z <= 0 ) facePossible = false; break;
			case FACE_ID::XY_Zm:
			if( comps.z >= 0 ) facePossible = false; break;

			default: break;
		}

		if( facePossible ){
			// Check  if tRay intersects current face
			exit = linSurfIntersection{ r, v.getFace( i ) }.Result();
			if( exit.hasSolution ){
				exit.face = i; break;
			}
		}
	}
}