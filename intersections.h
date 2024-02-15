#pragma once
/*********************************************************************
 * @file   intersections.h
 * @brief  Classes for operations on mathematical objects
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/
#include <type_traits>

#include "generelMath.h"
#include "vector3D.h"
#include "line.h"
#include "surface.h"
#include "ray.h"
#include "voxel.h"


 /*********************************************************************
	Definitions
 *********************************************************************/


/*!
 * @brief Class describing the intersection result of a Line and surface
*/
template<class L, class S>
class LineSurfaceIntersection : public MathematicalObject{

	// Assert base classes
	static_assert( std::is_base_of_v< Surface, S > );
	static_assert( std::is_base_of_v< Line, L > );


	public:

	/*!
	 * @brief Default constructor
	*/
	LineSurfaceIntersection( const L line, const S surface );

	/*!
	 * @brief Default constructor
	*/
	LineSurfaceIntersection( void );

	/*!
	 * @brief Convert result's data to string
	 * @return String with result's data
	*/
	string ToString( const unsigned int newline_tabulators = 0 ) const override;


	Point3D intersection_point_;				/*!< Point of intersection */
	bool intersection_exists_;					/*!< Line intersects surface */
	double line_parameter_;						/*!< Line parameter where intersection occurs */
	double surface_parameter_1_;				/*!< Surface parameter a where intersection occurs */
	double surface_parameter_2_;				/*!< Surface parameter b where intersection occurs */

};


/*!
 * @brief Class for calculation the intersection of a Line and a surface
*/
class RayVoxelIntersection {

	public:

	/*!
	 * @brief Constructor
	 * @param v_ Voxel
	 * @param r_ Ray
	*/
	RayVoxelIntersection( const Voxel& voxel, const Ray& ray );


	Voxel::Face entrance_face_;										/*!< Entrance face*/
	LineSurfaceIntersection<Ray, BoundedSurface> entrance_;		/*!< Entrance*/

	Voxel::Face exit_face_;										/*!< Exit face*/
	LineSurfaceIntersection<Ray, BoundedSurface> exit_;		/*!Exit*/

};

#include "intersections.hpp"