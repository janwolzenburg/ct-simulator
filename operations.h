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
class linSurfIntersection : virtual public mathObj{

	linSurfIntersection( const line l_, const surf s_ );

	inline linSurf_Intersection_Result Result( void ) const { return result; };

	public:
	line l;
	surf s;
	linSurf_Intersection_Result result;
	

};