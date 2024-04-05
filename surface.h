#pragma once
/*********************************************************************
 * @file   Surface.h
 * @brief  classes for surfaces
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
 * @brief class for unconfined surfaces
*/
class Surface : public MathematicalObject{

	public:

	/*!
	 * @brief constructor
	 * @param direction_1 First direction vector
	 * @param direction_2 second direction vector. Must be orthogonal to first vector
	 * @param origin origin
	*/
	explicit Surface( const Unitvector3D direction_1, const Unitvector3D  direction_2, const Point3D  origin );

	/*!
	 * @brief construct from binary data
	 * @param binary_data data vector
	 * @param current_byte iterator
	 * @param coordinate_system system to assign surface to
	*/
	Surface( const vector<char>& binary_data, vector<char>::const_iterator& current_byte, CoordinateSystem* coordinate_system );

	/*!
	 * @brief default constructor
	*/
	Surface( void );

	/*!
	 * @brief convert surface's data to string
	 * @param newline_tabulators amount of tabulators to insert after each line break
	 * @return string with surface's data
	*/
	string ConvertToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief serialize this instance
	 * @param binary_data data to write to
	 * @return bytes written
	 */
	size_t Serialize( vector<char>& binary_data ) const;

	/*!
	 * @brief get origin of surface
	 * @return origing
	*/
	Point3D origin( void ) const{ return origin_; };

	/*!
	 * @brief get first direction vector
	 * @return direction_1 of surface
	*/
	Unitvector3D  direction_1( void ) const{ return direction_1_; };

	/*!
	 * @brief get second direction vector
	 * @return direction_2 of surface
	*/
	Unitvector3D  direction_2( void ) const{ return direction_2_; };

	/*!
	 * @brief get point on the surface based on parameters
	 * @param surface_parameter_1 surface parameter a
	 * @param surface_parameter_2 surface parameter b
	 * @return point p = origin_ + direction_1_*a + direction_2_*b
	*/
	Point3D GetPoint( const double surface_parameter_1, const double surface_parameter_2 ) const{ return  origin_ + ( direction_1_ * surface_parameter_1 + direction_2_ * surface_parameter_2 ); };

	/*!
	 * @brief get surface normal by cross product
	 * @return surface normal
	*/
	Unitvector3D GetNormal( void ) const{ return normal_; };

	/*!
	 * @brief convert surface to different coordinate system
	 * @param coordinate_system system to convert to
	 * @return converted surface
	*/
	Surface ConvertTo( const CoordinateSystem* const coordinate_system ) const;

	/*!
	 * @brief checks if parameters are inside surface bounds
	 * @param parameter_1 surface parameter 1
	 * @param parameter_2 surface parameter 2
	 * @return always true for unconfined surfaces
	*/
	virtual bool AreParametersInBounds( [[maybe_unused]] const double parameter_1, [[maybe_unused]] const double parameter_2 ) const { return true; };


	protected:
	Unitvector3D  direction_1_;			/*!< first direction vector*/
	Unitvector3D  direction_2_;			/*!< second direction vector*/
	Point3D  origin_;								/*!< origin*/
	Unitvector3D normal_;						/*!< the surfaces normal*/

};



/*!
 * @brief confined surface with limited parameters creating a rectangle
*/
class BoundedSurface : public Surface{

	public:

	/*!
	 * @brief constructor
	 * @param direction_1 first direction vector
	 * @param direction_2 second direction vector. Must be orthogonal to first vector
	 * @param origin origin
	 * @param parameter_1_range limits for parameter a
	 * @param parameter_2_range limits for parameter b
	*/
	explicit BoundedSurface( const Unitvector3D  direction_1, const Unitvector3D  direction_2, const Point3D  origin,
							 const NumberRange parameter_1_range, const NumberRange parameter_2_range );

	/*!
	 * @brief constructor
	 * @param direction_1 dirst direction vector
	 * @param direction_2 second direction vector. Must be orthogonal to first vector
	 * @param origin origin
	 * @param parameter_1_min lower bound for parameter 1
	 * @param parameter_1_max upper bound for parameter 1
	 * @param parameter_2_min lower bound for parameter 2
	 * @param parameter_2_max upper bound for parameter 2
	*/
	explicit BoundedSurface( const Unitvector3D  direction_1, const Unitvector3D  direction_2, const Point3D  origin,
							 const double parameter_1_min, const double parameter_1_max,
							 const double parameter_2_min, const double parameter_2_max );

	/*!
	 * @brief constructor
	 * @param surface base surface
	 * @param parameter_1_range limits for parameter a
	 * @param parameter_2_range limits for parameter b
	*/
	explicit BoundedSurface( const Surface surface,
							 const NumberRange parameter_1_range,  const NumberRange parameter_2_range );

	/*!
	 * @brief constructor
	 * @param surface base surface
	 * @param parameter_1_min lower bound for parameter 1
	 * @param parameter_1_max upper bound for parameter 1
	 * @param parameter_2_min lower bound for parameter 2
	 * @param parameter_2_max upper bound for parameter 2
	*/
	explicit BoundedSurface( const Surface surface,
							 const double parameter_1_min, const double parameter_1_max,
							 const double parameter_2_min, const double parameter_2_max );

	/*!
	 * @brief default constructor
	*/
	BoundedSurface( void ) : BoundedSurface( Surface{}, NumberRange{ 0., 1. }, NumberRange{ 0., 1. } ){};

	/*!
	 * @brief construct from binary data
	 * @param binary_data data vector
	 * @param current_byte iterator
	 * @param coordinate_system dystem to assign surface to
	*/
	BoundedSurface( const vector<char>& binary_data, vector<char>::const_iterator& current_byte, CoordinateSystem* coordinate_system );

	/*!
	 * @brief convert surface's data to string
	 * @param newline_tabulators amount of tabulators to insert after each line break
	 * @return string with surface's data
	*/
	std::string ConvertToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief serialize this instance
	 * @param binary_data data to write to
	 * @return bytes written
	 */
	size_t Serialize( vector<char>& binary_data ) const;

	/*!
	 * @brief get minimum of parameter a
	 * @return value of parameter 1 minimum
	*/
	double parameter_1_min( void ) const{ return parameter_1_range_.start(); };

	/*!
	 * @brief get maximum of parameter a
	 * @return value of parameter 1 maximum
	*/
	double parameter_1_max( void ) const{ return parameter_1_range_.end(); };

	/*!
	 * @brief get minimum of parameter b
	 * @return value of parameter 2 minimum
	*/
	double parameter_2_min( void ) const{ return parameter_2_range_.start(); };

	/*!
	 * @brief get maximum of parameter b
	 * @return value of parameter 2 maximum
	*/
	double parameter_2_max( void ) const{ return parameter_2_range_.end(); };

	/*!
	 * @brief convert surface to different coordinate system
	 * @param coordinate_system System to convert to
	 * @return converted surface
	*/
	BoundedSurface ConvertTo( const CoordinateSystem* const coordinate_system ) const{ return BoundedSurface{ this->Surface::ConvertTo( coordinate_system ), parameter_1_range_, parameter_2_range_ }; };

	/*!
	 * @brief checks if parameters are inside surface bounds
	 * @param parameter_1 surface parameter a
	 * @param parameter_2 surface parameter b
	 * @return true when parameters are inside surface bounds
	*/
	bool AreParametersInBounds( const double parameter_1, const double parameter_2 ) const override{ 
		return parameter_1_min() <= parameter_1 && parameter_1 <= parameter_1_max() && parameter_2_min() <= parameter_2 && parameter_2 <= parameter_2_max();  };

	/*!
	 * @brief get center point of limited surface
	 * @return the center point
	*/
	Point3D  GetCenter( void ) const{ return this->GetPoint( ( parameter_1_max() + parameter_1_min() ) / 2, ( parameter_2_max() + parameter_2_min() ) / 2); };

	/*!
	 * @brief get the surfaces normal as Line through its center
	 * @return normal Line
	*/
	Line  GetCenterNormal( void ) const;

	
	private:
	
	NumberRange parameter_1_range_;		/*!< bounds for parameter 1*/
	NumberRange parameter_2_range_;		/*!< bounds for parameter 2*/
	
};
