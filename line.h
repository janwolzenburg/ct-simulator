#pragma once
/*********************************************************************
 * @file   line.h
 * @brief  Classes for lines
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



 /*********************************************************************
	Includes
 *********************************************************************/
#include "generelMath.h"
#include "vec3D.h"



/*********************************************************************
   Definitions
*********************************************************************/


/*!
 * @brief Class for a line
*/
class line : virtual public mathObj{

	public:

	/*!
	 * @brief Constructor
	 * @param v Line direction
	 * @param p O
	*/
	explicit line( const vec3 v, const pnt3 p );

	/*!
	 * @brief Convert line's data to string
	 * @return String with line's data
	*/
	string toStr( const unsigned int newLineTabulators = 0 ) const override;

	/*!
	 * @brief Get origin of line
	 * @return O
	*/
	pnt3 O( void ) const;//{ return o; };

	/*!
	 * @brief Get line trajectory
	 * @return Trajectory
	*/
	uvec3 R( void ) const;//{ return r; };

	/*!
	 * @brief Convert line components to different coordinate system
	 * @param target Target system
	 * @return Line in target system
	*/
	line convertTo( const cartCSys* const target ) const;

	/*!
	 * @brief Get point on the line based on parameter
	 * @param linPara Line parameter t
	 * @return Point p = o + r*t
	*/
	pnt3 getPnt( const double linPara ) const;

	/*!
	 * @brief Get line parameter corresponding to point
	 * @param p Point on line
	 * @param success Is set to true when the given point lies on the line. False if not
	 * @return Line parameter
	*/
	double getPara( const pnt3 p, bool* const success ) const;

	/*!
	 * @brief Get angle between line and surface
	 * @param s Surface
	 * @return Angle in radians
	*/
	double getAngle( surf s ) const;

	/*!
	 * @brief Get perpendicular vector from this line to point p
	 * @param p Point to get the lot to
	 * @return To this line perpendicular vector connecting this line and point p
	*/
	vec3 getLot( const pnt3 p ) const;

	/*!
	 * @brief Get shortest distance between line and point
	 * @param p Point
	 * @return Distance in this line's unit
	*/
	double getDistance( const pnt3 p ) const;

	/*!
	 * @brief Get shortest distance between two lines
	 * @param l Line
	 * @return Distance in this line's unit
	*/
	double getDistance( const line l ) const;

	/*!
	 * @brief Calculate intersection between this line and surface
	 * @param sur Surface to intersect with
	 * @return Instersection result
	*/
	//linSurf_Intersection_Result isectSurf( const surfLim sur ) const;

	/*!
	 * @brief Project line on XY plane of coordinate system
	 * @param cSys System to project on
	 * @return Projected line
	*/
	line projectOnXYPlane( const cartCSys* const cSys ) const;

	/*!
	 * @brief Calculate radon coords of line in given coordinate system
	 * @param cSys Coordinate system
	 * @return Radon coordinates
	 * @details Gives the angle and distance of this line's XY plane-projected lot to origin
	*/
	//radonPoint getRadonCoords( const cartCSys* const cSys ) const;

	inline bool paraInBounds( const double para ) const { return true; };

	protected:
	uvec3 r;			/*!< Line trajectory */
	pnt3 o;		 	/*!< Line origin */
};