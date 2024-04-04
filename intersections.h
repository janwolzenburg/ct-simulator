#pragma once
/*********************************************************************
 * @file   intersections.h
 * @brief  classes for operations on mathematical objects
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
 * @brief class describing the intersection result of a Line and surface
*/
template<class L, class S>
class LineSurfaceIntersection : public MathematicalObject{

	// assert base classes
	static_assert( std::is_base_of_v< Surface, S > );
	static_assert( std::is_base_of_v< Line, L > );


	public:

	/*!
	 * @brief default constructor
	*/
	LineSurfaceIntersection( const L& line, const S& surface );

	/*!
	 * @brief default constructor
	*/
	LineSurfaceIntersection( void );

	/*!
	 * @brief convert result's data to string
	 * @return string with result's data
	*/
	string ConvertToString( const unsigned int newline_tabulators = 0 ) const override;


	Point3D intersection_point_;				/*!< point of intersection */
	bool intersection_exists_;					/*!< Line intersects surface */
	double line_parameter_;						/*!< Line parameter where intersection occurs */
	double surface_parameter_1_;				/*!< surface parameter a where intersection occurs */
	double surface_parameter_2_;				/*!< surface parameter b where intersection occurs */

};


/*!
 * @brief class for calculation the intersection of a Line and a surface
*/
class RayVoxelIntersection {

	public:

	/*!
	 * @brief constructor
	 * @param v_ Voxel
	 * @param r_ Ray
	*/
	RayVoxelIntersection( const Voxel& voxel, const Ray& ray );


	Voxel::Face entrance_face_;										/*!< entrance face*/
	LineSurfaceIntersection<Ray, BoundedSurface> entrance_;		/*!< entrance*/

	Voxel::Face exit_face_;										/*!< exit face*/
	LineSurfaceIntersection<Ray, BoundedSurface> exit_;		/*!Exit*/

};

#include "intersections.hpp"