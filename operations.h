#pragma once
/*********************************************************************
 * @file   operations.h
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

	public:
	linSurfIntersection( const L l_, const S s_ );

	inline linSurf_Intersection_Result Result( void ) const { return result; };

	public:
	L l;
	S s;
	linSurf_Intersection_Result result;
	

};



/*!
 * @brief Class describing iuntersection of one voxel face with ray
*/
class rayVox_Intersection_Result : public linSurf_Intersection_Result{

	//using linSurf_Intersection_Result::linSurf_Intersection_Result;

	public:

	/*!
	 * @brief Constructor
	 * @param res_ Instance of parent class
	*/
	rayVox_Intersection_Result( const linSurf_Intersection_Result res_ );

	rayVox_Intersection_Result( void );

	public:
	FACE_ID face;	/*!<ID of face for which the result is valid*/
};


/*!
 * @brief Class for calculation the intersection of a line and a surface
*/
class rayVoxelIntersection {

	public:
	rayVoxelIntersection( const vox v_, const ray r_ );

	inline rayVox_Intersection_Result Entrance( void ) const{ return entrance; };

	inline rayVox_Intersection_Result Exit( void ) const{ return exit; };

	public:
	vox v;
	ray r;
	rayVox_Intersection_Result entrance;
	rayVox_Intersection_Result exit;

};

#include "operations.hpp"