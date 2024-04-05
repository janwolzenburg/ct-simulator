#pragma once
/*********************************************************************
 * @file   line.h
 * @brief  classes for lines
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
 * @brief class for a line
*/
class Line : public MathematicalObject{

	public:

	/*!
	 * @brief cosntructor
	 * @param direction unit vector
	 * @param origin origin
	*/
	explicit Line( const Unitvector3D direction, const Point3D origin );

	/*!
	 * @brief constructor
	 * @param direction line direction
	 * @param origin origin
	*/
	explicit Line( const Vector3D direction, const Point3D origin );

	/*!
	 * @brief default constructor
	*/
	Line( void );

	/*!
	 * @brief convert line's data to string
	 * @return string with line's data
	*/
	string ConvertToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief get origin of line
	 * @return origin
	*/
	Point3D origin( void ) const { return origin_; };

	/*!
	 * @brief set origin
	 * @param new_origin new origin
	 * @return set origin
	*/
	Point3D origin( const Point3D new_origin ){ return origin_ = new_origin.ConvertTo( direction_ ); };

	/*!
	 * @brief get line trajectory
	 * @return trajectory
	*/
	Unitvector3D direction( void ) const { return direction_; };

	/*!
	 * @brief convert line components to different coordinate system
	 * @param target_coordinate_system target system
	 * @return line in target system
	*/
	Line ConvertTo( const CoordinateSystem* const target_coordinate_system ) const{ return Line{ direction_.ConvertTo( target_coordinate_system ), origin_.ConvertTo( target_coordinate_system ) }; };

	/*!
	 * @brief get point on the Line based on parameter
	 * @param line_parameter line parameter t
	 * @return point point = origin_ + direction_*t
	*/
	Point3D GetPoint( const double line_parameter ) const{ return origin_ + ( direction_ * line_parameter ); };
	
	/*!
	 * @brief get point on the Line based on parameter
	 * @details faster implemenation
	 * @param line_parameter line parameter t
	 * @return point point = origin_ + direction_*t
	 */
	Point3D GetPointFast( const double line_parameter ) const;

	/*!
	 * @brief get line parameter corresponding to point
	 * @param point_on_line point on Line
	 * @param solution_found is set to true when the given point lies on the Line. false if not
	 * @return line parameter
	*/
	double GetLineParameter( const Point3D point_on_line, bool* const solution_found ) const;

	/*!
	 * @brief get angle between Line and surface
	 * @param suface surface
	 * @return angle in radians
	*/
	double GetAngle( const Surface& suface ) const;

	/*!
	 * @brief get perpendicular vector from this Line to point point
	 * @param point Point to get the lot to
	 * @return to this line perpendicular vector connecting this line and point point
	*/
	Vector3D GetLot( const Point3D point ) const;

	/*!
	 * @brief get shortest distance between line and point
	 * @param point point
	 * @return distance in this line's unit
	*/
	double GetDistance( const Point3D point ) const{ return GetLot( point ).length(); };

	/*!
	 * @brief get shortest distance between two lines
	 * @param line line
	 * @return distance in this line's unit
	*/
	double GetDistance( const Line line ) const;

	/*!
	 * @brief project line on XY plane of coordinate system
	 * @param coordinate_system system to project on
	 * @return projected line
	*/
	Line ProjectOnXYPlane( const CoordinateSystem* const coordinate_system ) const;

	/*!
	 * @brief check if parameter is in bound
	 * @param parameter parameter
	 * @return always true for a line
	*/
	virtual bool IsParameterInBounds( [[maybe_unused]] const double parameter ) const { return true; };


	protected:

	Unitvector3D direction_;			/*!< line trajectory */
	Point3D origin_;		 					/*!< line origin_ */
};