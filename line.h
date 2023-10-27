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
#include "vector3D.h"



/*********************************************************************
   Definitions
*********************************************************************/


/*!
 * @brief Class for a line
*/
class line : public MathematicalObject{

	public:

	/*!
	 * @brief Cosntructor
	 * @param v Unit vector
	 * @param p Origin
	*/
	explicit line( const UnitVector3D v, const Point3D p );

	/*!
	 * @brief Constructor
	 * @param v Line direction
	 * @param p Origin
	*/
	explicit line( const Vector3D v, const Point3D p );


	/*!
	 * @brief Default constructor
	*/
	line( void );

	/*!
	 * @brief Convert line's data to string
	 * @return String with line's data
	*/
	string ToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief Get origin_ of line
	 * @return Origin
	*/
	Point3D O( void ) const { return o; };

	/*!
	 * @brief Set origin_
	 * @param newO New origin_
	 * @return Set origin_
	*/
	Point3D setOrigin( const Point3D newO ){ return o = newO.ConvertTo( r ); };

	/*!
	 * @brief Get line trajectory
	 * @return Trajectory
	*/
	UnitVector3D R( void ) const { return r; };

	/*!
	 * @brief Convert line components to different coordinate system
	 * @param target Target system
	 * @return Line in target system
	*/
	line convertTo( const CoordinateSystem* const target ) const{ return line{ r.ConvertTo( target ), o.ConvertTo( target ) }; };

	/*!
	 * @brief Get point on the line based on parameter
	 * @param linPara Line parameter t
	 * @return Point p = o + r*t
	*/
	Point3D getPnt( const double linPara ) const{ return o + ( r * linPara ); };
	

	/*!
	 * @brief Get line parameter corresponding to point
	 * @param p Point on line
	 * @param success Is set to true when the given point lies on the line. False if not
	 * @return Line parameter
	*/
	double getPara( const Point3D p, bool* const success ) const;

	/*!
	 * @brief Get GetAngle between line and surface
	 * @param s Surface
	 * @return Angle in radians
	*/
	double getAngle( surf s ) const;

	/*!
	 * @brief Get perpendicular vector from this line to point p
	 * @param p Point to get the lot to
	 * @return To this line perpendicular vector connecting this line and point p
	*/
	Vector3D getLot( const Point3D p ) const;

	/*!
	 * @brief Get shortest distance between line and point
	 * @param p Point
	 * @return Distance in this line's unit
	*/
	double getDistance( const Point3D p ) const{ return getLot( p ).length(); };

	/*!
	 * @brief Get shortest distance between two lines
	 * @param l Line
	 * @return Distance in this line's unit
	*/
	double getDistance( const line l ) const;

	/*!
	 * @brief Project line on XY plane of coordinate system
	 * @param coordinate_system_ System to project on
	 * @return Projected line
	*/
	line projectOnXYPlane( const CoordinateSystem* const cSys ) const;

	/*!
	 * @brief Check if parameters are in bound
	 * @param para Parameter
	 * @return Always true
	*/
	virtual bool paraInBounds( [[maybe_unused]] const double para ) const { return true; };


	protected:

	UnitVector3D r;			/*!< Line trajectory */
	Point3D o;		 		/*!< Line origin_ */
};