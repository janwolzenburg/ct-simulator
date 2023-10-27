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

 #include "systemOfEquations.h"
 #include "intersections.h"



/*********************************************************************
   Implementations
*********************************************************************/



/*
	RayVoxelIntersection
*/

RayVoxelIntersection::RayVoxelIntersection( const vox v_, const ray r_ )
{
	// Components of ray trajectory in voxel coordinate system
	Tuple3D comps = r_.direction().GetComponents( v_.O() );
	bool facePossible = true;

	// Find Entrance

	// Ray origin_ inside voxel
	if(  v_.contains( r_.origin() ) ){
		entrance_.intersection_exists_ = true;
		entrance_.line_parameter_ = 0;
		entrance_.intersection_point_ = r_.origin();
		entrance_face_ = FACE_ID::INVALID;
	}
	else{

		// Iterate all faces
		for( FACE_ID i = FACE_ID::BEGIN; i < FACE_ID::END; ++i ){
			facePossible = true;

			// Check if face can be an entrance_ face of the tRay
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
				entrance_ = LineSurfaceIntersection<ray, BoundedSurface>{ r_, v_.getFace( i ) };
				if( entrance_.intersection_exists_ ){
					entrance_face_ = i; break;
				}
			}
		}
	}


	// Find exit_

	for( FACE_ID i = FACE_ID::BEGIN; i < FACE_ID::END; ++i ){
		facePossible = true;

		// Check if face can be an exit_ face of the tRay
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
			exit_ = LineSurfaceIntersection<ray, BoundedSurface>{ r_, v_.getFace( i ) };
			if( exit_.intersection_exists_ ){
				exit_face_ = i; break;
			}
		}
	}
}