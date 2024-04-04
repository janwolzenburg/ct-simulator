#pragma once
/*********************************************************************
 * @file   Line.h
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
 * @brief class for a Line
*/
class Line : public MathematicalObject{

	public:

	/*!
	 * @brief cosntructor
	 * @param direction Unit vector
	 * @param origin Origin
	*/
	explicit Line( const UnitVector3D direction, const Point3D origin );

	/*!
	 * @brief constructor
	 * @param direction Line direction
	 * @param origin Origin
	*/
	explicit Line( const Vector3D direction, const Point3D origin );

	/*!
	 * @brief default constructor
	*/
	Line( void );

	/*!
	 * @brief convert Line's data to string
	 * @return string with Line's data
	*/
	string ToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief get origin_ of Line
	 * @return Origin
	*/
	Point3D origin( void ) const { return origin_; };

	/*!
	 * @brief set origin_
	 * @param new_origin New origin
	 * @return set origin
	*/
	Point3D origin( const Point3D new_origin ){ return origin_ = new_origin.ConvertTo( direction_ ); };

	/*!
	 * @brief get Line trajectory
	 * @return trajectory
	*/
	UnitVector3D direction( void ) const { return direction_; };

	/*!
	 * @brief convert Line components to different coordinate system
	 * @param target_coordinate_system Target system
	 * @return Line in target system
	*/
	Line ConvertTo( const CoordinateSystem* const target_coordinate_system ) const{ return Line{ direction_.ConvertTo( target_coordinate_system ), origin_.ConvertTo( target_coordinate_system ) }; };

	/*!
	 * @brief get point on the Line based on parameter
	 * @param line_parameter Line parameter t
	 * @return Point point = origin_ + direction_*t
	*/
	Point3D GetPoint( const double line_parameter ) const{ return origin_ + ( direction_ * line_parameter ); };
	
	Point3D GetPointFast( const double line_parameter ) const;

	/*!
	 * @brief get Line parameter corresponding to point
	 * @param point_on_line Point on Line
	 * @param solution_found Is set to true when the given point lies on the Line. False if not
	 * @return Line parameter
	*/
	double GetLineParameter( const Point3D point_on_line, bool* const solution_found ) const;

	/*!
	 * @brief get angle between Line and surface
	 * @param suface Surface
	 * @return angle in radians
	*/
	double GetAngle( const Surface& suface ) const;

	/*!
	 * @brief get perpendicular vector from this Line to point point
	 * @param point Point to get the lot to
	 * @return to this Line perpendicular vector connecting this Line and point point
	*/
	Vector3D GetLot( const Point3D point ) const;

	/*!
	 * @brief get shortest distance between Line and point
	 * @param point Point
	 * @return Distance in this Line's unit
	*/
	double GetDistance( const Point3D point ) const{ return GetLot( point ).length(); };

	/*!
	 * @brief get shortest distance between two lines
	 * @param line Line
	 * @return Distance in this Line's unit
	*/
	double GetDistance( const Line line ) const;

	/*!
	 * @brief Project Line on XY plane of coordinate system
	 * @param coordinate_system System to project on
	 * @return Projected Line
	*/
	Line ProjectOnXYPlane( const CoordinateSystem* const coordinate_system ) const;

	/*!
	 * @brief check if parameters are in bound
	 * @param parameter Parameter
	 * @return always true
	*/
	virtual bool IsParameterInBounds( [[maybe_unused]] const double voxel_data_ ) const { return true; };


	protected:

	UnitVector3D direction_;			/*!< Line trajectory */
	Point3D origin_;		 		/*!< Line origin_ */
};