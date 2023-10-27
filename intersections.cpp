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

RayVoxelIntersection::RayVoxelIntersection( const Voxel v_, const Ray r_ )
{
	// Components of Ray trajectory in voxel coordinate system
	Tuple3D comps = r_.direction().GetComponents( v_.origin_corner() );
	bool facePossible = true;

	// Find Entrance

	using Face = Voxel::Face;

	// Ray origin_ inside voxel
	if(  v_.Contains( r_.origin() ) ){
		entrance_.intersection_exists_ = true;
		entrance_.line_parameter_ = 0;
		entrance_.intersection_point_ = r_.origin();
		entrance_face_ = Face::Invalid;
	}
	else{

		// Iterate all faces
		for( Voxel::Face i = Face::Begin; i < Face::End; ++i ){
			facePossible = true;

			// Check if face can be an entrance_ face of the tRay
			switch( i ){
				case Face::YZ_Xp:
				if( comps.x >= 0 ) facePossible = false; break;
				case Face::YZ_Xm:
				if( comps.x <= 0 ) facePossible = false; break;

				case Face::XZ_Yp:
				if( comps.y >= 0 ) facePossible = false; break;
				case Face::XZ_Ym:
				if( comps.y <= 0 ) facePossible = false; break;

				case Face::XY_Zp:
				if( comps.z >= 0 ) facePossible = false; break;
				case Face::XY_Zm:
				if( comps.z <= 0 ) facePossible = false; break;

				default: break;
			}

			if( facePossible ){
				// Check  if tRay intersects current face
				entrance_ = LineSurfaceIntersection<Ray, BoundedSurface>{ r_, v_.GetFace( i ) };
				if( entrance_.intersection_exists_ ){
					entrance_face_ = i; break;
				}
			}
		}
	}


	// Find exit_

	for( Voxel::Face i = Face::Begin; i < Face::End; ++i ){
		facePossible = true;

		// Check if face can be an exit_ face of the tRay
		switch( i ){
			case Face::YZ_Xp:
			if( comps.x <= 0 ) facePossible = false; break;
			case Face::YZ_Xm:
			if( comps.x >= 0 ) facePossible = false; break;

			case Face::XZ_Yp:
			if( comps.y <= 0 ) facePossible = false; break;
			case Face::XZ_Ym:
			if( comps.y >= 0 ) facePossible = false; break;

			case Face::XY_Zp:
			if( comps.z <= 0 ) facePossible = false; break;
			case Face::XY_Zm:
			if( comps.z >= 0 ) facePossible = false; break;

			default: break;
		}

		if( facePossible ){
			// Check  if tRay intersects current face
			exit_ = LineSurfaceIntersection<Ray, BoundedSurface>{ r_, v_.GetFace( i ) };
			if( exit_.intersection_exists_ ){
				exit_face_ = i; break;
			}
		}
	}
}