#pragma once
/*********************************************************************
 * @file   Line.h
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
 * @brief Class for a Line
*/
class Line : public MathematicalObject{

	public:

	/*!
	 * @brief Cosntructor
	 * @param direction Unit vector
	 * @param origin Origin
	*/
	explicit Line( const UnitVector3D direction, const Point3D origin );

	/*!
	 * @brief Constructor
	 * @param direction Line direction
	 * @param origin Origin
	*/
	explicit Line( const Vector3D direction, const Point3D origin );

	/*!
	 * @brief Default constructor
	*/
	Line( void );

	/*!
	 * @brief Convert Line's data_ to string
	 * @return String with Line's data_
	*/
	string ToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief Get origin_ of Line
	 * @return Origin
	*/
	Point3D origin( void ) const { return origin_; };

	/*!
	 * @brief Set origin_
	 * @param new_origin New origin
	 * @return Set origin
	*/
	Point3D origin( const Point3D new_origin ){ return origin_ = new_origin.ConvertTo( direction_ ); };

	/*!
	 * @brief Get Line trajectory
	 * @return Trajectory
	*/
	UnitVector3D direction( void ) const { return direction_; };

	/*!
	 * @brief Convert Line components to different coordinate system
	 * @param target_coordinate_system Target system
	 * @return Line in target system
	*/
	Line ConvertTo( const CoordinateSystem* const target_coordinate_system ) const{ return Line{ direction_.ConvertTo( target_coordinate_system ), origin_.ConvertTo( target_coordinate_system ) }; };

	/*!
	 * @brief Get point on the Line based on parameter
	 * @param line_parameter Line parameter t
	 * @return Point point = origin_ + direction_*t
	*/
	Point3D GetPoint( const double line_parameter ) const{ return origin_ + ( direction_ * line_parameter ); };
	
	/*!
	 * @brief Get Line parameter corresponding to point
	 * @param point_on_line Point on Line
	 * @param solution_found Is set to true when the given point lies on the Line. False if not
	 * @return Line parameter
	*/
	double GetLineParameter( const Point3D point_on_line, bool* const solution_found ) const;

	/*!
	 * @brief Get angle between Line and surface
	 * @param suface Surface
	 * @return Angle in radians
	*/
	double GetAngle( Surface suface ) const;

	/*!
	 * @brief Get perpendicular vector from this Line to point point
	 * @param point Point to get the lot to
	 * @return To this Line perpendicular vector connecting this Line and point point
	*/
	Vector3D GetLot( const Point3D point ) const;

	/*!
	 * @brief Get shortest distance between Line and point
	 * @param point Point
	 * @return Distance in this Line's unit
	*/
	double GetDistance( const Point3D point ) const{ return GetLot( point ).length(); };

	/*!
	 * @brief Get shortest distance between two lines
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
	 * @brief Check if parameters are in bound
	 * @param parameter Parameter
	 * @return Always true
	*/
	virtual bool IsParameterInBounds( [[maybe_unused]] const double voxel_data_ ) const { return true; };


	protected:

	UnitVector3D direction_;			/*!< Line trajectory */
	Point3D origin_;		 		/*!< Line origin_ */
};