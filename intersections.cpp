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

RayVoxelIntersection::RayVoxelIntersection( const Voxel& voxel, const Ray& ray ) :
	entrance_face_( Voxel::Face::Invalid ),
	exit_face_( Voxel::Face::Invalid )
{
	// components of ray trajectory in voxel coordinate system
	const Tuple3D local_ray_direction = ray.direction().GetComponents( voxel.origin_corner() );
	bool face_possible = true;

	// find entrance

	using Face = Voxel::Face;

	// ray origin inside voxel
	if(  voxel.Contains( ray.origin() ) ){
		entrance_.intersection_exists_ = true;
		entrance_.line_parameter_ = 0;
		entrance_.intersection_point_ = ray.origin();
		entrance_face_ = Face::Invalid;
	}
	else{

		// iterate all faces
		for( Voxel::Face i = Face::Begin; i < Face::End; ++i ){
			face_possible = true;

			// check if face can be an entrance face of the ray
			switch( i ){
				case Face::YZ_Xp:
				if( local_ray_direction.x >= 0 ) face_possible = false; break;
				case Face::YZ_Xm:
				if( local_ray_direction.x <= 0 ) face_possible = false; break;

				case Face::XZ_Yp:
				if( local_ray_direction.y >= 0 ) face_possible = false; break;
				case Face::XZ_Ym:
				if( local_ray_direction.y <= 0 ) face_possible = false; break;

				case Face::XY_Zp:
				if( local_ray_direction.z >= 0 ) face_possible = false; break;
				case Face::XY_Zm:
				if( local_ray_direction.z <= 0 ) face_possible = false; break;

				default: break;
			}

			if( face_possible ){
				// check  if ray intersects current face
				entrance_ = LineSurfaceIntersection<Ray, BoundedSurface>{ ray, voxel.GetFace( i ) };
				if( entrance_.intersection_exists_ ){
					entrance_face_ = i; break;
				}
			}
		}
	}


	// find exit

	for( Voxel::Face i = Face::Begin; i < Face::End; ++i ){
		face_possible = true;

		// check if face can be an exit face of the ray
		switch( i ){
			case Face::YZ_Xp:
			if( local_ray_direction.x <= 0 ) face_possible = false; break;
			case Face::YZ_Xm:
			if( local_ray_direction.x >= 0 ) face_possible = false; break;

			case Face::XZ_Yp:
			if( local_ray_direction.y <= 0 ) face_possible = false; break;
			case Face::XZ_Ym:
			if( local_ray_direction.y >= 0 ) face_possible = false; break;

			case Face::XY_Zp:
			if( local_ray_direction.z <= 0 ) face_possible = false; break;
			case Face::XY_Zm:
			if( local_ray_direction.z >= 0 ) face_possible = false; break;

			default: break;
		}

		if( face_possible ){
			// check  if ray intersects current face
			exit_ = LineSurfaceIntersection<Ray, BoundedSurface>{ ray, voxel.GetFace( i ) };
			if( exit_.intersection_exists_ ){
				exit_face_ = i; break;
			}
		}

	}
}