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
#include<string>
using std::string;
#include <type_traits>

#include "generelMath.h"
#include "vec3D.h"
#include "line.h"
#include "surf.h"
#include "rays.h"
#include "voxel.h"


 /*********************************************************************
	Definitions
 *********************************************************************/

/*!
 * @brief Class describing the intersection result of a line and surface
*/
class linSurf_Intersection_Result : virtual public mathObj{

	public:
	pnt3 isectPnt;							/*!< Point of intersection */
	bool hasSolution;						/*!< Line intersects surface */
	double linPara;							/*!< Line parameter where intersection occurs */
	double surfParaA;						/*!< Surface parameter a where intersection occurs */
	double surfParaB;						/*!< Surface parameter b where intersection occurs */


	public:

	/*!
	 * @brief Default constructor
	*/
	linSurf_Intersection_Result( void );

	/*!
	 * @brief Convert result's data to string
	 * @return String with result's data
	*/
	string toStr( const unsigned int newLineTabulators = 0 ) const override;

};



/*!
 * @brief Class for calculation the intersection of a line and a surface
*/
template<class L, class S>
class linSurfIntersection {

	static_assert( std::is_base_of_v< surf, S > );
	static_assert( std::is_base_of_v< line, L > );

	public:
	/*!
	 * @brief Constructor
	 * @param l_ Object derived from class line
	 * @param s_ Object derived from class surf
	*/
	linSurfIntersection( const L l_, const S s_ );

	/*!
	 * @brief Get the intersetion result
	 * @return Intersection result
	*/
	inline linSurf_Intersection_Result Result( void ) const { return result; };


	public:
	L l;									/*!<Line*/
	S s;									/*!<Surface*/
	linSurf_Intersection_Result result;		/*!<Calculated result*/
	

};



/*!
 * @brief Class describing iuntersection of one voxel face with ray
*/
class rayVox_Intersection_Result : public linSurf_Intersection_Result{

	public:

	/*!
	 * @brief Constructor
	 * @param res_ Instance of parent class
	*/
	rayVox_Intersection_Result( const linSurf_Intersection_Result res_ );

	/*!
	 * @brief Default constructor
	*/
	rayVox_Intersection_Result( void );

	public:
	FACE_ID face;	/*!<ID of face for which the result is valid*/
};



/*!
 * @brief Class for calculation the intersection of a line and a surface
*/
class rayVoxelIntersection {

	public:
	/*!
	 * @brief Constructor
	 * @param v_ Voxel
	 * @param r_ Ray
	*/
	rayVoxelIntersection( const vox v_, const ray r_ );

	/*!
	 * @brief Get entrance into voxel
	 * @return Intersection result of entrance
	*/
	inline rayVox_Intersection_Result Entrance( void ) const{ return entrance; };

	/*!
	 * @brief Get exit into voxel
	 * @return Intersection result of exit
	*/
	inline rayVox_Intersection_Result Exit( void ) const{ return exit; };


	public:
	vox v;										/*!<Voxel*/
	ray r;										/*!<Ray*/
	rayVox_Intersection_Result entrance;		/*!<Entrance*/
	rayVox_Intersection_Result exit;			/*!Exit*/

};

#include "intersections.hpp"