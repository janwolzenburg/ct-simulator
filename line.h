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
class line : public mathObj{

	public:


	explicit line( const uvec3 v, const pnt3 p );

	/*!
	 * @brief Constructor
	 * @param v Line direction
	 * @param p O
	*/
	explicit line( const vec3 v, const pnt3 p );


	/*!
	 * @brief Default constructor
	 * @param  
	*/
	line( void );

	/*!
	 * @brief Convert line's data to string
	 * @return String with line's data
	*/
	string toStr( const unsigned int newLineTabulators = 0 ) const override;

	/*!
	 * @brief Get origin of line
	 * @return O
	*/
	pnt3 O( void ) const { return o; };

	/*!
	 * @brief Set origin
	 * @param newO New origin
	 * @return Set origin
	*/
	pnt3 setOrigin( const pnt3 newO ){ return o = newO.convertTo( r ); };

	/*!
	 * @brief Get line trajectory
	 * @return Trajectory
	*/
	uvec3 R( void ) const { return r; };

	/*!
	 * @brief Convert line components to different coordinate system
	 * @param target Target system
	 * @return Line in target system
	*/
	line convertTo( const cartCSys* const target ) const{ return line{ r.convertTo( target ), o.convertTo( target ) }; };

	/*!
	 * @brief Get point on the line based on parameter
	 * @param linPara Line parameter t
	 * @return Point p = o + r*t
	*/
	pnt3 getPnt( const double linPara ) const{ return o + ( r * linPara ); };
	

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
	double getDistance( const pnt3 p ) const{ return getLot( p ).Length(); };

	/*!
	 * @brief Get shortest distance between two lines
	 * @param l Line
	 * @return Distance in this line's unit
	*/
	double getDistance( const line l ) const;

	/*!
	 * @brief Project line on XY plane of coordinate system
	 * @param cSys System to project on
	 * @return Projected line
	*/
	line projectOnXYPlane( const cartCSys* const cSys ) const;

	/*!
	 * @brief Check if parameters are in bound
	 * @param para Parameter
	 * @return Always true
	*/
	virtual bool paraInBounds( [[maybe_unused]] const double para ) const { return true; };


	protected:

	uvec3 r;			/*!< Line trajectory */
	pnt3 o;		 		/*!< Line origin */
};